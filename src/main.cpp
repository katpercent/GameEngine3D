#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include<string>
#include<vector>
#include<algorithm>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include "../include/module/vec4.hpp"
#include "../include/module/mat4.hpp"
#include "../include/module/Triangle.hpp"
#include "../include/module/Mesh.hpp"
#include "../include/module/Meshes.hpp"
#include "../include/module/utils.hpp"

vec4 soleil(vec4& vec, double& time)
{
    float radians = timeToRadians(time);
    mat4 matRot = MatrixRotationZ(radians);
    return matRot * vec;
}


int main(int argc, char* argv[])
{
    SDL_Window* win = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* img = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int ScreenSizeX = DM.w - 100;
    int ScreenSizeY = DM.h - 100;
    
    vec4 lightPos = { 0.0f, 0.0f, 0.0f, 1.0f};
    int StartHour = 6;
    int TickRate = 10;
    float Velocity = 0.4;
    float VelocityCam = 0.2;
    float FovRad = 1.0f / tanf(90.0f * 0.5f * 3.1415926535f / 180.0f);
    float Far = 1000.0f;
    float Near = 0.1f;
    float AspectRatio = static_cast<float>(ScreenSizeX) / static_cast<float>(ScreenSizeY);
    float Theta = 0.0f;
    float Yaw = 0.0f;
    float elapsedTime = 0.0f;
    
    vec4 vCamera(0.0, 0.0, 0.0, 1.0);
    vec4 vLookSi(1.0, 0.0, 0.0, 1.0);
    vec4 vLookFor(0.0, 0.0, 1.0, 1.0); 
    vec4 vSide(0.0, 0.0, 0.0, 1.0); 
    vec4 vFor(0.0, 0.0, 0.0, 1.0);

    mat4 MatrixProjection;
    MatrixProjection.m[0][0] = AspectRatio * FovRad;
    MatrixProjection.m[1][1] = FovRad;
    MatrixProjection.m[2][2] = Far / (Far - Near);
    MatrixProjection.m[2][3] = (-Far * Near) / (Far - Near);
    MatrixProjection.m[3][2] = 1.0f;
    
    std::string path = getCurrentDirectory();
    Meshes meshes({ path + "/obj/mountains.obj", path + "/obj/Wall.ob" },  { vec4({0.0f, -20.0f, 0.0f, 1.0f}), vec4({0.0f, 0.0f, 20.0f, 0.02f}) }, { vec4({0.0f, 0.0f, 0.0f, 0.0f}), vec4({3.1415926535f / 2.0f, 2.0f * 3.1415926535f / 2.0f, 0.0f, 0.0f}) }) ;
    
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
        //matWorld = MatrixMultiplyMatrix(matWorld, matRotY);

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
                    triTranslated.p[i].x = triTransformed.p[i].x + mesh.coo.x;
                    triTranslated.p[i].y = triTransformed.p[i].y + mesh.coo.y;
                    triTranslated.p[i].z = triTransformed.p[i].z + mesh.coo.z;
                }

                vec4 line1 = triTranslated.p[1] - triTranslated.p[0];
                vec4 line2 = triTranslated.p[2] - triTranslated.p[0];

                // It's normally normal to normalise the normal
                vec4 normal = line1.crossProduct(line2);
                normal.normalize();

                vec4 vCameraRay = triTranslated.p[0] - vCamera;

                //if (normal.coo[2] < 0)
                if (normal.dot(vCameraRay) < 0.0f)
                {
                    /*for (int i = 0; i < 3; i++)
                    {
                        
                        vec4 lightDir = lightPos - triProjected.p[i]; // Assuming lightPos is the position of the light source
                        lightDir.normalize();
                        vec4 viewDir = vCamera - triProjected.p[i]; // Assuming cameraPos is the position of the camera
                        viewDir.normalize();

                        // Calculate Phong lighting
                        //vec4& normal, vec4& lightDir, vec4& viewDir, float ambientStrength, float diffuseStrength, float specularStrength, float shininess
                        vec4 lightColor(1.0f, 0.0f, 0.0f, 0.0f);
                        //triTranslated.col[i] = Gouraud(normal, lightDir, lightColor);
                        //triTranslated.col[i] = orenNayar(normal, lightDir, viewDir, 0.0f); // Adjust the parameters as needed
                        //triTranslated.col[i] = PhongLighting(lightColor, normal, lightDir, viewDir, 0.7f, 0.6f, 0.5f, 256.0f); // Adjust the parameters as needed
                    }*/

                    // Convert World Space-- > View Space
                    triViewed.p[0] = matView * triTranslated.p[0];
                    triViewed.p[1] = matView * triTranslated.p[1];
                    triViewed.p[2] = matView * triTranslated.p[2];

                    int nClippedTriangles = 0;
                    triangle clipped[2];
                    nClippedTriangles = Triangle_ClipAgainstPlane(vec4(0.0f, 0.0f, 0.1f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), triViewed, clipped[0], clipped[1]);

                    // We may end up with multiple triangles form the clip, so project as
                    // required
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
                            //triProjected.col[x] = triTranslated.col[x];
                            triProjected.col[x] = SDL_Color({ 255, 255, 255 });
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
        int triDraw = 0;
        // Clip triangles against all four screen edges
        for (auto& tri : vecTrianglesToSort) {
            std::vector<triangle> clippedTriangles = { tri };
            std::vector<triangle> trianglesToAdd;

            for (int plane = 0; plane < 4; ++plane) {
                trianglesToAdd.clear();
                for (auto& t : clippedTriangles) {
                    int nClippedTriangles;
                    triangle clipped[2]; // To store up to 2 clipped triangles

                    switch (plane) {
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
                { SDL_FPoint{ triProjected.p[0].x, triProjected.p[0].y }, triProjected.col[0], SDL_FPoint{0} },
                { SDL_FPoint{ triProjected.p[1].x, triProjected.p[1].y }, triProjected.col[1], SDL_FPoint{0} },
                { SDL_FPoint{ triProjected.p[2].x, triProjected.p[2].y }, triProjected.col[2], SDL_FPoint{0} },
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
