#include <string>
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

mat4 MatrixPointAt(vec4& pos, vec4& target, vec4& up)
{

    /*[[newRightx, newUpx, newForwardx, 0.0]
    [newRighty, newUpy, newForwardy, 0.0]
    [newRightz, newUpz, newForwardz, 0.0]
    [dp1, dp2, dp3, 0.0]]*/

    vec4 newForward = target - pos;
    newForward.normalize();

    float r = up.dot(newForward);
    vec4 a = newForward * r;
    vec4 newUp = up - a;
    newUp.normalize();

    vec4 newRight = newUp.crossProduct(newForward);

    mat4 matrix;
    matrix.MatrixMakeIdentity();

    matrix.m[0][0] = newRight.x;
    matrix.m[0][1] = newRight.y;
    matrix.m[0][2] = newRight.z;
    matrix.m[0][3] = -(pos.dot(newRight));

    matrix.m[1][0] = newUp.x;
    matrix.m[1][1] = newUp.y;
    matrix.m[1][2] = newUp.z;
    matrix.m[1][3] = -(pos.dot(newUp));

    matrix.m[2][0] = newForward.x;
    matrix.m[2][1] = newForward.y;
    matrix.m[2][2] = newForward.z;
    matrix.m[2][3] = -(pos.dot(newForward));
    return matrix;
};    

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
