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
 * @brief Transforms a 3D vertex by an MVP matrix, performs perspective division,
 * and maps it to 2D screen coordinates.
 * 
 * This function simulates the vertex processing stage of a standard graphics pipeline.
 * Assumes that WIDTH and HEIGHT macros are defined for the screen dimensions.
 *
 * @param model_vertex Pointer to the 3D vertex in its local model space.
 * @param mvp          Pointer to the combined Model-View-Projection matrix.
 * @param out_screen_pos Pointer to a Vec2i struct to store the final screen coordinates.
 * @return             true if the vertex is within the viewing frustum (visible), false otherwise.
 */
static inline bool project_vertex(const Vec3f* model_vertex, const Mat4f* mvp, Vec2i* out_screen_pos, int screen_w, int screen_h) {
    // We assume WIDTH and HEIGHT are available, as they were in the original prompt.
    // e.g. #define WIDTH 128
    //      #define HEIGHT 64

    // 1. Apply the MVP matrix to the vertex (Model -> World -> View -> Clip Space)
    // We treat the input as a 4D vector (x, y, z, 1) for the matrix multiplication.
    float clip_x = model_vertex->x * mvp->m[0][0] + model_vertex->y * mvp->m[0][1] + model_vertex->z * mvp->m[0][2] + 1.0f * mvp->m[0][3];
    float clip_y = model_vertex->x * mvp->m[1][0] + model_vertex->y * mvp->m[1][1] + model_vertex->z * mvp->m[1][2] + 1.0f * mvp->m[1][3];
    // float clip_z = model_vertex->x * mvp->m[2][0] + model_vertex->y * mvp->m[2][1] + model_vertex->z * mvp->m[2][2] + 1.0f * mvp->m[2][3];
    float clip_w = model_vertex->x * mvp->m[3][0] + model_vertex->y * mvp->m[3][1] + model_vertex->z * mvp->m[3][2] + 1.0f * mvp->m[3][3];

    // 2. Perform basic clipping.
    // If w is near zero or negative, the point is behind or on the camera plane.
    // This simple check prevents division by zero and incorrect projections.
    if (clip_w < 0.1f) { // 0.1f is a typical near-plane value
        return false;
    }

    // 3. Perspective Division (Clip Space -> Normalized Device Coordinates, NDC)
    // This transforms the viewing frustum into a cube from [-1, 1] on all axes.
    float ndc_x = clip_x / clip_w;
    float ndc_y = clip_y / clip_w;
    // float ndc_z = clip_z / clip_w; // Z is not needed for 2D screen mapping

    // 4. Simple frustum culling (check if outside the NDC cube).
    // A full clipping implementation is complex; for now, we just reject vertices
    // that are clearly off-screen.
    if (ndc_x < -1.0f || ndc_x > 1.0f || ndc_y < -1.0f || ndc_y > 1.0f) {
        return false;
    }

    // 5. Viewport Transform (NDC -> Screen Coordinates)
    // Maps the [-1, 1] NDC range to the [0, WIDTH] and [0, HEIGHT] screen range.
    out_screen_pos->x = (int)((ndc_x + 1.0f) * 0.5f * screen_w);
    // Y is flipped because NDC +1 is 'up', but screen coordinate 0 is at the top.
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
