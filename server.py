from ctypes import sizeof
import socket
import struct
import time
import codecs
from tkinter import Pack
from turtle import update

from numpy import number, uint

#region exception
class InvalidPacketSize(Exception):
    pass


class DosAttemptDetected(Exception):
    pass

class InvalidAuthField(Exception):
    pass
#endregion






class Packet:
    def __init__(self,type,format,dimension):
        self.type = type
        self.format = format
        self.dimension = dimension

#packet type (type_id,format,dimension) second int is player_id
P_DISCONNECTED = Packet("A","sI",8)
P_KEEP_ALIVE = Packet("B","sI",8)
P_POSITION = Packet("C","sIff",16)
P_HANDSHACKING = Packet("D","sI",8)


last_auth = 0

class Player:

    def __init__(self, signature):
        global last_auth
        self.signature = signature
        self.x = 0
        self.y = 0
        self.last_update = time.time()
        self.auth = last_auth + 1
        last_auth = self.auth
        self.warning = 0
        self.last_warning = None
        print("new user " + str(self.signature) + " with auth " + str(self.auth))
        




    def check_auth(self,auth):
        if self.auth is None:
            print("none")
            raise InvalidAuthField()
        elif self.auth != auth:
            print('self auth {0} autth {1}'.format(self.auth,auth))

            raise InvalidAuthField()

    def keep_alive(self,auth):
        self.check_auth(auth)
        self.last_update = time.time()

    def update_pos(self, auth, x, y):
        self.check_auth(auth)
        self.x = x
        self.y = y
        self.last_update = time.time()



class Server:

    def __init__(self, address='0.0.0.0', port=9999, tolerance=20):

        self.address = address
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind((address, port))
        self.players = {}
        self.tolerance = tolerance


    def process_packet(self,packet,sender):

        type,auth= struct.unpack_from("sI",packet)
        type = codecs.decode(type,"UTF-8")
        
        print("questo è il tipo",str(type))
        
        if(type == P_POSITION.type):
            if(len(packet)!= P_POSITION.dimension):
                raise InvalidPacketSize()
            else:
                _,_,x,y = struct.unpack(P_POSITION.format,packet)
                self.players[sender].update_pos(auth,x,y)
                new_packet = struct.pack(P_POSITION.format,bytes(type,'utf-8_'),auth,x,y)
                #print("position for {0} is {1}/{2}".format(auth,x,y))
                print("info sent:"+str(new_packet))
                self.broadcast(sender,new_packet)
                return

        if(type == P_KEEP_ALIVE.type):
            print("sono qui")
            if(len(packet)!= P_KEEP_ALIVE.dimension):
                raise InvalidPacketSize()
            else:
                self.players[sender].keep_alive(auth)
                return

        if(type == P_HANDSHACKING.type):
            if(len(packet)!= P_HANDSHACKING.dimension):
                raise InvalidPacketSize()
            else:
                print("request for auth")
                auth_packet = struct.pack(P_HANDSHACKING.format,bytes(P_HANDSHACKING.type,"UTF-8"),self.players[sender].auth)
                sent = self.socket.sendto(auth_packet,self.players[sender].signature)
                print("signature "+str(self.players[sender].signature))
                print("byte sent:"+str(sent))
                print("info sent:"+str(auth_packet))
                return
        print("invalid packet type " + type)
                





    def run_once(self):
        try:
            packet, sender = self.socket.recvfrom(8192)
            print(str(sender))

            if sender in self.players:
                if self.players[sender] is None:  # banned?
                    return
                now = time.time()
                if now - self.players[sender].last_update < (1 / self.tolerance):
                    self.players[sender].warning+=1
                    if self.players[sender].warning > 10:
                        raise DosAttemptDetected()
                if self.players[sender].warning is not None:
                    if now - self.players[sender].warning > 4:
                        self.players[sender].warning = 0
            else:
                self.players[sender] = Player(sender)
            
            self.process_packet(packet,sender)
            self.check_dead_clients()
        except DosAttemptDetected:
            print('Dos detected from {0}, kicking it out'.format(sender))
            # del(self.players[sender])
            self.players[sender] = None  # banned!
        except InvalidAuthField:
            print('Invalid packet auth detected from {0} auth {1}'.format(sender,self.players[sender].auth))
        except InvalidPacketSize:
            print('Invalid packet size detected from {0}'.format(sender))
        except OSError:
            print('packet discarded')

    def broadcast(self, sender, packet):
        for signature in self.players:
            if self.players[signature] is not None:
                if signature != sender:  # avoid loop
                    self.socket.sendto(packet,signature)

    def check_dead_clients(self):
        dead_clients = []
        now = time.time()
        for signature in self.players:
            if self.players[signature] is not None:
                if self.players[signature].last_update + 10 < now:
                    dead_clients.append(signature)

        for dead_client in dead_clients:
            print('{0} is dead, removing from the list of players'.format(dead_client))
            del(self.players[dead_client])

    def run(self):
        print('running Bomberman server...')
        while True:
            self.run_once()

if __name__ == '__main__':
    server = Server()
    server.run()