#include <string>
#include <vector>
#include <cmath>
#include <limits.h>
#include "../../include/core/vec4.hpp"
#include "../../include/core/mat4.hpp"
#include "../../include/core/Triangle.hpp"
#include "../../include/utils/utils.hpp"

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

int maxValue(int a, int b) {
    if (a > b)
        return a;
    else return b;
};

int minValue(int a, int b) {
    if (a > b)
        return b;
    else return a;
};

double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

// Convert time (in hours) to radians representing the sun's position
double timeToRadians(double& time) {
    // Convert time to fraction of a day (0 to 1)
    double fractionOfDay = fmod(time / 24.0, 1.0);
    // Convert fraction of a day to radians (0 to 2π)
    return fractionOfDay * 2 * M_PI;
}

vec4 normalise(vec4 vec) {
    if (vec.magnitude() == 0) {
        //throw std::invalid_argument("Cannot normalize a zero-length vector!");
        return vec;
        
    }
    return vec / vec.magnitude();
}

mat4 MatrixPointAt(vec4& pos, vec4& target, vec4& up)
{
    // Calculate new forward direction
    vec4 forward = (target - pos).normalize();

    // Calculate new up direction
    vec4 a = forward * up.dot(forward);
    vec4 newUp = (up - a).normalize();

    // New right direction
    vec4 right = newUp.crossProduct(forward);

    // Construct rotation matrix
    mat4 cameraRot;
    cameraRot.m[0][0] = right.x;   cameraRot.m[0][1] = right.y;   cameraRot.m[0][2] = right.z;   cameraRot.m[0][3] = -(pos.dot(right));;
    cameraRot.m[1][0] = newUp.x;   cameraRot.m[1][1] = newUp.y;   cameraRot.m[1][2] = newUp.z;   cameraRot.m[1][3] = -(pos.dot(newUp));;
    cameraRot.m[2][0] = forward.x; cameraRot.m[2][1] = forward.y; cameraRot.m[2][2] = forward.z; cameraRot.m[2][3] = -(pos.dot(forward));;
    cameraRot.m[3][0] = 0.0f;      cameraRot.m[3][1] = 0.0f;      cameraRot.m[3][2] = 0.0f;      cameraRot.m[3][3] = 1.0f;


    // Combine rotation and translation to form view matrix
    return cameraRot ;
}

mat4 MatrixTranslation(vec4& coordinates)
{
    mat4 matrix;
    matrix.m[0][3] = coordinates.x;
    matrix.m[1][3] = coordinates.y;
    matrix.m[2][3] = coordinates.z;
    matrix.m[0][0] = 1.0;
    matrix.m[1][1] = 1.0;
    matrix.m[2][2] = 1.0;
    matrix.m[3][3] = 1.0;
    return matrix;
}

mat4 MatrixRotationX(float& AngleRad)
{
    mat4 matrix;
    matrix.m[0][0] = 1.0;
    matrix.m[1][1] = cosf(AngleRad);
    matrix.m[1][2] = -(sinf(AngleRad));
    matrix.m[2][1] = sinf(AngleRad);
    matrix.m[2][2] = cosf(AngleRad);
    matrix.m[3][3] = 1.0;
    return matrix;
};

mat4 MatrixRotationY(float& AngleRad)
{
    mat4 matrix;
    matrix.m[0][0] = cosf(AngleRad);
    matrix.m[0][2] = sinf(AngleRad);
    matrix.m[2][0] = -(sinf(AngleRad));
    matrix.m[1][1] = 1.0;
    matrix.m[2][2] = cosf(AngleRad);
    matrix.m[3][3] = 1.0;
    return matrix;
};

mat4 MatrixRotationZ(float& AngleRad)
{
    mat4 matrix;
    matrix.m[0][0] = cosf(AngleRad);
    matrix.m[0][1] = -(sinf(AngleRad));
    matrix.m[1][0] = sinf(AngleRad);
    matrix.m[1][1] = cosf(AngleRad);
    matrix.m[2][2] = 1.0;
    matrix.m[3][3] = 1.0;
    return matrix;
};

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

void DrawTriangleDepthBuffer(SDL_Renderer* renderer, triangle& triProjected, std::vector<float>& depthBuffer, int ScreenSizeX, int ScreenSizeY, int& numPixels)
{
    // Calculation of barycentric coordinates for each pixel
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
