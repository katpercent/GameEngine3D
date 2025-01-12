#include <string>
#include <vector>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include "../../include/core/vec4.hpp"
#include "../../include/core/Triangle.hpp"
#include "../../include/utils/utils.hpp"
#include "../../include/core/Camera.hpp"

#ifdef _WIN30
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

std::string getCurrentDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    } else {
        perror("getcwd() error");
        return "";
    }
}

double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

vec4 normalise(vec4 vec) {
    if (vec.magnitude() == 0) {
        //throw std::invalid_argument("Cannot normalize a zero-length vector!");
        return vec;
        
    }
    return vec / vec.magnitude();
}

vec4 Vector_IntersectPlane(vec4& plane_p, vec4& plane_n, vec4& lineStart, vec4& lineEnd, float& t)
{
    plane_n.normalize();
    float plane_d = -(plane_n.dot(plane_p));
    float ad = lineStart.dot(plane_n);
    float bd = lineEnd.dot(plane_n);
    t = (-plane_d - ad) / (bd - ad);
    vec4 lineStartToEnd = lineEnd - lineStart;
    vec4 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Triangle_ClipAgainstPlane(vec4 plane_p, vec4 plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n.normalize();

    // Return signed shortest distance from point to plane
    auto dist = [&](vec4& p)
        {
            return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
        };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    vec4* inside_points[3];  int nInsidePointCount = 0;
    vec4* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
    else {
        outside_points[nOutsidePointCount++] = &in_tri.p[0];
    }
    if (d1 >= 0) {
        inside_points[nInsidePointCount++] = &in_tri.p[1];
    }
    else {
        outside_points[nOutsidePointCount++] = &in_tri.p[1];
    }
    if (d2 >= 0) {
        inside_points[nInsidePointCount++] = &in_tri.p[2];
    }
    else {
        outside_points[nOutsidePointCount++] = &in_tri.p[2];
    }

    // Now classify triangle points, and break the input triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        for (int i = 0; i < 3; ++i)
            out_tri1.col[i] = in_tri.col[i];

        // The inside point is valid, so keep that...
        out_tri1.p[0] = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        float t;
        out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);

        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangle

        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];

        float t;
        out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);

        // The second triangle is composed of one of the inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);

        // Copy appearance info to new triangles
        for (int i = 0; i < 3; ++i)
        {
            out_tri1.col[i] = in_tri.col[i];
            out_tri2.col[i] = in_tri.col[i];
        }

        return 2; // Return two newly formed triangles which form a quad
    }
    return -1;
}

void DrawTriangleDepthBuffer(SDL_Renderer* renderer, triangle& triProjected, float* depthBuffer, int ScreenSizeX, int ScreenSizeY, int& numPixels)
{
    // Find the edges of the triangle (min and max in x and y)
    int minX = std::max(0, static_cast<int>(std::min(triProjected.p[0].x, std::min(triProjected.p[1].x, triProjected.p[2].x))));
    int maxX = std::min(ScreenSizeX - 1, static_cast<int>(std::max(triProjected.p[0].x, std::max(triProjected.p[1].x, triProjected.p[2].x))));
    int minY = std::max(0, static_cast<int>(std::min(triProjected.p[0].y, std::min(triProjected.p[1].y, triProjected.p[2].y))));
    int maxY = std::min(ScreenSizeY - 1, static_cast<int>(std::max(triProjected.p[0].y, std::max(triProjected.p[1].y, triProjected.p[2].y))));

    // Loop on each pixel in the rectangle enclosing the triangle
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Calculation of barycentric coordinates 
            float alpha = ((triProjected.p[1].y - triProjected.p[2].y) * (x - triProjected.p[2].x) + (triProjected.p[2].x - triProjected.p[1].x) * (y - triProjected.p[2].y)) /
                          ((triProjected.p[1].y - triProjected.p[2].y) * (triProjected.p[0].x - triProjected.p[2].x) + (triProjected.p[2].x - triProjected.p[1].x) * (triProjected.p[0].y - triProjected.p[2].y));
            float beta = ((triProjected.p[2].y - triProjected.p[0].y) * (x - triProjected.p[2].x) + (triProjected.p[0].x - triProjected.p[2].x) * (y - triProjected.p[2].y)) /
                         ((triProjected.p[1].y - triProjected.p[2].y) * (triProjected.p[0].x - triProjected.p[2].x) + (triProjected.p[2].x - triProjected.p[1].x) * (triProjected.p[0].y - triProjected.p[2].y));
            float gamma = 1.0f - alpha - beta;

            // If barycentric coordinates are valid (point inside the triangle) 
            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                // Calculation of pixel depth (an interpolation of the depths of the three points) 
                float pixelDepth = alpha * triProjected.p[0].z + beta * triProjected.p[1].z + gamma * triProjected.p[2].z;

                // If this pixel is closer to the camera than what is currently in the Depth Buffer
                int index = y * ScreenSizeX + x;
                if (pixelDepth < depthBuffer[index])
                {
                    // Update Depth Buffer
                    depthBuffer[index] = pixelDepth;
                    numPixels += 1;
                    
                    // Draw the pixel
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Exemple : blanc
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}


void DrawTriangleDepthBufferPerformance(
    SDL_Renderer* renderer,
    triangle& triProjected,
    triangle& triTransformed,
    float* depthBuffer,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    int& numPixels,
    Camera& camera,
    Lighting& light,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numShadow)
{
    const float scaleX = static_cast<float>(DEPTH_BUFFER_RES_X) / ScreenSizeX;
    const float scaleY = static_cast<float>(DEPTH_BUFFER_RES_Y) / ScreenSizeY;

    const int pixelWidth = static_cast<int>(std::ceil(1.0f / scaleX)); // Block width in screen space
    const int pixelHeight = static_cast<int>(std::ceil(1.0f / scaleY)); // Block height in screen space

    // Find the edges of the triangle (min and max in depth buffer space)
    int minX = std::max(0, static_cast<int>(std::min(triProjected.p[0].x, std::min(triProjected.p[1].x, triProjected.p[2].x)) * scaleX));
    int maxX = std::min(static_cast<int>(DEPTH_BUFFER_RES_X) - 1, static_cast<int>(std::max(triProjected.p[0].x, std::max(triProjected.p[1].x, triProjected.p[2].x)) * scaleX));
    int minY = std::max(0, static_cast<int>(std::min(triProjected.p[0].y, std::min(triProjected.p[1].y, triProjected.p[2].y)) * scaleY));
    int maxY = std::min(static_cast<int>(DEPTH_BUFFER_RES_Y) - 1, static_cast<int>(std::max(triProjected.p[0].y, std::max(triProjected.p[1].y, triProjected.p[2].y)) * scaleY));

    // Loop over depth buffer pixels
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Map depth buffer pixel to screen space
            float screenX = x / scaleX;
            float screenY = y / scaleY;

            // Calculate barycentric coordinates
            float alpha = ((triProjected.p[1].y - triProjected.p[2].y) * (screenX - triProjected.p[2].x) +
                           (triProjected.p[2].x - triProjected.p[1].x) * (screenY - triProjected.p[2].y)) /
                          ((triProjected.p[1].y - triProjected.p[2].y) * (triProjected.p[0].x - triProjected.p[2].x) +
                           (triProjected.p[2].x - triProjected.p[1].x) * (triProjected.p[0].y - triProjected.p[2].y));
            float beta = ((triProjected.p[2].y - triProjected.p[0].y) * (screenX - triProjected.p[2].x) +
                          (triProjected.p[0].x - triProjected.p[2].x) * (screenY - triProjected.p[2].y)) /
                         ((triProjected.p[1].y - triProjected.p[2].y) * (triProjected.p[0].x - triProjected.p[2].x) +
                          (triProjected.p[2].x - triProjected.p[1].x) * (triProjected.p[0].y - triProjected.p[2].y));
            float gamma = 1.0f - alpha - beta;

            // Check if the pixel is inside the triangle
            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                // Interpolate depth value
                float pixelDepth = alpha * triProjected.p[0].z + beta * triProjected.p[1].z + gamma * triProjected.p[2].z;

                // Calculate depth buffer index
                int index = y * DEPTH_BUFFER_RES_X + x;

                if (index < 0 || index >= DEPTH_BUFFER_RES_X * DEPTH_BUFFER_RES_Y) {
                fprintf(stderr, "Index out of bounds: %d\n", index);
                continue; // Skip this iteration
                }


                // Update depth buffer if the pixel is closer
                if (pixelDepth < depthBuffer[index])
                {
                    depthBuffer[index] = pixelDepth;
                    numPixels++;

                    vec4 point = triProjected.p[0] * alpha + triProjected.p[1] * beta + triProjected.p[2] * gamma;
                    
                    // Transform the pixel position to light space
                    vec4 lightSpacePos = light.computeSpaceMatrix() * point;
                    lightSpacePos = lightSpacePos / lightSpacePos.w; // Perspective divide

                    
                    // Convert to shadow map coordinates
                    int shadowMapX = static_cast<int>((lightSpacePos.x + 1.0f) * 0.5f * DEPTH_BUFFER_RES_X);
                    int shadowMapY = static_cast<int>((lightSpacePos.y + 1.0f) * 0.5f * DEPTH_BUFFER_RES_Y);
                        
                    // Check if the coordinates are within the bounds of the shadow map
                    bool isInShadow = false;
                    if (shadowMapX >= 0 && shadowMapX < DEPTH_BUFFER_RES_X && shadowMapY >= 0 && shadowMapY < DEPTH_BUFFER_RES_Y) {
                        // Get the depth value from the shadow map
                        float shadowMapDepth = shadowMap[shadowMapY * DEPTH_BUFFER_RES_X + shadowMapX];

                        // Add a small bias to the shadow map depth to avoid shadow acne
                        const float shadowBias = 0.001f; // Adjust this value as needed
                        if (pixelDepth > shadowMapDepth + shadowBias) {
                            isInShadow = true; // The pixel is in shadow
                        }   
                    }
                    vec4 PixelColor = vec4(255);
                    if (isInShadow) {
                        PixelColor = PixelColor * 0.0f; // Darken the color if in shadow
                        numShadow++;
                    }

                    // Render rectangle to cover the corresponding screen area
                    SDL_Rect rect = {
                        static_cast<int>(x / scaleX), // Top-left X
                        static_cast<int>(y / scaleY), // Top-left Y
                        pixelWidth,                   // Width
                        pixelHeight                   // Height
                    };
                    
                    SDL_SetRenderDrawColor(renderer, PixelColor.x, PixelColor.y, PixelColor.z, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }
}

void ShadowMap(
    triangle& tri,
    float* shadowMap,
    int& ScreenSizeX,
    int& ScreenSizeY,
    unsigned int DEPTH_BUFFER_RES_X,
    unsigned int DEPTH_BUFFER_RES_Y,
    int& numPixels,
    int& numShadow)
{
    

    const float scaleX = static_cast<float>(DEPTH_BUFFER_RES_X) / ScreenSizeX;
    const float scaleY = static_cast<float>(DEPTH_BUFFER_RES_Y) / ScreenSizeY;

    const int pixelWidth = static_cast<int>(std::ceil(1.0f / scaleX)); // Block width in screen space
    const int pixelHeight = static_cast<int>(std::ceil(1.0f / scaleY)); // Block height in screen space

    // Find the edges of the triangle (min and max in depth buffer space)
    int minX = std::max(0, static_cast<int>(std::min(tri.p[0].x, std::min(tri.p[1].x, tri.p[2].x)) * scaleX));
    int maxX = std::min(static_cast<int>(DEPTH_BUFFER_RES_X) - 1, static_cast<int>(std::max(tri.p[0].x, std::max(tri.p[1].x, tri.p[2].x)) * scaleX));
    int minY = std::max(0, static_cast<int>(std::min(tri.p[0].y, std::min(tri.p[1].y, tri.p[2].y)) * scaleY));
    int maxY = std::min(static_cast<int>(DEPTH_BUFFER_RES_Y) - 1, static_cast<int>(std::max(tri.p[0].y, std::max(tri.p[1].y, tri.p[2].y)) * scaleY));
    
    // Loop over depth buffer pixels
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Map depth buffer pixel to screen space
            float screenX = x / scaleX;
            float screenY = y / scaleY;

            // Calculate barycentric coordinates
            float alpha = ((tri.p[1].y - tri.p[2].y) * (screenX - tri.p[2].x) +
                           (tri.p[2].x - tri.p[1].x) * (screenY - tri.p[2].y)) /
                          ((tri.p[1].y - tri.p[2].y) * (tri.p[0].x - tri.p[2].x) +
                           (tri.p[2].x - tri.p[1].x) * (tri.p[0].y - tri.p[2].y));
            float beta = ((tri.p[2].y - tri.p[0].y) * (screenX - tri.p[2].x) +
                          (tri.p[0].x - tri.p[2].x) * (screenY - tri.p[2].y)) /
                         ((tri.p[1].y - tri.p[2].y) * (tri.p[0].x - tri.p[2].x) +
                          (tri.p[2].x - tri.p[1].x) * (tri.p[0].y - tri.p[2].y));
            float gamma = 1.0f - alpha - beta;

            // Check if the pixel is inside the triangle
            if (alpha >= 0 && beta >= 0 && gamma >= 0)
            {
                // Interpolate depth value
                float pixelDepth = alpha * tri.p[0].z + beta * tri.p[1].z + gamma * tri.p[2].z;

                // Calculate depth buffer index
                int index = y * DEPTH_BUFFER_RES_X + x;

                if (index < 0 || index >= DEPTH_BUFFER_RES_X * DEPTH_BUFFER_RES_Y) {
                fprintf(stderr, "Index out of bounds: %d\n", index);
                continue; // Skip this iteration
                }


                // Update depth buffer if the pixel is closer
                if (pixelDepth < shadowMap[index])
                {
                    numPixels++;
                    shadowMap[index] = pixelDepth;
                }
            }
        }
    }
}
