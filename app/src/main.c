#include "SDL.h"
#include <time.h>
#include "bomberman.h"
#include "level001.h"
#include "parse_bmp.h"
#include "usefull_scripts.h"
#include "client.h"

#define MAX_ATTEMPT_HANDSHAKING 5

int main(int argc, char **argv)
{
 
bomberman_t player0;
bomberman_t player1;

bomberman_t remote_player;
bomberman_t local_player;

    level_t level001;
    level_init(&level001, 8, 8, 64, level001_cells);
 
 
    SDL_Init(SDL_INIT_VIDEO);
 
    SDL_Window *window = SDL_CreateWindow("Bomberman",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          level001.cols * level001.cell_size,
                                          level001.rows * level001.cell_size,
                                          0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
 

    // SDL_Texture *texture;
    Uint8 *img_data;


    player0.movable.x = 100;
    player0.movable.y = 100;
    player0.movable.width = 32;
    player0.movable.height = 32;
    player0.movable.speed = 48;
    player0.drawable.texture=NULL;
    Usfllfuc_Open_File("Capture.bmp","rb",&img_data);
    Bomberman_Create_Texture(img_data,3,renderer,&player0.drawable.texture);
    SDL_free(img_data);
        
    player1.movable.x = 150;
    player1.movable.y = 100;
    player1.movable.width = 32;
    player1.movable.height = 32;
    player1.movable.speed = 48;
    player1.drawable.texture=NULL;
    
     Usfllfuc_Open_File("wall.bmp","rb",&img_data);
     Bomberman_Create_Texture(img_data,3,renderer,&player1.drawable.texture);
     SDL_free(img_data);
 
    SDL_Rect players_rect = {0, 0, player0.movable.width, player0.movable.height};
    SDL_Rect cell_rect = {0, 0, level001.cell_size, level001.cell_size};
 

 
    float delta_right = 0;
    float delta_left = 0;
    float delta_down = 0;
    float delta_up = 0;
 
    int running = 1;

     client_init();
     send_handshaking_req();

    clock_t start = clock();  //time
    clock_t start_send_timer = clock();  //time
    float send_data_timer = (float)1 / (float)15;
    int counter=0;
    
    while(!handshaking_verifier() && counter<MAX_ATTEMPT_HANDSHAKING){
        send_handshaking_req();
        counter++;
    }

    if(verify_handshacking()){

    local_player=player0;
    remote_player=player1;
    }else{


        local_player=player1;
        remote_player=player0;
    }

        
    local_player.drawable.rect = players_rect;
    remote_player.drawable.rect= players_rect;


    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    delta_right = player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    delta_left = -player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    delta_down = player0.movable.speed * (1.0 / 60);
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    delta_up = -player0.movable.speed * (1.0 / 60);
                }
            }
            if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    delta_right = 0;
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    delta_left = 0;
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    delta_down = 0;
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    delta_up = 0;
                }
            }
        }
 
 
        for (uint32_t row = 0; row < level001.rows; row++)
        {
            for (uint32_t col = 0; col < level001.cols; col++)
            {
                int32_t cell = level_cell(&level001, col, row);
                int32_t cell_texture = cell & 0xff;
                cell_rect.x = col * level001.cell_size;
                cell_rect.y = row * level001.cell_size;
 
                if (cell_texture == BLOCK_GROUND)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
                else if (cell_texture == BLOCK_WALL)
                {
                    SDL_SetRenderDrawColor(renderer, 100, 50, 0, 255);
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
                else if (cell_texture == BLOCK_DESTROYABLE)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 50, 50, 255);
                    SDL_RenderFillRect(renderer, &cell_rect);
                }
            }
        }
 
        move_on_level(&level001, &local_player.movable, delta_right + delta_left, delta_down + delta_up);

         local_player.drawable.rect.x= local_player.movable.x;
         local_player.drawable.rect.y = local_player.movable.y;
        




        send_alive_packet(&start);
        if(((clock()-start_send_timer)/CLOCKS_PER_SEC)>send_data_timer){
            
            send_position(local_player.movable.x,local_player.movable.y);
            start_send_timer=clock();
        }

        receive_data(&remote_player);


         SDL_RenderCopy(renderer,local_player.drawable.texture,NULL,&local_player.drawable.rect);
         SDL_RenderCopy(renderer,remote_player.drawable.texture,NULL,&remote_player.drawable.rect);
         SDL_RenderPresent(renderer);
    }
 
	SDL_Quit();
    return 0;
}