#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include<string>
#include<vector>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include "../include/core/vec4.hpp"
#include "../include/core/mat4.hpp"
#include "../include/core/Triangle.hpp"
#include "../include/core/Camera.hpp"
#include "../include/core/Mesh.hpp"
#include "../include/core/Lighting.hpp"
#include "../include/core/World.hpp"
#include "../include/utils/utils.hpp"

const float PI = 3.1415926535f;

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
    std::string path = getCurrentDirectory();

    win = SDL_CreateWindow("Game Engine 3D", 100, 100, ScreenSizeX, ScreenSizeY, SDL_WINDOW_SHOWN); // | SDL_WINDOW_FULLSCREEN_DESKTOP);
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); //  | SDL_RENDERER_PRESENTVSYNC

    std::vector<std::string> meshPath;
    std::vector<vec4> meshTranslation;
    std::vector<vec4> meshRotation;

    /*meshPath.push_back(path + "/obj/mountains.obj");
    meshTranslation.push_back(vec4({0.0f, -20.0f, 0.0f, 1.0f}));
    meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));*/
    
    meshPath.push_back(path + "/obj/VideoShip.obj");
    meshTranslation.push_back(vec4({0.0f, 0.0f, 10.0f, 1.0f}));
    meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));
    
    meshPath.push_back(path + "/obj/VideoShip.obj");
    meshTranslation.push_back(vec4({0.0f, 2.0f, 10.0f, 0.2f}));
    meshRotation.push_back(vec4({0.0f, 0.0f, 0.0f, 0.0f}));

    /*meshPath.push_back(path + "/obj/Wall.obj");
    meshTranslation.push_back(vec4({0.0f, 0.0f, 20.0f, 0.02f}));
    meshRotation.push_back(vec4({PI / 2.0f, 0.0f, 0.0f, 0.0f}));*/

    World world(meshPath, meshTranslation, meshRotation, vec4(0.0f * (PI / 180.0f))); 
    
    float scaleFactor = 100.0f / 100.0f;
    unsigned int BUFFER_RESOLUTIONX = static_cast<unsigned int>(ScreenSizeX * scaleFactor);
    unsigned int BUFFER_RESOLUTIONY = static_cast<unsigned int>(ScreenSizeY * scaleFactor);

    float *depthBuffer = (float*)malloc(BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY * sizeof(float));

    // Create the shadow map texture
    SDL_Texture* shadowMapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, BUFFER_RESOLUTIONX, BUFFER_RESOLUTIONY);
    float *shadowMap = (float*)malloc(BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY * sizeof(float));

    if (!depthBuffer) {
        fprintf(stderr, "Failed to allocate depth buffer\n");
        exit(EXIT_FAILURE);
    }
    if (!shadowMap) {
        fprintf(stderr, "Failed to allocate shadow map buffer\n");
        exit(EXIT_FAILURE);
    }

    // Define the direction vectors
    vec4 UpDirection(0.0, 1.0, 0.0);  // Y-axis, up direction
    vec4 ForwardDirection(0.0, 0.0, 1.0); // Z-axis, forward direction

    Camera camera(vec4(0.0, -3.0, 0.0), 
                  ForwardDirection,
                  UpDirection, 
                  90.0f, 
                  90.0f,
                  0.1f, 
                  1000.0f,
                  (static_cast<float>(ScreenSizeX)/static_cast<float>(ScreenSizeY)));

    // Create a directional light
    Lighting Light(vec4(0.0f, 10.0f, 10.0f), 
                   vec4(0.0f, -1.0f, 0.0f),
                   vec4(0.0f, 0.0f, 1.0f),
                   vec4(1.0f, 1.0f, 1.0f), 
                   DIRECTIONAL, 
                   90.0f,
                   90.0f,
                   0.1f, 
                   1000.0f,
                   (static_cast<float>(ScreenSizeX)/static_cast<float>(ScreenSizeY)));

    float Velocity = 0.4;
    float cameraYaw = -90.0f;   // Horizontal rotation
    float cameraPitch = 0.0f; // Vertical rotation
    float sensitivity = 0.1f; // Mouse sensitivity                        
    
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;
    float fps = 0.0f;
    
    // Track key states
    std::unordered_map<SDL_Keycode, bool> keyStates = {
        {SDLK_UP, false},
        {SDLK_DOWN, false},
        {SDLK_LEFT, false},
        {SDLK_RIGHT, false},
        {SDLK_z, false},
        {SDLK_s, false}
    };

    // SDL_SetRelativeMouseMode(SDL_TRUE); // Capture mouse movement

    bool running = true;
    while (running)
    {
        int numVerts = 0;
        int numPixels = 0;
        int numShadow = 0;

        SDL_SetRenderDrawColor(renderer, 20, 40, 100, 255);
        SDL_RenderClear(renderer);
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                keyStates[event.key.keysym.sym] = true; // Set key state to true on key down
            } else if (event.type == SDL_KEYUP) {
                keyStates[event.key.keysym.sym] = false; // Set key state to false on key up
            }
        }
        
        // Use the keyStates map to control camera movement
        if (keyStates[SDLK_UP]) {
            camera.moveUp(Velocity);
        }
        if (keyStates[SDLK_DOWN]) {
            camera.moveUp(-Velocity);
        }
        if (keyStates[SDLK_LEFT]) {
            camera.moveRight(-Velocity);
        }
        if (keyStates[SDLK_RIGHT]) {
            camera.moveRight(Velocity);
        }
        if (keyStates[SDLK_z]) {
            camera.moveForward(Velocity);
        }
        if (keyStates[SDLK_s]) {
            camera.moveForward(-Velocity);
        }


        mat4 WorldRotationX, WorldRotationY, WorldRotationZ;

        // Create all the matrix for the world transformation
        WorldRotationX.RotationX(world.WorldRotation.x); // (rotation around X-axis)
        WorldRotationY.RotationY(world.WorldRotation.y); // (rotation around Y-axis)
        WorldRotationZ.RotationZ(world.WorldRotation.z); // (rotation around Z-axis)
        
        // All the transformation of the world (apply to all meshes)
        mat4 WorldTransformation;
        WorldTransformation = WorldRotationZ * WorldRotationX * WorldRotationY;

        // Initialize memory
        for (int i = 0; i < BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY; i++) {
            depthBuffer[i] =  std::numeric_limits<float>::infinity(); // Set each element to infinity
            shadowMap[i] =  std::numeric_limits<float>::infinity();
        }

        for (auto& mesh : world.meshes) {
            mat4 MeshRotationX, MeshRotationY, MeshRotationZ, MeshTranslation;

            // Mesh transformation (different for each mesh)
            MeshRotationX.RotationX(mesh.rotation.x); // Rotation around X-axis
            MeshRotationY.RotationY(mesh.rotation.y); // Rotation around Y-axis
            MeshRotationZ.RotationZ(mesh.rotation.z); // Rotation around Z-axis

            // Combine rotations (order matters: first Z, then X, then Y)
            mat4 MeshRotation;
            MeshRotation = MeshRotationZ * MeshRotationX * MeshRotationY; // Combining rotations

            // Mesh translation (moving the mesh to its world position)
            MeshTranslation.Translation(mesh.position);

            // Final transformation matrix
            mat4 MeshTransformation;

            // Apply rotation first, then translation
            MeshTransformation = MeshTranslation * MeshRotation;

            // =========================
            // Begin Triangles Processing
            // =========================

            for (auto& tri : mesh.tris)
            {
                triangle triProjected, triViewed, triTransformed, triMeshTransformed, triLightSpace;

                // Apply mesh transformation on each triangle
                for (int o = 0; o < 3; o++) {
                    triMeshTransformed.p[o] = MeshTransformation * tri.p[o];
                }
                
                // Apply world transformation on each triangle
                for (int o = 0; o < 3; o++) {  
                    triTransformed.p[o] = WorldTransformation * triMeshTransformed.p[o];
                }

                vec4 line1 = triTransformed.p[1] - triTransformed.p[0];
                vec4 line2 = triTransformed.p[2] - triTransformed.p[0];
                
                vec4 normal = line1.crossProduct(line2);
                normal.normalize();

                vec4 vCameraRay = triTransformed.p[0] - Light.position;
                
                if (normal.dot(vCameraRay) < 0.0f) {
                    // =================================================================
                    // Clip Triangles against the edge of the screen (frustrum clipping)
                    // =================================================================

                    int nClippedTriangles = 0;
                    triangle triLightClipped[2];

                    nClippedTriangles = Triangle_ClipAgainstPlane(vec4(0.0f, 0.0f, 0.1f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), triTransformed, triLightClipped[0], triLightClipped[1]);
                    
                    for (int n = 0; n < nClippedTriangles; n++)
                    {                                                                                                  
                        for (int o = 0; o < 3; o++) { 
                            // =========================================================================================================
                            // Convert World Space --> View Space (view matrix) and Project triangles from 3D --> 2D (projection matrix)
                            // =========================================================================================================
                            triLightSpace.p[o] = Light.computeSpaceMatrix() * triLightClipped[n].p[o];
                            triLightSpace.p[o] = triLightSpace.p[o] / triLightSpace.p[o].w;

                            // ======================
                            // NDC --> Screen Space
                            // ======================
                            triLightSpace.p[o].x = (triLightSpace.p[o].x + 1.0f) * 0.5f * ScreenSizeX;
                            triLightSpace.p[o].y = (triLightSpace.p[o].y + 1.0f) * 0.5f * ScreenSizeY;
                        }

                        // =================
                        // Render Shadow Map
                        // =================
                        ShadowMap(triLightSpace, shadowMap, ScreenSizeX, ScreenSizeY, BUFFER_RESOLUTIONX, BUFFER_RESOLUTIONY, numPixels, numShadow);
                    }         
                }
            }

            // =========================
            // Begin Triangles Processing
            // =========================

            for (auto& tri : mesh.tris)
            {
                triangle triProjected, triViewed, triTransformed, triMeshTransformed, triLightSpace;

                // Apply mesh transformation on each triangle
                for (int o = 0; o < 3; o++) {
                    triMeshTransformed.p[o] = MeshTransformation * tri.p[o];
                }
                
                // Apply world transformation on each triangle
                for (int o = 0; o < 3; o++) {  
                    triTransformed.p[o] = WorldTransformation * triMeshTransformed.p[o];
                }

                vec4 line1 = triTransformed.p[1] - triTransformed.p[0];
                vec4 line2 = triTransformed.p[2] - triTransformed.p[0];
                
                vec4 normal = line1.crossProduct(line2);
                normal.normalize();

                vec4 vCameraRay = triTransformed.p[0] - camera.position;
                
                if (normal.dot(vCameraRay) < 0.0f) {
                    // =================================================================
                    // Clip Triangles against the edge of the screen (frustrum clipping)
                    // =================================================================

                    int nClippedTriangles = 0;
                    triangle triClipped[2];
                    
                    nClippedTriangles = Triangle_ClipAgainstPlane(vec4(0.0f, 0.0f, 0.1f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), triTransformed, triClipped[0], triClipped[1]);
                    
                    for (int n = 0; n < nClippedTriangles; n++)
                    {                                                                                                  
                        for (int o = 0; o < 3; o++) { 
                            // =========================================================================================================
                            // Convert World Space --> View Space (view matrix) and Project triangles from 3D --> 2D (projection matrix)
                            // =========================================================================================================
                            triProjected.p[o] = camera.computeSpaceMatrix() * triClipped[n].p[o];
                            triProjected.p[o] = triProjected.p[o] / triProjected.p[o].w; 

                            // ======================
                            // NDC --> Screen Space
                            // ======================
                            triProjected.p[o].x = (triProjected.p[o].x + 1.0f) * 0.5f * ScreenSizeX;
                            triProjected.p[o].y = (triProjected.p[o].y + 1.0f) * 0.5f * ScreenSizeY;
                        }

                        

                        // Calculate the material attributes for the triangle
                        triProjected.ambient = vec4(0.1f, 0.1f, 0.1f);
                        triProjected.diffuse = vec4(0.7f, 0.7f, 0.7f);
                        triProjected.specular = vec4(0.2f, 0.2f, 0.2f);
                        triProjected.shininess = 32.f;
                        triProjected.normal = normal;

                        // =================================================
                        // Drawing the projected triangle with a depthBuffer
                        // =================================================
                        DrawTriangleDepthBufferPerformance(renderer, triProjected, triTransformed, depthBuffer, shadowMap, ScreenSizeX, ScreenSizeY, numPixels, camera, Light, BUFFER_RESOLUTIONX, BUFFER_RESOLUTIONY, numShadow);
                        numVerts += 1;
                    }         
                }
            }
        }

        // ===================================
        // Create a texture for the Shadow Map
        // ===================================
        
        // After rendering your scene and generating the shadow map
        Uint32* pixels = new Uint32[BUFFER_RESOLUTIONX * BUFFER_RESOLUTIONY];
        
        // Get the pixel format from the texture
        Uint32 pixelFormatEnum;
        SDL_QueryTexture(shadowMapTexture, &pixelFormatEnum, NULL, NULL, NULL);

        // Convert depth values to grayscale
        for (unsigned int y = 0; y < BUFFER_RESOLUTIONY; ++y) {
            for (unsigned int x = 0; x < BUFFER_RESOLUTIONX; ++x) {
                float depth = shadowMap[y * BUFFER_RESOLUTIONX + x];
                Uint8 gray = static_cast<Uint8>(255 * (depth - 1.0f)); // Invert for visualization
                pixels[y * BUFFER_RESOLUTIONX + x] = SDL_MapRGBA(SDL_AllocFormat(pixelFormatEnum), gray, gray, gray, 255);
            }
        }

        // ===================
        // Render Shadow Map
        // ===================

        // Update the texture with the pixel data
        SDL_UpdateTexture(shadowMapTexture, NULL, pixels, BUFFER_RESOLUTIONX * sizeof(Uint32));
        delete[] pixels; // Clean up

        // Render the shadow map texture
        SDL_Rect destRect = { 0, 0, BUFFER_RESOLUTIONX / 3, BUFFER_RESOLUTIONY / 3 }; // Full screen
        SDL_RenderCopy(renderer, shadowMapTexture, NULL, &destRect);

        // ===============
        // FPS calculation
        // ===============
        frameCount++;
        Uint32 frameEnd = SDL_GetTicks();
        Uint32 elapsedTime = frameEnd - startTime;
        if (elapsedTime > 1000) { // Update every second
            
            fps = frameCount / (elapsedTime / 1000.0f);
            frameCount = 0; startTime = frameEnd;

            // Info in the title of the window
            std::string title = "GameEngineInfo : {FPS: " + std::to_string(fps) + \
                                ", Number of Triangles: " + std::to_string(numVerts) + \
                                ", Number of Pixels : " + std::to_string(numPixels) +   \
                                ", Number of Pixels in shadow : " + std::to_string(numShadow) + "}";

            SDL_SetWindowTitle(win, title.c_str());
        }

        SDL_RenderPresent(renderer);
    }

    free(depthBuffer);
    free(shadowMap);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
};
