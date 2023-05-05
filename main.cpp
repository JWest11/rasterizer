#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <cmath>
#include <vector>
#include <iostream>

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

#define PI 3.14159265

class Vec3C {
    public:
        f32 x;
        f32 y;
        f32 z;

        Vec3C() {
        };
        Vec3C(f32 _x, f32 _y, f32 _z) {
            x = _x;
            y = _y;
            z = _z;
        };
        Vec3C normalize() {
            f32 magnitude = sqrt(x * x + y * y + z * z);
            return *this / magnitude;
        };
        Vec3C operator * (f32 scalar) {
            Vec3C output;
            output.x = x * scalar;
            output.y = y * scalar;
            output.z = z * scalar;
            return output;
        };
        Vec3C operator / (f32 scalar) {
            Vec3C output;
            output.x = x / scalar;
            output.y = y / scalar;
            output.z = z / scalar;
            return output;
        };
        Vec3C operator + (Vec3C& vec) {
            Vec3C output;
            output.x = vec.x + x;
            output.y = vec.y + y;
            output.z = vec.z + z;
            return output;
        };
        Vec3C operator - (Vec3C& vec) {
            Vec3C output;
            output.x = vec.x - x;
            output.y = vec.y - y;
            output.z = vec.z - z;
            return output;
        };
        Vec3C perspectiveDivide() {
            Vec3C output;
            output.x = x / z;
            output.y = y / z;
            output.z = z;
            return output;
        };
};

class Vec3P {
    public:
        f32 r;
        f32 theta;
        f32 phi;

        Vec3P() {
        };
        Vec3P(Vec3C& vec) {
            r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
            theta = acos(vec.y/r);
            phi = acos(vec.x/sqrt(vec.x * vec.x + vec.z * vec.z));
            if (vec.z < 0) {
                phi = PI * 2 - phi;
            };
        };
        Vec3P(f32 _r, f32 _theta, f32 _phi) {
            r = _r;
            theta = _theta;
            phi = _phi;
        };
        Vec3C toVec3C() {
            Vec3C output;
            output.y = r * cos(theta);
            output.z = r * sin(theta) * sin(phi);
            output.x = r * sin(theta) * cos(phi);
            return output;
        };
    
};

f32 min3(f32 a, f32 b, f32 c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
};
f32 max3(f32 a, f32 b, f32 c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
};
struct box2D {
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
};

class ColMatrix3 {
    public:
        f32 data[9];

        ColMatrix3() {
            for (int i=0; i<9; i++) {
                data[i] = 0;
            };
        };
        ColMatrix3(f32 _data[9]) {
            for (int i=0; i<9; i++) {
                data[i] = _data[i];
            };
        };
        Vec3C operator * (Vec3C& vec) {
            Vec3C output;
            output.x = vec.x * data[0] + vec.y * data[1] + vec.z * data[2];
            output.y = vec.x * data[3] + vec.y * data[4] + vec.z * data[5];
            output.z = vec.x * data[6] + vec.y * data[7] + vec.z * data[8];
            return output;
        };
};
class Camera {
    public:
        Vec3C position;
        Vec3P orientation;
        f32 viewFrameRadiusY;
        f32 viewFrameRadiusX;

        Camera() {
            position = Vec3C(0,0,0);
            orientation = Vec3P(1,PI/2,0);
            viewFrameRadiusX = 1;
            viewFrameRadiusY = 1;
        };
        ColMatrix3 createCameraMatrix() {
            Vec3P ZP = orientation;
            Vec3P XP = Vec3P(1, PI / 2, orientation.phi + PI / 2);
            Vec3P YP = Vec3P(1, orientation.theta - PI / 2, orientation.phi + PI);
            Vec3C Z = ZP.toVec3C();
            Vec3C X = XP.toVec3C();
            Vec3C Y = YP.toVec3C();

            f32 matrixValues[9] = {X.x, Y.x, Z.x, X.y, Y.y, Z.y, X.z, Y.z, Z.z};
            ColMatrix3 output = ColMatrix3(matrixValues);
            return output;
        };
};
class Triangle {
    public:
        Vec3C v0;
        Vec3C v1;
        Vec3C v2;

        Triangle(Vec3C& _v0, Vec3C& _v1, Vec3C& _v2) {
            v0 = _v0;
            v1 = _v1;
            v2 = _v2;
        };
        Triangle() {
        };
        Triangle normalize() {
            Triangle output;
            output.v0 = v0.normalize();
            output.v1 = v1.normalize();
            output.v2 = v2.normalize();
            return output;
        };
        box2D getBoundingRectXY() {
            box2D output;
            output.x0 = min3(v0.x, v1.x, v2.x);
            output.x1 = max3(v0.x, v1.x, v2.x);
            output.y0 = min3(v0.y, v1.y, v2.y);
            output.y1 = max3(v0.y, v1.y, v2.y);
            return output;
        };
        Triangle toCameraSpace(ColMatrix3& cameraMatrix, Vec3C& cameraPosition) {
            Triangle output;
            output.v0 = cameraMatrix * (v0 - cameraPosition) ;
            output.v1 = cameraMatrix * (v1 - cameraPosition) ;
            output.v2 = cameraMatrix * (v2 - cameraPosition) ;
            return output;
            
        };
        Triangle toRasterSpace() {
            Triangle output;
            output.v0 = v0.perspectiveDivide();
            output.v1 = v1.perspectiveDivide();
            output.v2 = v2.perspectiveDivide();
            return output;
        };
        bool rasterTriangleIsVisible(Camera& camera) {
            box2D rect = this->getBoundingRectXY();
            if (rect.x0 > camera.viewFrameRadiusX || rect.y0 > camera.viewFrameRadiusY || rect.x1 < camera.viewFrameRadiusX * -1 || rect.y1 < camera.viewFrameRadiusY * -1) {return false;};
            return true;
        };
};

class SDL {
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        int pixelCountX;
        int pixelCountY;
        int pixelTotal;
        u32* pixels;

        SDL(int _pixelCountX, int _pixelCountY) {
            pixelCountX = _pixelCountX;
            pixelCountY = _pixelCountY;
            pixelTotal = pixelCountX * pixelCountY;
            pixels = new u32[pixelTotal];
            for (int i=0; i<pixelTotal; i++) {
                pixels[i] = u32(0x000000FF);
            };

            SDL_Init(SDL_INIT_VIDEO);

            window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixelCountX, pixelCountY, NULL);
            renderer = SDL_CreateRenderer(window, -1, NULL);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, pixelCountX, pixelCountY);
            SDL_UpdateTexture(texture, NULL, pixels, pixelCountX * 4);
            SDL_RenderCopy(renderer, texture, NULL, NULL);

            SDL_RenderPresent(renderer);
        };

        void renderRasterizedTriangle(Triangle& triangle) {
            for (int i=0; i<pixelTotal; i++) {
                int pointX = (i % pixelCountX) / pixelCountX;
                int pointY = (i / pixelCountX) / pixelCountY;
                bool bounded = false;
                if (triangle.v0)
            }
            
        };

        void render() {
            SDL_UpdateTexture(texture, NULL, pixels, pixelCountX * 4);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        };

        ~SDL() {
            delete[] pixels;
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        };
};

int main(int argc, char *argv[]) {

    SDL SDLState = SDL(1024, 768);

    Camera camera = Camera();

    std::vector<Triangle> trianglesWorldSpace;
    std::vector<Triangle> trianglesCameraSpace;
    std::vector<Triangle> trianglesRasterSpace;
    Triangle input = Triangle(Vec3C(0,0,1),Vec3C(1,1,1),Vec3C(0,1,1));
    trianglesWorldSpace.push_back(input);

    ColMatrix3 cameraMatrix = camera.createCameraMatrix();

    for (Triangle triangle : trianglesWorldSpace) {
        trianglesCameraSpace.push_back(triangle.toCameraSpace(cameraMatrix, camera.position));
    };

    for (Triangle triangle : trianglesCameraSpace) {
        Triangle rasterTriangle = triangle.toRasterSpace();
        if (rasterTriangle.rasterTriangleIsVisible(camera)) {
            trianglesRasterSpace.push_back(rasterTriangle);
        };
    };

    for (Triangle triangle : trianglesRasterSpace) {
        SDLState.renderRasterizedTriangle();
    };

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

    return 0;
}