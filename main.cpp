#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <thread>

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
        Vec3C operator + (const Vec3C& vec) {
            Vec3C output;
            output.x = vec.x + x;
            output.y = vec.y + y;
            output.z = vec.z + z;
            return output;
        };
        Vec3C operator - (const Vec3C& vec) {
            Vec3C output;
            output.x = x - vec.x;
            output.y = y - vec.y;
            output.z = z - vec.z;
            return output;
        };
        Vec3C perspectiveDivide() {
            Vec3C output;
            f32 positiveZ = z < 0 ? z * -1 : z;
            positiveZ = positiveZ < 0.1 ? 0.1 : positiveZ;
            output.x = x / positiveZ;
            output.y = y / positiveZ;
            output.z = z;
            return output;
        };
        Vec3C crossProduct(const Vec3C& vec) {
            return Vec3C(z*vec.y-y*vec.z, x*vec.z-z*vec.x, y*vec.x-x*vec.y);
        };
        f32 magnitude() {
            return sqrt(x*x + y*y + z*z);
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
            r = vec.magnitude();
            theta = acos(vec.y/r);
            phi = acos(vec.z/sqrt(vec.x * vec.x + vec.z * vec.z));
            if (vec.x < 0) {
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
            output.x = r * sin(theta) * sin(phi);
            output.z = r * sin(theta) * cos(phi);
            return output;
        };
    
};
void printVector(Vec3C& vec) {
    std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
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
struct BarycentricCoordinates {
    f32 lambda0;
    f32 lambda1;
    f32 lambda2;
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
        Vec3C operator * (const Vec3C& vec) {
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

Vec3C normalizeVecToCameraCanvas(Vec3C& vec, Camera& camera) {
    Vec3C output;
    output.x = (vec.x + camera.viewFrameRadiusX) / (2 * camera.viewFrameRadiusX);
    output.y = (vec.y + camera.viewFrameRadiusY) / (2 * camera.viewFrameRadiusY);
    output.z = vec.z;
    return output;
};
class Triangle {
    public:
        Vec3C v0;
        Vec3C v1;
        Vec3C v2;

        Triangle(const Vec3C& _v0, const Vec3C& _v1, const Vec3C& _v2) {
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
            output.v0 = cameraMatrix * (v0 - cameraPosition);
            output.v1 = cameraMatrix * (v1 - cameraPosition);
            output.v2 = cameraMatrix * (v2 - cameraPosition);
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
        bool pixelIsBounded(Vec3C& pixelRelative) {
            if ((v1-v0).crossProduct(pixelRelative-v0).z > 0) {return false;};
            if ((v2-v1).crossProduct(pixelRelative-v1).z > 0) {return false;};
            if ((v0-v2).crossProduct(pixelRelative-v2).z > 0) {return false;};
            return true;
        };
        Triangle normalizeToCameraCanvas(Camera& camera) {
            Triangle output;
            output.v0 = normalizeVecToCameraCanvas(v0, camera);
            output.v1 = normalizeVecToCameraCanvas(v1, camera);
            output.v2 = normalizeVecToCameraCanvas(v2, camera);
            return output;
        };
        BarycentricCoordinates getBarycentricCoordinates(Vec3C& point) {
            BarycentricCoordinates output;
            f32 A0 = (v1-v2).crossProduct(point-v1).magnitude() / 2;
            f32 A1 = (v2-v0).crossProduct(point-v2).magnitude() / 2;
            f32 A2 = (v0-v1).crossProduct(point-v0).magnitude() / 2;
            output.lambda0 = A0 / (A0 + A1 + A2);
            output.lambda1 = A1 / (A0 + A1 + A2);
            output.lambda2 = A2 / (A0 + A1 + A2);
            return output;
        };
        Triangle rasterToCameraSpace(Camera& camera) {
            Triangle output;
            f32 zCamera = camera.position.z;
            output.v0.x = v0.x * zCamera;
            output.v0.y = v0.y * zCamera;
            output.v0.z = v0.z;
            output.v1.x = v1.x * zCamera;
            output.v1.y = v1.y * zCamera;
            output.v1.z = v1.z;
            output.v2.x = v2.x * zCamera;
            output.v2.y = v2.y * zCamera;
            output.v2.z = v2.z;
            return output;
        };
};

struct VertexValues {
    f32 val0;
    f32 val1;
    f32 val2;
};

f32 interpolate3D(BarycentricCoordinates barycentricCoordinates, VertexValues vertexValues) {
    return barycentricCoordinates.lambda0 * vertexValues.val0 + barycentricCoordinates.lambda1 * vertexValues.val1 + barycentricCoordinates.lambda2 * vertexValues.val2;
};

class SDL {
    public:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        Camera _camera;

        int pixelCountX;
        int pixelCountY;
        int pixelTotal;
        u32* pixels;
        f32* pixelZBuffer;

        const int zMax = 10;

        SDL(int _pixelCountX, int _pixelCountY, int windowPixelsX, int windowPixelsY, Camera& cam) {
            _camera = cam;
            pixelCountX = _pixelCountX;
            pixelCountY = _pixelCountY;
            pixelTotal = pixelCountX * pixelCountY;
            pixels = new u32[pixelTotal];
            pixelZBuffer = new f32[pixelTotal];
            for (int i=0; i<pixelTotal; i++) {
                pixels[i] = u32(0x000000FF);
                pixelZBuffer[i] = f32(zMax);
            };

            SDL_Init(SDL_INIT_VIDEO);

            window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowPixelsX, windowPixelsY, 0);
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, pixelCountX, pixelCountY);
            SDL_UpdateTexture(texture, NULL, pixels, pixelCountX * 4);
            SDL_RenderCopy(renderer, texture, NULL, NULL);

            SDL_RenderPresent(renderer);
        };

        void reRenderNormalizedRasterizedTriangles(std::vector<Triangle>& triangles) {
            for (Triangle& triangle : triangles) {
                renderNormalizedRasterizedTriangle(triangle);
            };
            for (int i=0; i<pixelTotal; i++) {
                pixelZBuffer[i] = f32(zMax);
            };
        };

        void renderRow(int minX, int maxX, int minY, int maxY, Triangle triangle) {
            for (int pixelY=minY; pixelY<maxY; pixelY++) {
                for (int pixelX=minX; pixelX<maxX; pixelX++) {
                    f32 relativeX = f32(pixelX) / f32(pixelCountX);
                    f32 relativeY = f32(pixelY) / f32(pixelCountY);
                    Vec3C relativePixel = Vec3C(relativeX, relativeY, 1);

                    f32 zPixel = interpolate3D(triangle.getBarycentricCoordinates(relativePixel), {triangle.v0.z, triangle.v1.z, triangle.v2.z});

                    int pixelIndex = pixelY * pixelCountX + pixelX;

                    if (zPixel > pixelZBuffer[pixelIndex]) { continue; };

                    if (zPixel > 0.2 && triangle.pixelIsBounded(relativePixel)) {
                        u32 inputVal = 0x0000FFFF;
                        pixels[pixelIndex] = inputVal;
                        pixelZBuffer[pixelIndex] = zPixel;
                    };
                };
            };
        };

        
        void renderNormalizedRasterizedTriangle(Triangle& triangle) {
            box2D boundingRect = triangle.getBoundingRectXY();
            int minX = boundingRect.x0 * pixelCountX >= 0 ? boundingRect.x0 * pixelCountX : 0;
            int minY = boundingRect.y0 * pixelCountY >= 0 ? boundingRect.y0 * pixelCountY : 0;
            int maxX = boundingRect.x1 * pixelCountX <= pixelCountX ? boundingRect.x1 * pixelCountX : pixelCountX;
            int maxY = boundingRect.y1 * pixelCountY <= pixelCountY ? boundingRect.y1 * pixelCountY : pixelCountY;

            std::thread t1 = std::thread(&SDL::renderRow, this, minX, maxX, minY, maxY/8, triangle);
            std::thread t2 = std::thread(&SDL::renderRow, this, minX, maxX, maxY/8, maxY/4, triangle);
            std::thread t3 = std::thread(&SDL::renderRow, this, minX, maxX, maxY/4, maxY*3/8, triangle);
            std::thread t4 = std::thread(&SDL::renderRow, this, minX, maxX, maxY*3/8, maxY/2, triangle);
            std::thread t5 = std::thread(&SDL::renderRow, this, minX, maxX, maxY/2, maxY*5/8, triangle);
            std::thread t6 = std::thread(&SDL::renderRow, this, minX, maxX, maxY*5/8, maxY*3/4, triangle);
            std::thread t7 = std::thread(&SDL::renderRow, this, minX, maxX, maxY*3/4, maxY*7/8, triangle);
            std::thread t8 = std::thread(&SDL::renderRow, this, minX, maxX, maxY*7/8, maxY, triangle);

            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
            t6.join();
            t7.join();
            t8.join();
            
        };

        void render() {
            SDL_RenderClear(renderer);
            SDL_UpdateTexture(texture, NULL, pixels, pixelCountX * 4);
            SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
            SDL_RenderPresent(renderer);
        };

        ~SDL() {
            delete[] pixelZBuffer;
            delete[] pixels;
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        };
};


void printTriangle(Triangle& triangle) {
    std::cout << "v0:" << " ";
    printVector(triangle.v0);
    std::cout << "v1:" << " ";
    printVector(triangle.v1);
    std::cout << "v2:" << " ";
    printVector(triangle.v2);
};

void printCamera(Camera& camera) {
    std::cout << "position:";
    printVector(camera.position);
    std::cout << "orientation:";
    std::cout << "theta: " << camera.orientation.theta/PI << " " << "phi: " << camera.orientation.phi/PI << std::endl;
};


int main(int argc, char *argv[]) {

    Camera camera = Camera();

    SDL SDLState = SDL(800, 600, 1600, 1200, camera);

    std::vector<Triangle> trianglesWorldSpace;
    std::vector<Triangle> trianglesCameraSpace;
    std::vector<Triangle> trianglesRasterSpace;

    Triangle input = Triangle(Vec3C(0,1,4),Vec3C(0,0,4),Vec3C(1,0,4));
    trianglesWorldSpace.push_back(input);


    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_w) {
                    camera.position = camera.position + Vec3C(0,0,0.1);
                }
                if (e.key.keysym.sym == SDLK_s) {
                    camera.position = camera.position - Vec3C(0,0,0.1);
                }
                if (e.key.keysym.sym == SDLK_a) {
                    camera.position = camera.position - Vec3C(0.1,0,0);
                }
                if (e.key.keysym.sym == SDLK_d) {
                    camera.position = camera.position + Vec3C(0.1,0,0);
                }
                if (e.key.keysym.sym == SDLK_LEFT) {
                    camera.orientation.phi += 0.05;
                }
                if (e.key.keysym.sym == SDLK_RIGHT) {
                    camera.orientation.phi -= 0.05;
                }
                if (e.key.keysym.sym == SDLK_UP) {
                    camera.orientation.theta += 0.05;
                }
                if (e.key.keysym.sym == SDLK_DOWN) {
                    camera.orientation.theta -= 0.05;
                }
            }

            for (int i=0; i<SDLState.pixelTotal; i++) {
                SDLState.pixels[i] = 0x000000FF;
            };

            trianglesCameraSpace.clear();
            trianglesRasterSpace.clear();

            ColMatrix3 cameraMatrix = camera.createCameraMatrix();

            for (Triangle triangle : trianglesWorldSpace) {
                Triangle triangleCameraSpace = triangle.toCameraSpace(cameraMatrix, camera.position);
                trianglesCameraSpace.push_back(triangleCameraSpace);
                std::cout << "camera: ";
                printTriangle(triangleCameraSpace);
                printCamera(camera);
            };

            for (Triangle triangle : trianglesCameraSpace) {
                Triangle rasterTriangle = triangle.toRasterSpace();
                std::cout << "raster: ";
                printTriangle(rasterTriangle);
                if (rasterTriangle.rasterTriangleIsVisible(camera)) {
                    Triangle normalizedRasterizedTriangle = rasterTriangle.normalizeToCameraCanvas(camera);
                    trianglesRasterSpace.push_back(normalizedRasterizedTriangle);
                };
            };

            SDLState.reRenderNormalizedRasterizedTriangles(trianglesRasterSpace);

            SDLState.render();
        }
    }

    return 0;
}