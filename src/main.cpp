#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include<string>
#include<vector>
#include<algorithm>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include "../include/core/vec4.hpp"
#include "../include/core/mat4.hpp"
#include "../include/core/Triangle.hpp"
#include "../include/core/Mesh.hpp"
#include "../include/core/Meshes.hpp"
#include "../include/utils/utils.hpp"

const float PI = 3.1415926535f;

vec4 center_of_gravity(triangle triangleViewed)
{
    vec4 first_center = (triangleViewed.p[0] + triangleViewed.p[1])/2;
    vec4 G = (first_center + triangleViewed.p[2])/2;
    return G;
}

int main()
{
    SDL_Window* win = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int ScreenSizeX = DM.w - 100;
    int ScreenSizeY = DM.h - 100;
    
    
    float Velocity = 0.4;
    float VelocityCam = 0.2;
    float FovRad = 1/tan(90.0f * 0.5f * (3.1415926535f / 180.0f));
    float Far = 1000.0f;
    float Near = 0.1f;
    float AspectRatio = static_cast<float>(ScreenSizeX) / static_cast<float>(ScreenSizeY);
    float Theta = 0.0f;
    float Yaw = 0.0f;
    float elapsedTime = 0.0f;
    
    vec4 vCamera(0.0, 0.0, 0.0);
    vec4 vLookSi(1.0, 0.0, 0.0);
    vec4 vLookFor(0.0, 0.0, 1.0); 
    vec4 vSide(0.0, 0.0, 0.0); 
    vec4 vFor(0.0, 0.0, 0.0);

    mat4 MatrixProjection;
    MatrixProjection.m[0][0] = AspectRatio * FovRad;
    MatrixProjection.m[1][1] = FovRad;
    MatrixProjection.m[2][2] = Far / (Far - Near);
    MatrixProjection.m[2][3] = (-Far * Near) / (Far - Near);
    MatrixProjection.m[3][2] = 1.0f;
    
    std::string path = getCurrentDirectory();
    
    Meshes meshes({ path + "/obj/mountains.obj", path + "/obj/VideoShip.obj",path + "/obj/Wall.obj" },  
                  { vec4({0.0f, -20.0f, 0.0f, 1.0f}), vec4({0.0f, 0.0f, 0.0f, 0.2f}), vec4({0.0f, 0.0f, 20.0f, 0.02f}) },
                  { vec4({0.0f, 0.0f, 0.0f, 0.0f}), vec4({0.0f, 0.0f, 0.0f, 0.0f}), vec4({PI / 2.0f, 2.0f * PI / 2.0f, 0.0f, 0.0f}) }) ;

    TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
    win = SDL_CreateWindow("Game Engine 3D", 100, 100, ScreenSizeX, ScreenSizeY, SDL_WINDOW_SHOWN); // | SDL_WINDOW_FULLSCREEN_DESKTOP
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); //  | SDL_RENDERER_PRESENTVSYNC

    Uint32 startTime = SDL_GetTicks();

    int frameCount = 0;
    float fps = 0.0f;

    bool running = true;
    while (running)
    {
        int numVert = 0;
        SDL_SetRenderDrawColor(renderer, 20, 40, 100, 255);
        SDL_RenderClear(renderer);
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
                break;
            }
            // User presses a key
            else if (event.type == SDL_KEYDOWN)
            {
                vec4 vSide = vLookSi * Velocity;
                vec4 vForward = vLookFor * Velocity;

                if (SDLK_ESCAPE == event.key.keysym.sym) { running = false; break; }
                if (SDLK_UP == event.key.keysym.sym) { vCamera.y += Velocity; break; }
                if (SDLK_DOWN == event.key.keysym.sym) { vCamera.y -= Velocity; break; }
                if (SDLK_LEFT == event.key.keysym.sym) { vCamera = vCamera + vSide; break; }
                if (SDLK_RIGHT == event.key.keysym.sym) { vCamera = vCamera - vSide; break; }
                if (SDLK_z == event.key.keysym.sym) { vCamera = vCamera + vForward; break; }
                if (SDLK_q == event.key.keysym.sym) { Yaw += VelocityCam; break; }
                if (SDLK_s == event.key.keysym.sym) { vCamera = vCamera - vForward; break; }
                if (SDLK_d == event.key.keysym.sym) { Yaw -= VelocityCam; break; }
            }
        }
        
        mat4 matRotX = MatrixRotationX(Theta);
        mat4 matRotY = MatrixRotationY(Theta);
        mat4 matRotZ = MatrixRotationZ(Theta);
        
        mat4 matWorld;
        matWorld.MatrixMakeIdentity();
        matWorld = matRotZ * matRotX;
        matWorld = matWorld * matRotY;

        vec4 vUp( 0.0, 1.0, 0.0, 1.0 );
        vec4 vTargetZ( 0.0, 0.0, 1.0, 1.0 );
        vec4 vTargetX( 1.0, 0.0, 0.0, 1.0);

        mat4 matCameraRot = MatrixRotationY(Yaw);
        vLookFor = matCameraRot * vTargetZ;
        vLookSi = matCameraRot * vTargetX;
        vTargetZ = vCamera + vLookFor;
        mat4 matView = MatrixPointAt(vCamera, vTargetZ, vUp);
        
        std::vector<triangle> vecTrianglesToSort;
        for (auto& mesh : meshes.meshes)
        {
            mat4 matMeshRotX = MatrixRotationX(mesh.rot.x);
            mat4 matMeshRotY = MatrixRotationY(mesh.rot.y);
            mat4 matMeshRotZ = MatrixRotationZ(mesh.rot.z);

            // =========================
            // Begin Triangles Processing
            // =========================

            for (auto& tri : mesh.tris)
            {
                triangle triProjected, triViewed, triTranslated, triTransformed, triRotatedX, triRotatedXY, triRotatedXYZ;

                for (int o = 0; o < 3; o++)
                    triRotatedX.p[o] = matMeshRotX * tri.p[o];
                for (int o = 0; o < 3; o++)
                     triRotatedXY.p[o] = matMeshRotY * triRotatedX.p[o];
                for (int o = 0; o < 3; o++)
                    triRotatedXYZ.p[o] = matMeshRotZ * triRotatedXY.p[o];

                triTransformed.p[0] = matWorld * triRotatedXYZ.p[0];
                triTransformed.p[1] = matWorld * triRotatedXYZ.p[1];
                triTransformed.p[2] = matWorld * triRotatedXYZ.p[2];

                for (int i = 0; i < 3; i++) {
                    triTranslated.p[i] = triTransformed.p[i] + mesh.coo;
                }

                vec4 line1 = triTranslated.p[1] - triTranslated.p[0];
                vec4 line2 = triTranslated.p[2] - triTranslated.p[0];
                
                vec4 normal = line1.crossProduct(line2);
                normal.normalize();

                vec4 vCameraRay = triTranslated.p[0] - vCamera;
                
                //if (normal.coo[2] < 0)
                if (normal.dot(vCameraRay) < 0.0f)
                {
                    // Convert World Space --> View Space
                    triViewed.p[0] = matView * triTranslated.p[0];
                    triViewed.p[1] = matView * triTranslated.p[1];
                    triViewed.p[2] = matView * triTranslated.p[2];

                    // =========================
                    // Illumination
                    // =========================
                    
                    vec4 col(255, 255, 255, 255);
                    for (int i = 0; i < 3; i++) {
                         triViewed.col[i] = col;
                    }
                    
                    // =========================
                    // Clip Triangles against the edge of the screen (frustrum clipping)
                    // =========================

                    int nClippedTriangles = 0;
                    triangle clipped[2];
                    
                    nClippedTriangles = Triangle_ClipAgainstPlane(vec4(0.0f, 0.0f, 0.1f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), triViewed, clipped[0], clipped[1]);

                    for (int n = 0; n < nClippedTriangles; n++)
                    {
                        // Project triangles from 3D --> 2D
                        triProjected.p[0] = MatrixProjection * clipped[n].p[0];
                        triProjected.p[1] = MatrixProjection * clipped[n].p[1];
                        triProjected.p[2] = MatrixProjection * clipped[n].p[2];

                        triProjected.p[0] = triProjected.p[0] / triProjected.p[0].w;
                        triProjected.p[1] = triProjected.p[1] / triProjected.p[1].w;
                        triProjected.p[2] = triProjected.p[2] / triProjected.p[2].w;

                        // X/Y are inverted so put them back
                        triProjected.p[0].x *= -1.0f;
                        triProjected.p[1].x *= -1.0f;
                        triProjected.p[2].x *= -1.0f;
                        triProjected.p[0].y *= -1.0f;
                        triProjected.p[1].y *= -1.0f;
                        triProjected.p[2].y *= -1.0f;

                        // Scale into view
                        vec4 vOffsetView(1.0, 1.0, 0.0, 1.0f);
                        triProjected.p[0] = triProjected.p[0] + vOffsetView;
                        triProjected.p[1] = triProjected.p[1] + vOffsetView;
                        triProjected.p[2] = triProjected.p[2] + vOffsetView;
                        triProjected.p[0].x *= 0.5f * ScreenSizeX;
                        triProjected.p[0].y *= 0.5f * ScreenSizeY;
                        triProjected.p[1].x *= 0.5f * ScreenSizeX;
                        triProjected.p[1].y *= 0.5f * ScreenSizeY;
                        triProjected.p[2].x *= 0.5f * ScreenSizeX;
                        triProjected.p[2].y *= 0.5f * ScreenSizeY;

                        for (int x = 0; x < 3; x++)
                        {
                            triProjected.col[x] = triViewed.col[x];
                        }

                        // Store triangle for sorting
                        vecTrianglesToSort.push_back(triProjected);
                    }
                }
            }
        }


        // Sort triangles from back to front
        std::sort(vecTrianglesToSort.begin(), vecTrianglesToSort.end(), [](triangle& t1, triangle& t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });

        // Clip triangles against all four screen edges
        for (auto& tri : vecTrianglesToSort) {
            std::vector<triangle> clippedTriangles = { tri };
            std::vector<triangle> trianglesToAdd;

            for (int edge = 0; edge < 4; ++edge) {
                trianglesToAdd.clear();
                for (auto& t : clippedTriangles) {
                    int nClippedTriangles;
                    triangle clipped[2]; // To store up to 2 clipped triangles

                    switch (edge) {
                    case 0: // Clip against left edge
                        nClippedTriangles = Triangle_ClipAgainstPlane(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4( 1.0f, 0.0f, 0.0f, 1.0f ), t, clipped[0], clipped[1]);
                        break;
                    case 1: // Clip against right edge
                        nClippedTriangles = Triangle_ClipAgainstPlane(vec4( (float)(ScreenSizeX - 1), 0.0f, 0.0f, 1.0f), vec4( -1.0f, 0.0f, 0.0f, 1.0f), t, clipped[0], clipped[1]);
                        break;
                    case 2: // Clip against top edge
                        nClippedTriangles = Triangle_ClipAgainstPlane(vec4( 0.0f, 0.0f, 0.0f, 1.0f), vec4( 0.0f, 1.0f, 0.0f, 1.0f), t, clipped[0], clipped[1]);
                        break;
                    case 3: // Clip against bottom edge
                        nClippedTriangles = Triangle_ClipAgainstPlane(vec4( 0.0f, (float)(ScreenSizeY - 1), 0.0f, 1.0f), vec4( 0.0f, -1.0f, 0.0f, 1.0f), t, clipped[0], clipped[1]);
                        break;
                    }

                    for (int i = 0; i < nClippedTriangles; ++i) {
                        trianglesToAdd.push_back(clipped[i]);
                    }
                }
                clippedTriangles = trianglesToAdd;
            }
        } 

        for (auto& triProjected : vecTrianglesToSort)
        {
            const std::vector<SDL_Vertex> verts =
            {
                { SDL_FPoint{ triProjected.p[0].x, triProjected.p[0].y }, \
                  SDL_Color{static_cast<Uint8>(triProjected.col[0].x),
                            static_cast<Uint8>(triProjected.col[0].x),
                            static_cast<Uint8>(triProjected.col[0].z),
                            static_cast<Uint8>(triProjected.col[0].w) }, SDL_FPoint{0} },
                { SDL_FPoint{ triProjected.p[1].x, triProjected.p[1].y }, 
                  SDL_Color{static_cast<Uint8>(triProjected.col[1].x),
                            static_cast<Uint8>(triProjected.col[1].x),
                            static_cast<Uint8>(triProjected.col[1].z),
                            static_cast<Uint8>(triProjected.col[1].w) }, SDL_FPoint{0} },
                { SDL_FPoint{ triProjected.p[2].x, triProjected.p[2].y }, 
                  SDL_Color{static_cast<Uint8>(triProjected.col[2].x),
                            static_cast<Uint8>(triProjected.col[2].x),
                            static_cast<Uint8>(triProjected.col[2].z),
                            static_cast<Uint8>(triProjected.col[2].w) }, SDL_FPoint{0} },
            };
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawLine(renderer, triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y);
            SDL_RenderDrawLine(renderer, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y);
            SDL_RenderDrawLine(renderer, triProjected.p[2].x, triProjected.p[2].y, triProjected.p[0].x, triProjected.p[0].y);
            SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);
            numVert += 1;
        }

        // FPS calculation
        frameCount++;
        Uint32 frameEnd = SDL_GetTicks();
        Uint32 elapsedTime = frameEnd - startTime;
        if (elapsedTime > 1000) { // Update every second
            fps = frameCount / (elapsedTime / 1000.0f);
            frameCount = 0;
            startTime = frameEnd;
            // Info in the title of the window
            std::string title = "GameEngine {Info : {FPS: " + std::to_string(fps) + ", Number of Triangles: " + std::to_string(numVert) + "}}";
            SDL_SetWindowTitle(win, title.c_str());
        }

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
};
