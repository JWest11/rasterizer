#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <cmath>

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

class Vec3 {
    public:
        f32 x;
        f32 y;
        f32 z;
        Vec3() {
            x = 0;
            y = 0;
            z = 0;
        };
        Vec3(f32 _x, f32 _y, f32 _z) {
            x = _x;
            y = _y;
            z = _z;
        };
        void Normalize() {
            f32 magnitude = sqrtf(x*x + y*y + z*z);
            x = x/magnitude;
            y = y/magnitude;
            z = z/magnitude;
        };
};
class Triangle {
    public:
        Vec3 v1;
        Vec3 v2;
        Vec3 v3;
        
        Triangle(Vec3& _v1, Vec3& _v2, Vec3& _v3) {
            v1 = _v1;
            v2 = _v2;
            v3 = _v3;
        };
};

class Matrix {
    public:
        u16 rows;
        u16 cols;
        u16 length;
        f32* data;

        Matrix(u16 _rows, u16 _cols) {
            rows = _rows;
            cols = _cols;
            length = _rows * _cols;
            data = new f32[length];
        };

        ~Matrix() {
            delete[] data;
        }
};

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

    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = true;
            }
        }
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    free(pixels);
    return 0;
}