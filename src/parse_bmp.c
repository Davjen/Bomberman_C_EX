#include <SDL.h>

#define _FILE_SIZE_OFFSET 2
#define _PIXEL_OFFSET 10
#define _IMAGE_SIZE_OFFSET 18
#define _PIXEL_DATA_SIZE_OFFSET 34


//** texture to use as out parameter
int Bomberman_Create_Texture(Uint8 *data,Uint8 channels, SDL_Renderer *renderer,SDL_Texture **texture){

    const Uint32 alignment = 4;

    Uint8 *pixel_starting_point = data + (*(data+_PIXEL_OFFSET));

    Uint32 image_size[2];
    SDL_memcpy(image_size, data + _IMAGE_SIZE_OFFSET, 8);

    *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, image_size[0], image_size[1]);

    if(!(*texture))
        return -1;

    Uint32 row_size = image_size[0] * channels;

    Uint32 padded_row_size = (row_size / alignment) * alignment;
    if (row_size % alignment != 0)
    {
        padded_row_size += alignment;
    }


    Uint8 *pixels = SDL_malloc(row_size * image_size[1]);
    if(!pixels){
        SDL_free(pixels);
        return -1;
    }


    for (Uint32 y = 0; y < image_size[1]; y++)
    {
        
        Uint8 *texture_row= pixels + (y*row_size);
        Uint8 *bitmap_row = pixel_starting_point +((image_size[1]-1)-y)*padded_row_size;

        SDL_memcpy(texture_row, bitmap_row, row_size);
    }
    SDL_UpdateTexture(*texture, NULL, pixels, row_size);
    SDL_free(pixels);
    return 0;
}
