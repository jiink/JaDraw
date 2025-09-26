#pragma once
#include "IApplet.h"
#include "system.h"
#include "JaDraw.h"
#include <math.h>

#define MAX_SEGMENTS 100
#define WORLD_MAX_X (2.0f)
#define WORLD_MAX_Y (1.0f)
// The spacing is now determined by speed and framerate, so this define is no longer needed for movement.
// #define SEGMENT_SPACING (0.08f) 

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
    if (count < MAX_SEGMENTS)
    {
        p->segments[count].active = true;
        if (count > 0) {
            p->segments[count].pos = p->segments[count - 1].pos;
        }
    }
}

static void init_game(GameState* state)
{
    state->player.speed = 1.0f;
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

    // Initial spacing is still useful for the start of the game
    float initial_spacing = 0.08f;
    unsigned int numSegments = player_get_num_segments(&state->player);
    for (unsigned int i = 1; i < numSegments; i++) {
        state->player.segments[i].pos.x = state->player.segments[i-1].pos.x - initial_spacing;
        state->player.segments[i].pos.y = state->player.segments[i-1].pos.y;
    }
}


// FIXED: Replaced update logic with the classic "shift the body" algorithm.
static void update_player(GameState* state, float dt, int inputDir)
{
    Player* player = &(state->player);
    Segment* head = &(player->segments[0]);
    unsigned int numSegments = player_get_num_segments(player);

    // Part 1: Shift the entire body FIRST.
    // Each segment moves into the previous position of the segment in front of it.
    // This must be done in a reverse loop, from tail to head.
    for (unsigned int i = numSegments - 1; i > 0; i--)
    {
        player->segments[i].pos = player->segments[i - 1].pos;
    }

    // Part 2: Now, steer and move the head to its new position.
    const float turnSpeed = 2.5f;

    if (inputDir == 1) {
        player->directionRad += turnSpeed * dt;
    } else if (inputDir == -1) {
        player->directionRad -= turnSpeed * dt;
    }

    head->pos.x += cosf(player->directionRad) * player->speed * dt;
    head->pos.y += sinf(player->directionRad) * player->speed * dt;

    // World boundary wrapping (this is now safe to do)
    if (head->pos.x > WORLD_MAX_X)  head->pos.x = -WORLD_MAX_X;
    if (head->pos.x < -WORLD_MAX_X) head->pos.x = WORLD_MAX_X;
    if (head->pos.y > WORLD_MAX_Y)  head->pos.y = -WORLD_MAX_Y;
    if (head->pos.y < -WORLD_MAX_Y) head->pos.y = WORLD_MAX_Y;
}


static void draw_game(GameState* state, JaDraw<WIDTH, HEIGHT>& canvas)
{
    canvas.clear(0);
    unsigned int numSegments = player_get_num_segments(&state->player);

    for (unsigned int i = 0; i < numSegments; i++) {
        const Segment* seg = &state->player.segments[i];
        
        int px = (int)(((seg->pos.x + WORLD_MAX_X) / (2.0f * WORLD_MAX_X)) * (WIDTH - 1));
        int py = (int)(((seg->pos.y + WORLD_MAX_Y) / (2.0f * WORLD_MAX_Y)) * (HEIGHT - 1));
        
        if (i == 0) {
            canvas.drawPixel(px, py, Colors::Green); // Head
        } else {
            canvas.drawPixel(px, py, Colors::White); // Body
        }
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
    if (inputs.rotation > 0.1) { inputDir = 1; }
    else if (inputs.rotation < -0.1) { inputDir = -1; }

    update_player(&state, dt, inputDir);

    draw_game(&state, canvas);
}

const char* SnakeGameApplet::getName() const { return "Snake game"; }