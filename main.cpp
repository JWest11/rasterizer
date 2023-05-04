#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <cmath>
#include <vector>

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

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};
struct Triangle {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
};
Vec3 scalarMultiply(Vec3& vec, f32 f) {
    Vec3 output;
    output.x = vec.x * f;
    output.y = vec.y * f;
    output.z = vec.z * f;
    return output;
};
Vec3 normalize(Vec3& vec) {
    f32 magnitude = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return scalarMultiply(vec, magnitude);
};
Triangle projectTriangle(Triangle& triangle) {
    Triangle output;
    output.v0 = scalarMultiply(triangle.v0, f32(1)/triangle.v0.z);
    output.v1 = scalarMultiply(triangle.v1, f32(1)/triangle.v1.z);
    output.v2 = scalarMultiply(triangle.v2, f32(1)/triangle.v2.z);
    return output;
};
Triangle toCameraSpace(Triangle triangle, Vec3 cameraLocation) {
    Triangle output;
    f32 transformationMatrix[3][3];
    return triangle;

};

Vec3 matrixMultiply(Vec3 vec, f32 matrix[3][3]) {
    Vec3 output;
    output.x = vec.x * matrix[0][0] + vec.y * matrix[1][0] + vec.z * matrix[2][0];
    output.y = vec.x * matrix[0][1] + vec.y * matrix[1][1] + vec.z * matrix[2][1];
    output.z = vec.x * matrix[0][2] + vec.y * matrix[1][2] + vec.z * matrix[2][2];
};

int main(int argc, char *argv[]) {

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    u32 pixelCount = screenWidth * screenHeight;
    u32* pixels = new u32[screenWidth*screenHeight];

    for (int i = 0; i < pixelCount; i++) {
        pixels[i] = 0;
    };

    std::vector<Triangle> triangles;
    std::vector<Triangle> projections;

    triangles.push_back({{1,1,1}, {1,2,1}, {1,1,2}});

    // triangles to canvas space
    for (Triangle triangle : triangles) {
        projections.push_back(projectTriangle(triangle));
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

    delete[] pixels;
    return 0;
}