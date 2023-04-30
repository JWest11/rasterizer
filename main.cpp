#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;

const int screenWidth = 1024;
const int screenHeight = 768;

int main(int argc, char *argv[]) {

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    u32 pixelCount = screenWidth * screenHeight;
    u32* pixels = (u32*) malloc(pixelCount * sizeof(u32));

    for (int i = 0; i < pixelCount; i++) {
        pixels[i] = 0;
    };

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, NULL);
    renderer = SDL_CreateRenderer(window, -1, NULL);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
    SDL_UpdateTexture(texture, NULL, pixels, screenWidth * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);

    SDL_Delay(5000);
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    free(pixels);
    return 0;
}