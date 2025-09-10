#pragma once
#include "IApplet.h"
#include "system.h"
#include "JaDraw.h"
#include <math.h>

#define MAX_SEGMENTS 100
#define WORLD_MAX_X (2.0f)
#define WORLD_MAX_Y (1.0f)
#define SEGMENT_SPACING (0.2f)


typedef struct {
    Vec2 pos;
    float radius;
    bool active;
} Segment;

typedef struct {
    Segment segments[MAX_SEGMENTS];
    float directionRad;
    float speed;
} Player;

typedef struct {
    Player player;
} GameState;

static unsigned int player_get_num_segments(Player* p)
{
    for (unsigned int i = 0; i < MAX_SEGMENTS; i++)
    {
        if (!p->segments[i].active)
        {
            return i;
        }
    }
    return MAX_SEGMENTS;
}

static void player_add_segment(Player* p)
{
    unsigned int count = player_get_num_segments(p);
    if (count < 0) { count = 1; }
    if (count < MAX_SEGMENTS - 1)
    {
        p->segments[count].active = 1;
        p->segments[count].pos.x = p->segments[count-1].pos.x;
        p->segments[count].pos.y = p->segments[count-1].pos.y;
    }
}

static void init_game(GameState* state)
{
    state->player.speed = 0.25f;
    state->player.directionRad = 0.0f;
    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        state->player.segments[i].active = false;
        state->player.segments[i].pos.x = 0.0f;
        state->player.segments[i].pos.y = 0.0f;
        state->player.segments[i].radius = 1.0f;
    }
    state->player.segments[0].active = true;
    state->player.segments[0].pos.x = 0.0f;
    state->player.segments[0].pos.y = 0.0f;
    state->player.segments[0].radius = 1.0f;
    player_add_segment(&(state->player));
    player_add_segment(&(state->player));
    player_add_segment(&(state->player));
    unsigned int numSegments = player_get_num_segments(&state->player);
    for (unsigned int i = 1; i < numSegments; i++) {
        state->player.segments[i].pos.x = state->player.segments[i-1].pos.x - SEGMENT_SPACING;
        state->player.segments[i].pos.y = state->player.segments[i-1].pos.y;
    }
}



static void update_player(GameState* state, float dt, int inputDir)
{
    Player* player = &(state->player);
    Segment* head = &(player->segments[0]);

    // Steer left/right like a snake
    const float turnSpeed = 2.5f; // radians per second

    if (inputDir == 1) {
        player->directionRad += turnSpeed * dt;
    } else if (inputDir == -1) {
        player->directionRad -= turnSpeed * dt;
    }

    // Move head using speed and dt
    float dx = cosf(player->directionRad) * player->speed * dt;
    float dy = sinf(player->directionRad) * player->speed * dt;
    head->pos.x += dx;
    head->pos.y += dy;

    if (head->pos.x > WORLD_MAX_X)  head->pos.x = -WORLD_MAX_X;
    if (head->pos.x < -WORLD_MAX_X) head->pos.x = WORLD_MAX_X;
    if (head->pos.y > WORLD_MAX_Y)  head->pos.y = -WORLD_MAX_Y;
    if (head->pos.y < -WORLD_MAX_Y) head->pos.y = WORLD_MAX_Y;

    unsigned int numSegments = player_get_num_segments(player);
     for (unsigned int i = 1; i < numSegments; i++)
    {
        Vec2* prev = &player->segments[i - 1].pos; // The segment in front
        Vec2* curr = &player->segments[i].pos;   // The current segment

        // Get the vector from current segment to the previous one
        float dx = prev->x - curr->x;
        float dy = prev->y - curr->y;
        float dist = sqrtf(dx * dx + dy * dy);

        // If the segment is further from its parent than the allowed spacing, move it closer.
        if (dist > SEGMENT_SPACING)
        {
            // Calculate how much it needs to move to close the gap
            float moveAmount = dist - SEGMENT_SPACING;

            // Move the current segment towards the previous one by that amount
            curr->x += (dx / dist) * moveAmount;
            curr->y += (dy / dist) * moveAmount;
        }
    }
}

static void draw_game(GameState* state, JaDraw<WIDTH, HEIGHT>& canvas)
{
    canvas.clear(0);

    // Map world coordinates: x in [-2,2], y in [-1,1]
    for (int i = 0; i < MAX_SEGMENTS; i++) {
        const Segment* seg = &state->player.segments[i];
        if (!seg->active) break;
        // Map x from [-2,2] to [0, WIDTH-1]
        int px = (int)(((seg->pos.x + 2.0f) * 0.25f) * (WIDTH - 1));
        // Map y from [-1,1] to [0, HEIGHT-1]
        int py = (int)(((seg->pos.y + 1.0f) * 0.5f) * (HEIGHT - 1));
        int pr = (int)(seg->radius * 0.25f * (WIDTH + HEIGHT) * 0.25f);
        if (pr <= 0) pr = 1;
        canvas.drawPixel(px, py, Colors::White);
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

void SnakeGameApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs)
{
    static GameState state;
    static bool initialized = false;
    if (!initialized)
    {
        init_game(&state);
        initialized = true;
    }
    int inputDir = 0;
    if (inputs.rotation > 0) { inputDir = 1; } 
    else if (inputs.rotation < 0) { inputDir = -1; }

    update_player(&state, dt, inputDir);

    draw_game(&state, canvas);
}

const char* SnakeGameApplet::getName() const { return "Snake game"; }
