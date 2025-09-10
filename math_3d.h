#ifndef MATH_3D_H
#define MATH_3D_H

#include <math.h>

// --- Structures ---
struct Vec2i { int x, y; };

typedef struct {
    float x, y, z;
} Vec3f;

typedef struct {
    float m[4][4];
} Mat4f;


// --- Vector Helper Functions ---

static inline Vec3f vec3_subtract(Vec3f a, Vec3f b) {
    return (Vec3f){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline Vec3f vec3_cross(Vec3f a, Vec3f b) {
    return (Vec3f){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline float vec3_dot(Vec3f a, Vec3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float vec3_length(Vec3f v) {
    return sqrtf(vec3_dot(v, v));
}

static inline Vec3f vec3_normalize(Vec3f v) {
    float l = vec3_length(v);
    if (l > 0.0f) {
        return (Vec3f){v.x / l, v.y / l, v.z / l};
    }
    return (Vec3f){0, 0, 0};
}


// --- Matrix Transformation Functions ---

/**
 * @brief Returns an identity matrix.
 */
static inline Mat4f matrix_identity() {
    Mat4f result = {{0}}; // Initialize all elements to 0
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

/**
 * @brief Creates a translation matrix.
 */
static inline Mat4f matrix_translate(Vec3f v) {
    Mat4f result = matrix_identity();
    result.m[0][3] = v.x;
    result.m[1][3] = v.y;
    result.m[2][3] = v.z;
    return result;
}

/**
 * @brief Creates a scaling matrix.
 */
static inline Mat4f matrix_scale(Vec3f v) {
    Mat4f result = matrix_identity();
    result.m[0][0] = v.x;
    result.m[1][1] = v.y;
    result.m[2][2] = v.z;
    return result;
}

/**
 * @brief Creates a rotation matrix around the Y axis.
 */
static inline Mat4f matrix_rotate_y(float angle_rad) {
    Mat4f result = matrix_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    return result;
}

/**
 * @brief Multiplies two 4x4 matrices (a * b).
 */
static inline Mat4f matrix_multiply(Mat4f a, Mat4f b) {
    Mat4f result = {{0}};
    for (int i = 0; i < 4; ++i) { // row of a
        for (int j = 0; j < 4; ++j) { // column of b
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}


// --- Projection and View Functions ---

/**
 * @brief Creates a perspective projection matrix.
 */
static inline Mat4f matrix_perspective(float fov_y_rad, float aspect, float z_near, float z_far) {
    Mat4f result = {{0}};
    float tan_half_fovy = tanf(fov_y_rad / 2.0f);
    
    result.m[0][0] = 1.0f / (aspect * tan_half_fovy);
    result.m[1][1] = 1.0f / (tan_half_fovy);
    result.m[2][2] = -(z_far + z_near) / (z_far - z_near);
    result.m[3][2] = -1.0f;
    result.m[2][3] = -(2.0f * z_far * z_near) / (z_far - z_near);
    
    return result;
}

/**
 * @brief Creates a view matrix to position the camera (the "look-at" matrix).
 */
static inline Mat4f matrix_look_at(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f f = vec3_normalize(vec3_subtract(center, eye)); // Forward
    Vec3f s = vec3_normalize(vec3_cross(f, up));          // Right
    Vec3f u = vec3_cross(s, f);                          // Up
    
    Mat4f result = matrix_identity();
    
    result.m[0][0] = s.x;
    result.m[0][1] = s.y;
    result.m[0][2] = s.z;
    
    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;
    
    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;
    
    result.m[0][3] = -vec3_dot(s, eye);
    result.m[1][3] = -vec3_dot(u, eye);
    result.m[2][3] = vec3_dot(f, eye);
    
    return result;
}
/**
 * @brief Transforms a 3D vertex, performs perspective division, and maps to screen coordinates.
 *
 * This version no longer culls vertices outside the L/R/T/B planes, allowing for
 * triangles to be partially off-screen. It only rejects vertices behind the camera.
 */
static inline bool project_vertex(const Vec3f* model_vertex, const Mat4f* mvp, Vec2i* out_screen_pos, int screen_w, int screen_h)
{
    // 1. Apply the MVP matrix to the vertex
    float clip_x = model_vertex->x * mvp->m[0][0] + model_vertex->y * mvp->m[0][1] + model_vertex->z * mvp->m[0][2] + 1.0f * mvp->m[0][3];
    float clip_y = model_vertex->x * mvp->m[1][0] + model_vertex->y * mvp->m[1][1] + model_vertex->z * mvp->m[1][2] + 1.0f * mvp->m[1][3];
    float clip_w = model_vertex->x * mvp->m[3][0] + model_vertex->y * mvp->m[3][1] + model_vertex->z * mvp->m[3][2] + 1.0f * mvp->m[3][3];

    // 2. Perform Near-Plane Clipping.
    // This is the most important check. If a vertex is behind the camera (w < 0),
    // the perspective division will flip coordinates and cause massive visual errors.
    // Rejecting the whole triangle is a simple and effective way to handle this.
    // A small epsilon (0.001f) prevents division by zero.
    if (clip_w < 0.001f) {
        return false;
    }

    // 3. Perspective Division (Clip Space -> NDC)
    float ndc_x = clip_x / clip_w;
    float ndc_y = clip_y / clip_w;

    // 4. Viewport Transform (NDC -> Screen Coordinates)
    // We REMOVED the culling check here. The coordinates are calculated even if they
    // are outside the [-1, 1] NDC range.
    out_screen_pos->x = (int)((ndc_x + 1.0f) * 0.5f * screen_w);
    out_screen_pos->y = (int)((1.0f - ndc_y) * 0.5f * screen_h);

    return true;
}

// Transforms a 3D vector by a 4x4 matrix (ignoring perspective division)
// Useful for converting vertices from model to world space.
static inline Vec3f transform_vertex(const Vec3f* v, const Mat4f* m) {
    return (Vec3f) {
        v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + m->m[0][3],
        v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + m->m[1][3],
        v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + m->m[2][3]
    };
}

#endif // MATH_3D_H
