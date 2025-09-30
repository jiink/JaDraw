#pragma once
#include "IApplet.h"
#include "system.h"
#include "JaDraw.h"
#include <math.h>
#include <stdlib.h> // For rand(), qsort()
#include <time.h>   // For srand()

#define MAX_SEGMENTS 100
#define WORLD_MAX_X (2.0f)
#define WORLD_MAX_Y (1.0f)

#define HISTORY_BUFFER_SIZE (MAX_SEGMENTS * 10) 
#define SEGMENT_SPACING 16

// Radii are now used for both collision and drawing
#define HEAD_RADIUS 0.1f
#define FRUIT_RADIUS 0.1f

// --- NEW: Vector Math Helpers ---
static inline Vec2 vec2_add(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
static inline Vec2 vec2_sub(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
static inline Vec2 vec2_muls(Vec2 v, float s) { return {v.x * s, v.y * s}; }
static inline float vec2_length_sq(Vec2 v) { return v.x * v.x + v.y * v.y; }
static inline Vec2 vec2_normalize(Vec2 v) {
    float len_sq = vec2_length_sq(v);
    if (len_sq > 0.00001f) {
        float len = sqrtf(len_sq);
        return {v.x / len, v.y / len};
    }
    return {1.0f, 0.0f}; // Default direction
}

// MODIFIED: Segment now stores its own direction vector for drawing.
typedef struct {
    Vec2 pos;
    Vec2 dir;
    float radius;
    bool active;
} Segment;

typedef struct {
    Vec2 pos;
} Fruit;

typedef struct {
    Segment segments[MAX_SEGMENTS];
    float directionRad;
    float speed;
    
    Vec2 headHistory[HISTORY_BUFFER_SIZE];
    int historyIndex; 
} Player;

// MODIFIED: GameState now includes a game-over flag.
typedef struct {
    Player player;
    Fruit fruit;
    bool is_game_over;
    // Speed scaling: start fast, then ease down over a number of fruits
    int fruits_eaten;
    float start_speed;
    float target_speed;
    int speed_ease_fruits; // number of fruits over which to ease speed
} GameState;


static void draw_filled_circle(JaDraw<WIDTH, HEIGHT>& canvas, int centerX, int centerY, int radius, bool white)
{
    // Iterate through a bounding box around the circle
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            // Check if the point (x, y) is inside the circle using the distance formula
            if (x*x + y*y <= radius*radius) {
                int px = centerX + x;
                int py = centerY + y;
                
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    canvas.drawPixel(px, py, white ? Colors::White : Colors::Black);
                }
            }
        }
    }
}

// --- NEW: Polygon Drawing ---
// Comparison function for qsort
static int compare_floats(const void* a, const void* b) {
   float fa = *(const float*)a;
   float fb = *(const float*)b;
   return (fa > fb) - (fa < fb);
}

// A portable, static function to draw a filled polygon using only drawPixel.
// Implements a scanline rasterization algorithm.
static void draw_filled_polygon(JaDraw<WIDTH, HEIGHT>& canvas, const Vec2* points, int numPoints, bool white)
{
    if (numPoints < 3) return;

    // Find Y bounding box
    float minY = points[0].y, maxY = points[0].y;
    for (int i = 1; i < numPoints; ++i) {
        if (points[i].y < minY) minY = points[i].y;
        if (points[i].y > maxY) maxY = points[i].y;
    }
    
    // Clamp to screen bounds
    int iMinY = (int)ceil(minY); if (iMinY < 0) iMinY = 0;
    int iMaxY = (int)floor(maxY); if (iMaxY >= HEIGHT) iMaxY = HEIGHT - 1;

    float intersections[numPoints];

    for (int y = iMinY; y <= iMaxY; ++y) {
        int num_intersections = 0;
        float fy = (float)y + 0.5f;

        for (int i = 0; i < numPoints; ++i) {
            const Vec2& p1 = points[i];
            const Vec2& p2 = points[(i + 1) % numPoints];

            if ((p1.y <= fy && p2.y > fy) || (p2.y <= fy && p1.y > fy)) {
                float x = (fy - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
                if (num_intersections < numPoints) {
                    intersections[num_intersections++] = x;
                }
            }
        }
        
        qsort(intersections, num_intersections, sizeof(float), compare_floats);
        
        for (int i = 0; i < num_intersections; i += 2) {
            if (i + 1 < num_intersections) {
                int x_start = (int)ceil(intersections[i]);
                int x_end = (int)floor(intersections[i+1]);

                if (x_start < 0) x_start = 0;
                if (x_end >= WIDTH) x_end = WIDTH - 1;

                for (int x = x_start; x <= x_end; ++x) {
                    canvas.drawPixel(x, y, white ? Colors::White : Colors::Black);
                }
            }
        }
    }
}


static void respawn_fruit(Fruit* fruit)
{
    float randX = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    float randY = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;

    fruit->pos.x = randX * (WORLD_MAX_X * 0.9f); // Spawn slightly away from edge
    fruit->pos.y = randY * (WORLD_MAX_Y * 0.9f);
}

static unsigned int player_get_num_segments(Player* p)
{
    for (unsigned int i = 0; i < MAX_SEGMENTS; i++) {
        if (!p->segments[i].active) {
            return i;
        }
    }
    return MAX_SEGMENTS;
}

static void player_add_segment(Player* p)
{
    unsigned int count = player_get_num_segments(p);
    if (count < MAX_SEGMENTS) {
        p->segments[count].active = true;
        // Initialize the new segment's position, direction and radius immediately
        // so it doesn't flash at the origin for one frame. Use the history buffer
        // to place it where the tail should be.
        int histIndex = (p->historyIndex - (count * SEGMENT_SPACING) + HISTORY_BUFFER_SIZE) % HISTORY_BUFFER_SIZE;
        p->segments[count].pos = p->headHistory[histIndex];
        if (count > 0) {
            // Copy direction and radius from previous segment to avoid visual jump
            p->segments[count].dir = p->segments[count - 1].dir;
            p->segments[count].radius = p->segments[count - 1].radius;
        } else {
            p->segments[count].dir = {1.0f, 0.0f};
            p->segments[count].radius = HEAD_RADIUS;
        }
    }
}

static void init_game(GameState* state)
{
    // Start with a high initial speed which will ease down over the first
    // `speed_ease_fruits` fruits to `target_speed`.
    state->start_speed = 0.9f;      // fast at the beginning
    state->target_speed = 0.45f;    // slower steady speed after easing
    state->speed_ease_fruits = 10;  // ease over 10 fruits
    state->fruits_eaten = 0;
    state->player.speed = state->start_speed;
    state->player.directionRad = 0.0f;
    state->player.historyIndex = 0;
    state->is_game_over = false;

    for (int i = 0; i < MAX_SEGMENTS; i++) {
        state->player.segments[i].active = false;
        state->player.segments[i].pos = {0.0f, 0.0f};
        state->player.segments[i].dir = {1.0f, 0.0f};
        state->player.segments[i].radius = HEAD_RADIUS;
    }
    
    state->player.segments[0].active = true;
    state->player.segments[0].pos = {0.0f, 0.0f};

    for (int i = 0; i < HISTORY_BUFFER_SIZE; i++) {
        state->player.headHistory[i] = state->player.segments[0].pos;
    }

    player_add_segment(&(state->player));
    player_add_segment(&(state->player));
    player_add_segment(&(state->player));

    respawn_fruit(&state->fruit);
}

// MODIFIED: Update function now handles game-over state and self-collision.
static void update_player(GameState* state, float dt, int inputDir)
{
    Player* player = &(state->player);
    if (state->is_game_over) return;

    Segment* head = &(player->segments[0]);
    
    // --- Update direction and position ---
    const float turnSpeed = 3.5f;
    if (inputDir == 1) {
        player->directionRad += turnSpeed * dt;
    } else if (inputDir == -1) {
        player->directionRad -= turnSpeed * dt;
    }

    head->pos.x += cosf(player->directionRad) * player->speed * dt;
    head->pos.y += sinf(player->directionRad) * player->speed * dt;

    // --- Check for wall collision ---
    if (head->pos.x >= WORLD_MAX_X || head->pos.x <= -WORLD_MAX_X ||
        head->pos.y >= WORLD_MAX_Y || head->pos.y <= -WORLD_MAX_Y)
    {
        state->is_game_over = true;
        return;
    }

    // --- Update history and segment positions ---
    player->headHistory[player->historyIndex] = head->pos;
    player->historyIndex = (player->historyIndex + 1) % HISTORY_BUFFER_SIZE;

    unsigned int numSegments = player_get_num_segments(player);
    for (unsigned int i = 1; i < numSegments; i++) {
        int historyLookupIndex = (player->historyIndex - i * SEGMENT_SPACING + HISTORY_BUFFER_SIZE) % HISTORY_BUFFER_SIZE;
        player->segments[i].pos = player->headHistory[historyLookupIndex];
    }

    // --- Update segment directions and radii for polygon drawing ---
    head->dir = {cosf(player->directionRad), sinf(player->directionRad)};

    for (unsigned int i = 1; i < numSegments; i++) {
        Vec2 diff = vec2_sub(player->segments[i-1].pos, player->segments[i].pos);
        if (vec2_length_sq(diff) > 0.0001f) {
            player->segments[i].dir = vec2_normalize(diff);
        } else {
            player->segments[i].dir = player->segments[i-1].dir;
        }
    }
    for (unsigned int i = 0; i < numSegments; i++) {
        float t = (numSegments > 1) ? ((float)i / (float)(numSegments - 1)) : 0.0f;
        float scale = 1.0f - 0.2f * t; // Taper
        player->segments[i].radius = HEAD_RADIUS * scale;
    }

    // --- NEW: Check for self-collision (more forgiving) ---
    // Skip a larger neck region to avoid false positives when turning sharply.
    // Also require deeper overlap before triggering game over by applying
    // a collision_tolerance multiplier (<1.0 means closer required).
    const unsigned int collision_neck_skip = 6; // skip first 6 segments (0..5)
    const float collision_tolerance = 0.80f;    // require 80% of radii sum overlap
    if (numSegments > collision_neck_skip) {
        for (unsigned int i = collision_neck_skip; i < numSegments; ++i) {
            const Segment* body_seg = &player->segments[i];
            float dx_self = head->pos.x - body_seg->pos.x;
            float dy_self = head->pos.y - body_seg->pos.y;
            float distSq_self = dx_self * dx_self + dy_self * dy_self;
            float radiiSum_self = head->radius + body_seg->radius;
            float triggerDist = radiiSum_self * collision_tolerance;

            if (distSq_self < triggerDist * triggerDist) {
                state->is_game_over = true;
                return;
            }
        }
    }

    // --- Check for fruit collision ---
    float dx = head->pos.x - state->fruit.pos.x;
    float dy = head->pos.y - state->fruit.pos.y;
    float distSq = dx * dx + dy * dy;
    float radiiSum = head->radius + FRUIT_RADIUS;

    if (distSq < radiiSum * radiiSum) {
        player_add_segment(player);
        respawn_fruit(&state->fruit);
        // --- Adjust speed: ease from start_speed to target_speed over
        // the first `speed_ease_fruits` fruits eaten.
        state->fruits_eaten += 1;
        int easeCount = state->speed_ease_fruits;
        if (easeCount <= 0) easeCount = 1;
        float t = (float)state->fruits_eaten / (float)easeCount;
        if (t > 1.0f) t = 1.0f;
        // Linear interpolation
        player->speed = state->start_speed + (state->target_speed - state->start_speed) * t;
    }
}

// MODIFIED: This function now draws a pulsing border and bouncing game over text.
static void draw_game(GameState* state, JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis)
{
    canvas.clear(0);

    // --- NEW: Draw Pulsing Border ---
    float pulse_visibility = (cosf(millis / 400.0f) + 1.0f) * 0.5f; // Oscillates between 0 and 1
    for (int x = 0; x < WIDTH; ++x) {
        canvas.drawPixel(x, 0, Colors::White);
        canvas.drawPixel(x, HEIGHT - 1, Colors::White);
    }
    for (int y = 1; y < HEIGHT - 1; ++y) {
        canvas.drawPixel(0, y, Colors::White);
        canvas.drawPixel(WIDTH - 1, y, Colors::White);
    }
    if (pulse_visibility > 0.2f) { // Flicker off briefly
        for (int x = 1; x < WIDTH; ++x) {
            canvas.drawPixel(x, 1, Colors::White);
            canvas.drawPixel(x, HEIGHT - 2, Colors::White);
        }
        for (int y = 1; y < HEIGHT - 1; ++y) {
            canvas.drawPixel(1, y, Colors::White);
            canvas.drawPixel(WIDTH - 2, y, Colors::White);
        }
    }

    // --- Draw Fruit (as a circle) ---
    const float world_to_pixel_scale_y = (float)HEIGHT / (2.0f * WORLD_MAX_Y);
    int fruit_pixel_radius = (int)(FRUIT_RADIUS * world_to_pixel_scale_y);
    int fruit_px = (int)(((state->fruit.pos.x + WORLD_MAX_X) / (2.0f * WORLD_MAX_X)) * (WIDTH - 1));
    int fruit_py = (int)(((state->fruit.pos.y + WORLD_MAX_Y) / (2.0f * WORLD_MAX_Y)) * (HEIGHT - 1));
    float oscillate = 1.0f + cos(millis/256) * 0.2f;
    draw_filled_circle(canvas, fruit_px, fruit_py, FRUIT_RADIUS * world_to_pixel_scale_y * oscillate, true);

    // --- Draw Player (as a polygon) ---
    unsigned int numSegments = player_get_num_segments(&state->player);
    if (numSegments == 0) return;

    // 1. Generate the polygon vertices in world space
    const int max_points = MAX_SEGMENTS * 2 + 2;
    Vec2 world_points[max_points];
    int point_count = 0;
    
    const Segment* head = &state->player.segments[0];
    world_points[point_count++] = vec2_add(head->pos, vec2_muls(head->dir, head->radius)); // Tip of the head

    for (unsigned int i = 0; i < numSegments; ++i) { // Right side
        const Segment* seg = &state->player.segments[i];
        Vec2 perp_dir = { -seg->dir.y, seg->dir.x }; // Rotated +90 deg
        world_points[point_count++] = vec2_add(seg->pos, vec2_muls(perp_dir, seg->radius));
    }
    for (int i = numSegments - 1; i >= 0; --i) { // Left side (reversed)
        const Segment* seg = &state->player.segments[i];
        Vec2 perp_dir = { seg->dir.y, -seg->dir.x }; // Rotated -90 deg
        world_points[point_count++] = vec2_add(seg->pos, vec2_muls(perp_dir, seg->radius));
    }

    // 2. Convert world-space vertices to pixel-space
    Vec2 pixel_points[max_points];
    for(int i = 0; i < point_count; ++i) {
        pixel_points[i].x = ((world_points[i].x + WORLD_MAX_X) / (2.0f * WORLD_MAX_X)) * (WIDTH - 1);
        pixel_points[i].y = ((world_points[i].y + WORLD_MAX_Y) / (2.0f * WORLD_MAX_Y)) * (HEIGHT - 1);
    }
    
    // 3. Draw the final polygon
    draw_filled_polygon(canvas, pixel_points, point_count, true);

    // MODIFIED: Game over text now bounces
    if (state->is_game_over)
    {
        // Use millis to create a smooth bouncing effect
        float bounce_offset = -abs(cosf(millis / 1400.0f) * 20.0f); // Bounce 4 pixels up/down
        int text_y = 25 + (int)bounce_offset;

        canvas.drawText("GAME OVER", 15, text_y + 1, 2, Colors::Black, false);
        canvas.drawText("GAME OVER", 14, text_y, 2, Colors::White, false);
    }
}

class SnakeGameApplet : public IApplet {
public:
    SnakeGameApplet();
    ~SnakeGameApplet() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
};
SnakeGameApplet::SnakeGameApplet() {}
void SnakeGameApplet::setup() {}

// MODIFIED: Main loop now handles the game-over state and allows restarting.
void SnakeGameApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs)
{
    static GameState state;
    static bool initialized = false;
    if (!initialized)
    {
        srand(time(NULL));
        init_game(&state);
        initialized = true;
    }
    static unsigned long millis = 0;
    millis += (unsigned long)(dt * 1000.0f);   
    // If the game is over, wait for a button press to restart
    if (state.is_game_over) {
        if (inputs.pressed) {
            init_game(&state);
        }
    } else {
        // Otherwise, run the game update
        int inputDir = 0;
        if (inputs.rotation > 0.1) { inputDir = 1; }
        else if (inputs.rotation < -0.1) { inputDir = -1; }

        update_player(&state, dt, inputDir);
    }

    draw_game(&state, canvas, millis);
}

const char* SnakeGameApplet::getName() const { return "Snake game"; }