#include <SDL.h>

//** PER UTILIZZARLO IN OUT
int Usfllfuc_Open_File(const char *fileName,const char *mode_to_open, Uint8 **content){
     
    SDL_RWops *rw = SDL_RWFromFile(fileName,mode_to_open);
    if(!rw) 
        return -1;

    Sint64 file_size = SDL_RWsize(rw);
    *content = SDL_malloc(file_size);

    if(!(*content)){
        SDL_RWclose(rw);
        return -1;
    }

    SDL_RWread(rw,*content, file_size, 1);
    SDL_RWclose(rw);
    return 0;
}