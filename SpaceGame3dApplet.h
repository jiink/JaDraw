#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"
#include "math_3d.h"

#define MAX_ASTEROIDS 20
#define MAX_BULLETS 15

#define PLAYER_WIDTH 0.12f  
#define PLAYER_HEIGHT 0.1f  
#define PLAYER_Y_POS -0.85f
#define PLAYER_SPEED 2.5f
#define PLAYER_INITIAL_HP 100

#define BULLET_SPEED 3.0f
#define BULLET_WIDTH 0.03f
#define BULLET_HEIGHT 0.05f
#define BULLET_DAMAGE 5.0f
#define LASER_CHARGE_TIME_FOR_BULLET 1.0f
#define LASER_MAX_CHARGE_TIME 2.5f
#define LASER_WIDTH 0.04f
#define LASER_DURATION 0.65f
#define LASER_BASE_DAMAGE 200.0f

#define ASTEROID_SPAWN_INTERVAL 4.5f
#define ASTEROID_MIN_SPEED 0.2f
#define ASTEROID_MAX_SPEED 0.6f
#define ASTEROID_SPEED_INCREASE 0.01f;
#define ASTEROID_MIN_SIZE 0.2f
#define ASTEROID_MAX_SIZE 0.5f

//struct Vec2i { int x, y; };
//struct Vec3f { float x, y, z; };
//struct Mat4f { float m[4][4]; };

struct Vector2D{
    float x, y;
};
struct Player {
    Vector2D pos;
    float vel;
    float targetVel;
    float hp;
    float laserChargeTime;
};

struct Bullet {
    bool active;
    Vector2D pos;
    Vector2D vel;
};

struct Asteroid {
    bool active;
    Vector2D pos;
    Vector2D vel;
    float size;
    float hp;
    int flashTimerMs;
};

struct Laser {
    bool active;
    float x_pos; // Laser is a vertical beam
    float power;
    float duration;
};

struct GameState {
    Player player;
    Asteroid asteroids[MAX_ASTEROIDS];
    Bullet bullets[MAX_BULLETS];
    Laser laser;

    float asteroidSpawnTimer;
    bool fireBtnWasPressed;
    bool gameOver;
    int score;
    int nextTimeGoalMs;
    float asteroidBaseSpeed;
};

struct GameInputData {
    bool fireBtnPressed;
    float stickX; // -1.0 (left) to 1.0 (right)
};

const Vec3f CUBE_VERTICES[] = {
    {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}
};
const int CUBE_INDICES[] = {
    0, 2, 1, 0, 3, 2, // Back face
    1, 6, 5, 1, 2, 6, // Right face
    5, 7, 4, 5, 6, 7, // Front face
    4, 3, 0, 4, 7, 3, // Left face
    3, 6, 2, 3, 7, 6, // Top face
    4, 1, 5, 4, 0, 1  // Bottom face
};
const int CUBE_NUM_INDICES = 36;

const Vec3f ASTEROID_VERTICES[] = {
    {-0.5360100f, 0.5601110f, -0.1807760f},     {-0.05187100f, -0.1256810f, -0.5897210f},
    {-0.5453930f, -0.1025600f, 0.003804000f},     {0.4016950f, 0.4737450f, -0.2939520f},
    {-0.5151500f, 0.1303340f, 0.4875140f},     {0.06148500f, -0.5309490f, 0.2525580f},
    {-0.0009630000f, 0.5017880f, 0.3516430f},     {0.4872200f, 0.02320100f, 0.3961280f},
    {0.4230950f, -0.1500950f, -0.3389490f},
};

const int ASTEROID_INDICES[] = {
    1, 0, 3, 0, 4, 6, 7, 4, 5, 1, 8, 5, 
    3, 0, 6, 4, 2, 5, 6, 7, 3, 7, 6, 4, 
    1, 3, 8, 4, 0, 2, 5, 2, 1, 2, 0, 1, 
    8, 3, 7, 5, 8, 7
};

const int ASTEROID_NUM_INDICES = 42;

const Vec3f QUAD_VERTICES[] = {
    {-0.5f, 0.0f, -0.5f}, { 0.5f, 0.0f, -0.5f},
    {-0.5f, 0.0f,  5.5f}, { 0.5f, 0.0f,  5.5f},
};
const int QUAD_INDICES[] = {
    2, 1, 3, 2, 0, 1
};
const int QUAD_NUM_INDICES = 6;


const Vec3f SHIP_VERTICES[] = {
    {0.000000f, 0.09429200f, -0.8442920f},     {0.000000f, -0.05570800f, 0.4057080f},
    {-0.06993900f, 0.2442920f, 0.4057080f},     {0.06993900f, 0.2442920f, 0.4057080f},
    {-0.8699390f, 0.09429200f, 0.1557080f},     {0.8699390f, 0.09429200f, 0.1557080f},
    {-0.2704030f, 0.1381030f, -0.07282900f},     {0.2704030f, 0.1381030f, -0.07282900f},
    {-0.8453970f, 0.05703800f, 0.1043630f},     {0.8453970f, 0.05703800f, 0.1043630f},
    {-0.8453970f, 0.05703800f, 0.1043630f},     {-0.8048760f, 0.06294900f, -0.5191000f},
    {-0.7264710f, 0.04886800f, 0.07942300f},     {0.7264710f, 0.04886800f, 0.07942300f},
    {0.8048760f, 0.06294900f, -0.5191000f},
};

const int SHIP_INDICES[] = {
    2, 1, 3, 4, 1, 2, 5, 3, 1, 4, 6, 1, 
    0, 7, 1, 1, 7, 5, 1, 6, 0, 11, 12, 10, 
    13, 14, 9
};

const int SHIP_NUM_INDICES = 27;



static GameInputData gameInputData;

class SpaceGame3dApplet : public IApplet {
public:
    SpaceGame3dApplet();
    ~SpaceGame3dApplet() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
};

static void init_game(GameState* state);
static void update_player(GameState* state, float dt, const GameInputData* inputs);
static void update_bullets(GameState* state, float dt);
static void update_asteroids(GameState* state, float dt);
static void update_laser(GameState* state, float dt);
static void handle_spawning(GameState* state, float dt);
static void handle_collisions(GameState* state);
static void draw_game_3d(const GameState* state, JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis);
static void world_to_screen(float wx, float wy, uint32_t* sx, uint32_t* sy);
static void draw_filled_rect(JaDraw<WIDTH, HEIGHT>& canvas, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool white);


static float rand_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

SpaceGame3dApplet::SpaceGame3dApplet() { }

static void drawPixel(JaDraw<WIDTH, HEIGHT>& canvas, uint32_t x, uint32_t y, bool white)
{
    canvas.drawPixel(x, y, white ? Colors::White : Colors::Black);
}

static void clear_canvas(JaDraw<WIDTH, HEIGHT>& canvas)
{
    canvas.clear(0);
}

static void init_game(GameState* state) {
    //memset(state, 0, sizeof(GameState));

    // Init player
    state->player.pos.x = 0.0f;
    state->player.pos.y = PLAYER_Y_POS;
    state->player.vel = 0;
    state->player.targetVel = 0;
    state->player.hp = PLAYER_INITIAL_HP;
    state->player.laserChargeTime = 0.0f;

    // Init timers and flags
    state->asteroidSpawnTimer = ASTEROID_SPAWN_INTERVAL;
    state->gameOver = false;
    state->fireBtnWasPressed = false;
    state->score = 0;
    state->nextTimeGoalMs = 1000;
    state->asteroidBaseSpeed = 0.0f;
    
    // Seed random number generator. In a real embedded system without a time source,
    // you might use an unconnected ADC pin or some other source of entropy.
    // For now, this is fine but will produce the same sequence on each run.
    srand(12345);
}

void SpaceGame3dApplet::setup() { }

void SpaceGame3dApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    static unsigned long millis = 0;
    millis += (unsigned long)(dt * 1000.0f);
    gameInputData.fireBtnPressed = inputs.pressed;
    gameInputData.stickX = (float)inputs.rotation / 100.0f;
    GameInputData* gameInputs = &gameInputData;
    canvas.clear(0);
    canvas.drawPixel(0, 0, 0xFFFFFFFF);
    static GameState state;
    static bool initialized = false;

    // Initialize the game state on the first call
    if (!initialized) {
        millis = 0;
        init_game(&state);
        initialized = true;
    }

    if (!state.gameOver) {
        // --- Update Game Logic ---
        update_player(&state, dt, gameInputs);
        update_bullets(&state, dt);
        update_asteroids(&state, dt);
        update_laser(&state, dt);
        
        // --- Handle Interactions ---
        handle_collisions(&state);
        handle_spawning(&state, dt);

        if (millis >= state.nextTimeGoalMs)
        {
            state.score += 1;
            state.nextTimeGoalMs += 500;
            state.asteroidBaseSpeed += ASTEROID_SPEED_INCREASE;
        }

        // Check for game over condition
        if (state.player.hp <= 0) {
            state.gameOver = true;
        }
    } else {
        // If game is over, maybe listen for a reset button
        if (gameInputs->fireBtnPressed && !state.fireBtnWasPressed) {
            millis = 0;
            init_game(&state); // Restart the game
        }
        state.fireBtnWasPressed = gameInputs->fireBtnPressed;
    }


    // --- Render the Game State to the Canvas ---
    draw_game_3d(&state, canvas, millis);
}

const char* SpaceGame3dApplet::getName() const {
    return "Space game";
}

static void fire_bullet(GameState* state) {
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (!state->bullets[i].active) {
            state->bullets[i].active = true;
            state->bullets[i].pos = state->player.pos;
            state->bullets[i].pos.y += PLAYER_HEIGHT / 2.0f; // Fire from top of ship
            state->bullets[i].vel.x = 0;
            state->bullets[i].vel.y = BULLET_SPEED;
            return; // Exit after firing one bullet
        }
    }
}

static void fire_laser(GameState* state) {
    if (state->laser.active) return; // Only one laser at a time

    state->laser.active = true;
    state->laser.x_pos = state->player.pos.x;
    state->laser.duration = LASER_DURATION;
    
    // Power is proportional to charge time, capped at max
    float charge_ratio = state->player.laserChargeTime / LASER_MAX_CHARGE_TIME;
    if (charge_ratio > 1.0f) charge_ratio = 1.0f;
    state->laser.power = charge_ratio;
}

static void update_player(GameState* state, float dt, const GameInputData* inputs) {
    // Movement (smooth with acceleration)
    state->player.targetVel = inputs->stickX * PLAYER_SPEED * dt;
    // Smoothly interpolate velocity toward targetVel
    const float SMOOTHING = 0.05f; // Lower = smoother
    state->player.vel += (state->player.targetVel - state->player.vel) * SMOOTHING;
    state->player.pos.x += state->player.vel;

    // Screen bounds check
    float half_width = PLAYER_WIDTH / 2.0f;
    const float bounds = 0.6f;
    if (state->player.pos.x < -bounds + half_width) {
        state->player.pos.x = -bounds + half_width;
    }
    if (state->player.pos.x > bounds - half_width) {
        state->player.pos.x = bounds - half_width;
    }

    // Firing logic
    if (inputs->fireBtnPressed) {
        state->player.laserChargeTime += dt;
    } else {
        if (state->fireBtnWasPressed) { // This is a button release event
            if (state->player.laserChargeTime > LASER_CHARGE_TIME_FOR_BULLET) {
                fire_laser(state);
            } else {
                fire_bullet(state);
            }
        }
        state->player.laserChargeTime = 0.0f;
    }
    state->fireBtnWasPressed = inputs->fireBtnPressed;
}

static void update_bullets(GameState* state, float dt) {
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            state->bullets[i].pos.y += state->bullets[i].vel.y * dt;
            // Deactivate if it goes off-screen
            if (state->bullets[i].pos.y > 3.0f) {
                state->bullets[i].active = false;
            }
        }
    }
}

static void update_asteroids(GameState* state, float dt) {
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        if (state->asteroids[i].active) {
            state->asteroids[i].pos.x += state->asteroids[i].vel.x * dt;
            state->asteroids[i].pos.y += state->asteroids[i].vel.y * dt;
            // Deactivate if it goes off-screen
            if (state->asteroids[i].pos.y < -1.1f) {
                state->asteroids[i].active = false;
            }
            state->asteroids[i].flashTimerMs -= (int)(dt * 1000);
            if (state->asteroids[i].flashTimerMs < 0)
            {
                state->asteroids[i].flashTimerMs = 0;
            }
        }
    }
}

static void update_laser(GameState* state, float dt) {
    if (state->laser.active) {
        state->laser.duration -= dt;
        if (state->laser.duration <= 0) {
            state->laser.active = false;
        }
    }
}


static void handle_spawning(GameState* state, float dt) {
    state->asteroidSpawnTimer -= dt;
    if (state->asteroidSpawnTimer <= 0) {
        state->asteroidSpawnTimer = ASTEROID_SPAWN_INTERVAL * rand_float(0.7f, 1.3f);
        
        // Find an inactive asteroid to spawn
        for (int i = 0; i < MAX_ASTEROIDS; ++i) {
            if (!state->asteroids[i].active) {
                state->asteroids[i].active = true;
                state->asteroids[i].pos.x = rand_float(-0.5f, 0.5f);
                state->asteroids[i].pos.y = 5.1f; // Spawn just above the screen
                state->asteroids[i].vel.x = rand_float(-0.1f, 0.1f);
                if ((state->asteroids[i].pos.x < 0 && state->asteroids[i].vel.x < 0) ||
                    state->asteroids[i].pos.x > 0 && state->asteroids[i].vel.x > 0)
                {
                    state->asteroids[i].vel.x *= -1.0f;
                }
                state->asteroids[i].vel.y = -rand_float(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED) - state->asteroidBaseSpeed;
                state->asteroids[i].size = rand_float(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE);
                state->asteroids[i].hp = state->asteroids[i].size * 250.0f; // Bigger asteroids have more HP
                return; // Spawn one per frame
            }
        }
    }
}

static void handle_collisions(GameState* state) {
    // --- Bullets vs Asteroids ---
    for (int b = 0; b < MAX_BULLETS; ++b) {
        if (!state->bullets[b].active) continue;
        
        for (int a = 0; a < MAX_ASTEROIDS; ++a) {
            if (!state->asteroids[a].active) continue;

            // Simple AABB collision check
            float b_half_w = BULLET_WIDTH / 2.0f;
            float b_half_h = BULLET_HEIGHT / 2.0f;
            float a_half_s = state->asteroids[a].size / 2.0f;

            if (fabsf(state->bullets[b].pos.x - state->asteroids[a].pos.x) < (b_half_w + a_half_s) &&
                fabsf(state->bullets[b].pos.y - state->asteroids[a].pos.y) < (b_half_h + a_half_s))
            {
                state->asteroids[a].hp -= BULLET_DAMAGE;
                state->asteroids[a].flashTimerMs = 150;
                state->bullets[b].active = false; // Bullet is used up
                if (state->asteroids[a].hp <= 0) {
                    state->asteroids[a].active = false;
                    state->score += 120;
                }
                break; // A bullet can only hit one asteroid
            }
        }
    }

    // --- Laser vs Asteroids ---
    if (state->laser.active) {
        for (int a = 0; a < MAX_ASTEROIDS; ++a) {
            if (!state->asteroids[a].active) continue;

            float l_half_w = LASER_WIDTH / 2.0f;
            float a_half_s = state->asteroids[a].size / 2.0f;

            if (fabsf(state->laser.x_pos - state->asteroids[a].pos.x) < (l_half_w + a_half_s)) {
                // Laser does damage over time, scaled by dt
                state->asteroids[a].hp -= LASER_BASE_DAMAGE * state->laser.power * (1.0f / 60.0f); // Assuming 60fps for damage scaling
                 if (state->asteroids[a].hp <= 0) {
                    state->asteroids[a].active = false;
                }
            }
        }
    }

    // --- Asteroids vs Player ---
    for (int a = 0; a < MAX_ASTEROIDS; ++a) {
        if (!state->asteroids[a].active) continue;
        
        float p_half_w = PLAYER_WIDTH / 2.0f;
        float p_half_h = PLAYER_HEIGHT / 2.0f;
        float a_half_s = state->asteroids[a].size / 2.0f;

        if (fabsf(state->player.pos.x - state->asteroids[a].pos.x) < (p_half_w + a_half_s) &&
            fabsf(state->player.pos.y - state->asteroids[a].pos.y) < (p_half_h + a_half_s))
        {
            state->player.hp -= state->asteroids[a].size * 100.0f; // Damage based on size
            state->asteroids[a].active = false; // Asteroid is destroyed
        }
    }
}

////////////////////////// drawing ///////////////////////////////////////////////////////////////////

const uint32_t BLUE_NOISE[32][32] = {
  {0xe5, 0x91, 0xf4, 0x9e, 0xc0, 0xf9, 0x3d, 0xdc, 0xa6, 0x2e, 0x1f, 0xc2, 0xf2, 0x15, 0xcd, 0x27, 0x6e, 0x59, 0x97, 0x7a, 0x20, 0x70, 0x07, 0x7f, 0xa8, 0x3e, 0x99, 0xff, 0x5c, 0xbf, 0x09, 0x6c },
  {0x2d, 0xba, 0x48, 0x08, 0xcd, 0x7c, 0x1c, 0xb3, 0x6d, 0xfe, 0x9c, 0x4b, 0x79, 0x68, 0x91, 0x3e, 0xf1, 0xb1, 0x47, 0xfc, 0xc5, 0xe3, 0x5d, 0x49, 0xda, 0x10, 0xcb, 0x26, 0x36, 0x90, 0xed, 0x41 },
  {0xa2, 0xd4, 0x23, 0x89, 0x68, 0x31, 0x51, 0x8f, 0x0e, 0x5a, 0x86, 0xe3, 0xb5, 0x30, 0xd9, 0x07, 0x82, 0xbe, 0x2e, 0x0d, 0xa1, 0x3a, 0xb2, 0x94, 0xf0, 0x55, 0x68, 0xe2, 0xaf, 0x73, 0xdb, 0x11 },
  {0x39, 0x77, 0x5d, 0xef, 0xa8, 0xe5, 0xbb, 0xd6, 0xee, 0x39, 0xc9, 0x01, 0x41, 0xa3, 0xf8, 0x62, 0x9c, 0x14, 0xdb, 0x67, 0x52, 0x83, 0x25, 0xc3, 0x19, 0x89, 0xa3, 0x03, 0x4b, 0x1c, 0xa0, 0x57 },
  
  {0x00, 0xb1, 0xdd, 0x14, 0x40, 0x73, 0x27, 0x7f, 0x18, 0xaa, 0x71, 0x25, 0xd5, 0x58, 0x1c, 0xc3, 0x50, 0xe6, 0x76, 0xaa, 0xcf, 0x01, 0xf8, 0x72, 0x35, 0xd3, 0x7b, 0xbe, 0xf7, 0x87, 0x2d, 0xba },
  {0x99, 0xc8, 0x4f, 0x90, 0xc2, 0x04, 0xa1, 0x60, 0x46, 0xbf, 0xe8, 0x97, 0x7c, 0xb0, 0x8b, 0x70, 0x34, 0x23, 0x8f, 0x40, 0xea, 0x98, 0xb7, 0x60, 0x45, 0xe9, 0x2b, 0x5d, 0x3e, 0xcd, 0x66, 0xd6 },
  {0x43, 0x1e, 0xf2, 0x2d, 0x5a, 0xfc, 0xd2, 0x94, 0xf4, 0x2e, 0x66, 0x13, 0x4c, 0xed, 0x09, 0xce, 0xfe, 0xb4, 0xc8, 0x5b, 0x1c, 0x30, 0xdf, 0x11, 0xab, 0x91, 0x0b, 0xb1, 0xe7, 0x97, 0x0d, 0x50 },
  {0x60, 0x80, 0xa5, 0x6d, 0xb5, 0x84, 0x3a, 0x0d, 0xca, 0x53, 0x86, 0xb8, 0xd8, 0x2a, 0x3e, 0xa4, 0x4a, 0x81, 0x0c, 0xf4, 0x6e, 0x89, 0x4c, 0x7d, 0xca, 0xfa, 0x53, 0x70, 0x17, 0x7d, 0xa8, 0x23 },
  
  {0xd5, 0x36, 0x07, 0xe7, 0x1b, 0x4a, 0xe1, 0x23, 0x75, 0xa7, 0x05, 0xfa, 0x9d, 0x60, 0x78, 0xde, 0x17, 0x63, 0x99, 0x39, 0xbd, 0xa4, 0xd5, 0x21, 0x66, 0x9e, 0x25, 0xda, 0xc6, 0x32, 0xee, 0xdd },
  {0x4d, 0x97, 0xc3, 0x78, 0xce, 0x9e, 0x67, 0xb0, 0x8c, 0xe5, 0x43, 0x35, 0xc2, 0x1e, 0x92, 0xbb, 0xee, 0x2e, 0xac, 0xe2, 0x08, 0x57, 0xed, 0x40, 0x04, 0xbf, 0x38, 0x8d, 0x49, 0xb4, 0x6c, 0x58 },
  {0x18, 0xb3, 0xf7, 0x57, 0x32, 0x11, 0xbe, 0xf1, 0x59, 0x19, 0xd1, 0x6e, 0x80, 0xf2, 0x54, 0x04, 0x6c, 0xd1, 0x50, 0x77, 0xc8, 0x29, 0x95, 0xb5, 0x73, 0xe1, 0x81, 0xf0, 0x61, 0x01, 0x92, 0x1a },
  {0xde, 0x6a, 0x42, 0x25, 0x92, 0xdb, 0x7e, 0x3d, 0x2a, 0x97, 0xb4, 0x4d, 0x0d, 0xab, 0xc9, 0x44, 0x84, 0x20, 0x8e, 0xf8, 0x17, 0x6a, 0x85, 0xfd, 0x14, 0x59, 0xac, 0x10, 0xa0, 0xd6, 0xfe, 0x3c },
  
  {0xa7, 0x0d, 0x81, 0xe8, 0xab, 0x63, 0x4e, 0x01, 0xc4, 0xff, 0x5e, 0xdf, 0x27, 0xe7, 0x32, 0x9b, 0xdb, 0xb7, 0x3c, 0x9f, 0x48, 0xda, 0x36, 0xa5, 0x4b, 0xd2, 0x2c, 0x43, 0xc1, 0x21, 0x73, 0x85 },
  {0x38, 0x8b, 0xd1, 0xbb, 0x0a, 0xf6, 0x9d, 0xd3, 0x70, 0x84, 0x10, 0xa2, 0x8b, 0x74, 0x64, 0xfc, 0x13, 0x5c, 0xe5, 0x00, 0xad, 0x5e, 0xc4, 0x1e, 0xe8, 0x8a, 0x69, 0xf6, 0x7c, 0x52, 0x30, 0xca },
  {0xc2, 0x20, 0x5a, 0x4a, 0x72, 0x38, 0x21, 0xe5, 0xad, 0x31, 0x46, 0xce, 0xbd, 0x3e, 0x09, 0xa8, 0x7b, 0x2d, 0xc1, 0x72, 0x83, 0xf2, 0x0d, 0x7a, 0x99, 0xcb, 0x08, 0xb4, 0x91, 0xda, 0xae, 0x14 },
  {0xfe, 0x9c, 0xed, 0x16, 0x90, 0xcb, 0x82, 0x57, 0x14, 0x94, 0x69, 0xf6, 0x1f, 0x58, 0xd6, 0xc5, 0x8e, 0x50, 0xf6, 0x21, 0xd0, 0x2e, 0xb7, 0x53, 0x3e, 0x27, 0x5b, 0xe3, 0x19, 0x3f, 0x66, 0xf8 },
  
  {0x67, 0xad, 0x7b, 0xdf, 0x2d, 0xa4, 0xbe, 0x42, 0xef, 0xb8, 0xde, 0x04, 0x7c, 0x98, 0xf1, 0x47, 0x1b, 0xb3, 0x68, 0x9d, 0x43, 0x92, 0x6c, 0xed, 0xa1, 0xbe, 0x72, 0x34, 0xa7, 0xec, 0x03, 0x88 },
  {0x48, 0x01, 0x3f, 0xb5, 0x6a, 0xfb, 0x06, 0x64, 0x79, 0x25, 0x51, 0xa7, 0x36, 0xb5, 0x2a, 0x6e, 0xeb, 0x39, 0xd9, 0x12, 0x59, 0xe4, 0x08, 0xd4, 0x18, 0xfa, 0x86, 0xd0, 0x50, 0x78, 0x9f, 0xcb },
  {0x8c, 0xc5, 0x26, 0xd3, 0x4e, 0x12, 0xe3, 0x8d, 0xd8, 0x3b, 0xc5, 0x87, 0x61, 0xe4, 0x0f, 0x83, 0x95, 0x05, 0xa5, 0xc6, 0x87, 0xb0, 0x36, 0x7e, 0x62, 0x48, 0x05, 0x95, 0x21, 0xbf, 0x2d, 0x5f },
  {0x57, 0xf8, 0x99, 0x85, 0x5d, 0x9e, 0x33, 0xad, 0x1a, 0x99, 0xfd, 0x14, 0xd4, 0x49, 0xa2, 0xca, 0x5e, 0xe1, 0x76, 0x2a, 0xff, 0x4b, 0xc2, 0x23, 0xaa, 0xdd, 0xb5, 0xe8, 0x6b, 0xfd, 0x41, 0xad },
  
  {0xa3, 0x35, 0x6d, 0x1c, 0xed, 0xc6, 0x75, 0x49, 0xcd, 0x5a, 0x6d, 0x2f, 0xbc, 0x76, 0xf9, 0x25, 0x51, 0xb9, 0x41, 0x65, 0x1c, 0x9b, 0x71, 0xf4, 0x8d, 0x3d, 0x2f, 0x5a, 0x13, 0x83, 0xd8, 0x1a },
  {0xe0, 0xbc, 0x06, 0xdb, 0x3d, 0xb8, 0x23, 0xf5, 0x82, 0x00, 0xb2, 0xe6, 0x8f, 0x08, 0x3c, 0xae, 0x18, 0xf3, 0x80, 0xcc, 0xe7, 0x01, 0x56, 0xce, 0x0e, 0x9d, 0x7a, 0xc8, 0xa8, 0x4b, 0x97, 0xb9 },
  {0x12, 0x4c, 0xab, 0x7a, 0x8c, 0x55, 0x0b, 0x66, 0xde, 0xa0, 0x43, 0x21, 0x55, 0x9d, 0x66, 0xd9, 0x8b, 0x34, 0x0f, 0xab, 0x91, 0xbb, 0x2d, 0xeb, 0x69, 0x4f, 0xd7, 0xf3, 0x09, 0x2a, 0xe6, 0x65 },
  {0x83, 0xe9, 0xc5, 0x2d, 0xf8, 0xa5, 0xd5, 0x92, 0x2c, 0xc0, 0x74, 0xf3, 0xcc, 0x80, 0xeb, 0xc2, 0x71, 0x99, 0xd5, 0x48, 0x5d, 0x38, 0x86, 0xa7, 0x15, 0xb8, 0x22, 0x8c, 0x6f, 0xc1, 0x38, 0x56 },
  
  {0x9a, 0x59, 0x21, 0x67, 0x47, 0x15, 0xb3, 0x3a, 0xef, 0x51, 0x0d, 0xa9, 0x34, 0x12, 0x46, 0x29, 0x03, 0x55, 0xe4, 0x21, 0x79, 0xfa, 0xda, 0x46, 0x76, 0xe3, 0x3a, 0x5e, 0xa1, 0xd4, 0x7c, 0xb1 },
  {0x73, 0x09, 0x90, 0xce, 0xe3, 0x86, 0x71, 0x5d, 0x1c, 0xcf, 0x8b, 0x63, 0xb9, 0xdf, 0x5d, 0xa7, 0xfd, 0xb6, 0x6a, 0xc0, 0xa0, 0x07, 0x64, 0x26, 0xc5, 0x95, 0x03, 0xfe, 0x44, 0x19, 0xf0, 0x92 },
  {0xc1, 0xb1, 0xfc, 0xa1, 0x35, 0x02, 0xc8, 0xe7, 0x9c, 0x7b, 0x40, 0xf7, 0x97, 0x1e, 0x89, 0xce, 0x7b, 0x3e, 0x2f, 0xf0, 0x14, 0xcf, 0xb4, 0x89, 0xed, 0x50, 0xad, 0x80, 0xbb, 0x0c, 0x6a, 0x2f },
  {0x2a, 0x3e, 0x51, 0x18, 0x78, 0xb9, 0x49, 0x27, 0xab, 0x06, 0xd9, 0x2b, 0x71, 0x51, 0xed, 0x0d, 0x9e, 0x18, 0x91, 0x82, 0x53, 0x41, 0x98, 0x1b, 0x33, 0x6e, 0xcd, 0x2a, 0xde, 0x55, 0xa7, 0xea },
  
  {0x11, 0xdd, 0x82, 0x62, 0xd7, 0xf3, 0x90, 0x6b, 0xff, 0xbe, 0x4d, 0x15, 0xc5, 0xad, 0x33, 0x65, 0xdd, 0x4e, 0xca, 0xaf, 0xe9, 0x72, 0xdf, 0x5b, 0xf9, 0x0f, 0x9d, 0x61, 0x8f, 0xc7, 0x78, 0x1c },
  {0xf4, 0x96, 0xc3, 0xaa, 0x21, 0x56, 0x3d, 0x10, 0x84, 0x5b, 0xe2, 0xa0, 0x83, 0xd3, 0x43, 0xb7, 0x74, 0xf4, 0x22, 0x63, 0x0b, 0x2b, 0xc2, 0xa7, 0x7e, 0xd5, 0x40, 0x1f, 0xf6, 0x34, 0x4d, 0xb0 },
  {0xb5, 0x44, 0x06, 0x31, 0xe9, 0x9e, 0xb2, 0xd4, 0x2f, 0x94, 0x38, 0x67, 0x07, 0xfa, 0x24, 0x8b, 0x02, 0xbd, 0x37, 0xa3, 0xd4, 0x8d, 0x3b, 0x00, 0xbb, 0x4b, 0xe7, 0xb5, 0x05, 0x83, 0xef, 0x0e },
  {0x8a, 0x2d, 0x6e, 0x9b, 0x50, 0xba, 0x32, 0x43, 0x76, 0xc2, 0x66, 0x49, 0x03, 0xeb, 0x5a, 0x0c, 0x6c, 0xae, 0x18, 0x2f, 0x91, 0x75, 0x5c, 0x83, 0x08, 0x54, 0x77, 0x3a, 0x82, 0xd4, 0x9f, 0x1f },
};


void fillDitheredTriangle(JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis,
     const Vec2i* v1, const Vec2i* v2, const Vec2i* v3, float brightness)
{
    // --- Initial Setup ---

    // Sort vertices by Y coordinate (p0.y <= p1.y <= p2.y) using pointers to avoid copies.
    const Vec2i* p[3] = {v1, v2, v3};
    if (p[0]->y > p[1]->y) std::swap(p[0], p[1]);
    if (p[1]->y > p[2]->y) std::swap(p[1], p[2]);
    if (p[0]->y > p[1]->y) std::swap(p[0], p[1]);

    const Vec2i& p0 = *p[0];
    const Vec2i& p1 = *p[1];
    const Vec2i& p2 = *p[2];

    // Early exit if the triangle is completely outside the screen
    if (p2.y < 0 || p0.y >= canvas.height) return;
    if (std::max({p0.x, p1.x, p2.x}) < 0 || std::min({p0.x, p1.x, p2.x}) >= canvas.width) return;

    // --- Fixed-Point Setup ---

    // This is the ONLY floating-point operation, performed once per triangle.
    // It converts the 0.0-1.0 brightness into a 0-255 integer threshold.
    int32_t brightness_level = static_cast<int32_t>(brightness * 255.0f);

    // use the noise itself as a source of random position to offset it by
    //uint32_t offsetX = (millis / 3) & 63;
    //uint32_t offsetY = (millis / 11) & 63;
    uint32_t offsetX = BLUE_NOISE[(millis / 15) & 31][(millis / 15) & 31];
    uint32_t offsetY = BLUE_NOISE[((millis / 15) + 1) & 31][((millis / 15) + 1) & 31];

    // We'll use 16.16 fixed-point precision. (16 bits for integer, 16 for fraction)
    const int FIXED_POINT_SHIFT = 16;

    // --- Rasterize Top Half of Triangle ---
    int32_t height01 = p1.y - p0.y;
    if (height01 > 0) {
        int32_t height02 = p2.y - p0.y;
        // Calculate slopes (dx/dy) using fixed-point math
        int32_t slope1 = ((p1.x - p0.x) << FIXED_POINT_SHIFT) / height01;
        int32_t slope2 = ((p2.x - p0.x) << FIXED_POINT_SHIFT) / height02;

        int32_t x1 = p0.x << FIXED_POINT_SHIFT;
        int32_t x2 = p0.x << FIXED_POINT_SHIFT;

        for (int y = p0.y; y < p1.y; ++y) {
            // Only draw scanlines that are on-screen
            if (y >= 0 && y < canvas.height) {
                int startX = x1 >> FIXED_POINT_SHIFT;
                int endX   = x2 >> FIXED_POINT_SHIFT;
                if (startX > endX) std::swap(startX, endX);
                
                // Clip and draw the horizontal line
                for (int x = std::max(0, startX); x < std::min(canvas.width, endX); ++x) {
                    uint32_t threshold = BLUE_NOISE[(y + offsetY) & 31][(x + offsetX) & 31];
                    if (brightness_level > threshold) {
                        canvas.drawPixel(x, y, Colors::White);
                    } else {
                        canvas.drawPixel(x, y, Colors::Black);
                    }
                }
            }
            // Increment x positions by their slopes for the next line
            x1 += slope1;
            x2 += slope2;
        }
    }

    // --- Rasterize Bottom Half of Triangle ---
    int32_t height12 = p2.y - p1.y;
    if (height12 > 0) {
        int32_t height02 = p2.y - p0.y;
        // Calculate slopes. The long edge's slope (p0-p2) is the same.
        int32_t slope1 = ((p2.x - p1.x) << FIXED_POINT_SHIFT) / height12;
        int32_t slope2 = ((p2.x - p0.x) << FIXED_POINT_SHIFT) / height02;

        // Start x positions at the middle vertex and the corresponding point on the long edge
        int32_t x1 = p1.x << FIXED_POINT_SHIFT;
        int32_t x2 = (p0.x << FIXED_POINT_SHIFT) + slope2 * height01;

        for (int y = p1.y; y <= p2.y; ++y) {
            if (y >= 0 && y < canvas.height) {
                int startX = x1 >> FIXED_POINT_SHIFT;
                int endX   = x2 >> FIXED_POINT_SHIFT;
                if (startX > endX) std::swap(startX, endX);

                for (int x = std::max(0, startX); x < std::min(canvas.width, endX); ++x) {
                    uint32_t threshold = BLUE_NOISE[(y + offsetY) & 31][(x + offsetX) & 31];
                    if (brightness_level > threshold) {
                        canvas.drawPixel(x, y, Colors::White);
                    } else {
                        canvas.drawPixel(x, y, Colors::Black);
                    }
                }
            }
            x1 += slope1;
            x2 += slope2;
        }
    }
}
static void draw_3d_model(JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis, const Mat4f* vp_matrix,
                          const Vec3f* vertices, const int* indices, int num_indices,
                          Vec3f position, float rotation_x_rad, float rotation_y_rad, float rotation_z_rad, float scale,
                          const Vec3f* world_light_dir)
{
    // 1. Create Model and MVP Matrices (same as before)
    Mat4f scale_mat = matrix_scale((Vec3f){scale, scale, scale});
    Mat4f rot_mat = matrix_multiply(
        matrix_multiply( // order matters
            matrix_rotate_y(rotation_y_rad),
            matrix_rotate_x(rotation_x_rad)),
        matrix_rotate_z(rotation_z_rad)
    );
    Mat4f trans_mat = matrix_translate(position);
    Mat4f model_matrix = matrix_multiply(trans_mat, matrix_multiply(rot_mat, scale_mat));
    Mat4f mvp_matrix = matrix_multiply(*vp_matrix, model_matrix);

    const float AMBIENT_LIGHT = 0.0f;
    const float DIFFUSE_STRENGTH = 1.5f;

    // 2. Process and draw each triangle
    for (int i = 0; i < num_indices; i += 3) {
        const Vec3f* v_model[3];
        v_model[0] = &vertices[indices[i]];
        v_model[1] = &vertices[indices[i+1]];
        v_model[2] = &vertices[indices[i+2]];

        // --- Projection ---
        // Project all three vertices first.
        Vec2i v_screen[3];
        bool p1_visible = project_vertex(v_model[0], &mvp_matrix, &v_screen[0], WIDTH, HEIGHT);
        bool p2_visible = project_vertex(v_model[1], &mvp_matrix, &v_screen[1], WIDTH, HEIGHT);
        bool p3_visible = project_vertex(v_model[2], &mvp_matrix, &v_screen[2], WIDTH, HEIGHT);

        // If all three vertices are behind the camera, skip the whole triangle.
        // This is a simple but effective form of clipping.
        if (!p1_visible && !p2_visible && !p3_visible) {
            continue;
        }
        // A more robust check would be if ANY vertex is valid, but this is often good enough.
        // if (!p1_visible && !p2_visible && !p3_visible) continue;
        // Or even better, if all are invalid
        // if (!(p1_visible || p2_visible || p3_visible)) continue;


        // --- Back-face Culling ---
        // Use screen-space winding order. This is fast and effective.
        int cross_product_z = (v_screen[1].x - v_screen[0].x) * (v_screen[2].y - v_screen[0].y) -
                              (v_screen[1].y - v_screen[0].y) * (v_screen[2].x - v_screen[0].x);

        if (cross_product_z < 0) { // If triangle is facing the camera
            // --- Lighting Calculation (in World Space) ---
            // We do this *after* culling, because it's wasted computation on a triangle we won't draw.
            Vec3f v_world[3];
            v_world[0] = transform_vertex(v_model[0], &model_matrix);
            v_world[1] = transform_vertex(v_model[1], &model_matrix);
            v_world[2] = transform_vertex(v_model[2], &model_matrix);

            Vec3f edge1 = vec3_subtract(v_world[1], v_world[0]);
            Vec3f edge2 = vec3_subtract(v_world[2], v_world[0]);
            Vec3f face_normal = vec3_normalize(vec3_cross(edge1, edge2));

            float diffuse_intensity = vec3_dot(face_normal, *world_light_dir);

            // Final brightness calculation
            float brightness = AMBIENT_LIGHT;
            if (diffuse_intensity > 0) {
                brightness += diffuse_intensity * DIFFUSE_STRENGTH;
            }
            if (brightness > 1.0f) brightness = 1.0f;

            // --- Drawing ---
            // The rasterizer will handle clipping the triangle to the screen bounds.
            fillDitheredTriangle(canvas, millis, &v_screen[0], &v_screen[1], &v_screen[2], brightness);
        }
    }
}
/**
 * @brief Draws a point in 3D space with a specified size that respects perspective.
 *
 * @param canvas The canvas to draw on.
 * @param vp_matrix The combined View-Projection matrix.
 * @param world_position The position of the point in world space.
 * @param size The desired radius of the point in world space units.
 */
static void draw_3d_point(JaDraw<WIDTH, HEIGHT>& canvas, const Mat4f* vp_matrix,
                          Vec3f world_position, float size)
{
    // A point needs a center and an edge to define its size in the world.
    // We create a second point offset by `size` along the Y-axis.
    Vec3f center_world = world_position;
    Vec3f edge_world   = {world_position.x, world_position.y + size, world_position.z};

    // 1. Project the center of the point to find its screen position.
    Vec2i screen_pos;
    if (!project_vertex(&center_world, vp_matrix, &screen_pos, WIDTH, HEIGHT)) {
        // If the center isn't on screen (or is behind the camera), we can't draw it.
        return;
    }

    // 2. Project the "edge" point to determine the size in pixels.
    Vec2i screen_edge;
    if (!project_vertex(&edge_world, vp_matrix, &screen_edge, WIDTH, HEIGHT)) {
        // If the edge point is not projectable (e.g. behind camera), we can't
        // reliably calculate a size, so we can either draw a minimal 1x1 point
        // or just skip it. Skipping is safer to avoid visual artifacts.
        // For a small point, it's often fine to just draw a single pixel.
        //canvas.setPixel(screen_pos.x, screen_pos.y);
        return;
    }

    // 3. Calculate the radius in pixels using the distance between the projected points.
    // We use the distance formula, but since our offset was only on one axis (Y),
    // we can simplify it. Using the full distance formula would also work and be
    // more robust if the offset was in a more complex direction.
    float dx = screen_pos.x - screen_edge.x;
    float dy = screen_pos.y - screen_edge.y;
    float screen_radius = sqrtf(dx * dx + dy * dy);

    // 4. Calculate the final width/height of the rectangle (diameter).
    // We ensure it's at least 1 pixel wide so it doesn't disappear when very far away.
    int w = (int)(screen_radius * 2.0f);
    if (w < 1) {
        w = 1;
    }

    // 5. Draw the filled rectangle centered on the projected point's position.
    // The `true` likely corresponds to the color/fill state.
    draw_filled_rect(canvas, screen_pos.x - (w / 2), screen_pos.y - (w / 2), w, w, true);
}

// Helper to convert world coordinates [-1, 1] to screen pixels [0, Res-1]
static void world_to_screen(float wx, float wy, uint32_t* sx, uint32_t* sy) {
    *sx = (uint32_t)((wx + 1.0f) * 0.5f * WIDTH);
    // Y is inverted: world +1 is top, screen 0 is top
    *sy = (uint32_t)((-wy + 1.0f) * 0.5f * HEIGHT);
}

// Helper to draw a simple filled rectangle
static void draw_filled_rect(JaDraw<WIDTH, HEIGHT>& canvas, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool white) {
    // Clamp width and height to avoid overflow and infinite loops
    if (x >= WIDTH || y >= HEIGHT) return;
    uint32_t max_w = (x + w > WIDTH) ? (WIDTH - x) : w;
    uint32_t max_h = (y + h > HEIGHT) ? (HEIGHT - y) : h;
    for (uint32_t i = x; i < (uint32_t)(x + max_w); ++i) {
        for (uint32_t j = y; j < (uint32_t)(y + max_h); ++j) {
            drawPixel(canvas, i, j, white);
        }
    }
}

unsigned int prng_seed = 59345;

unsigned int simple_prng(void) {
    prng_seed ^= (prng_seed << 13);
    prng_seed ^= (prng_seed >> 17);
    prng_seed ^= (prng_seed << 5);
    return prng_seed;
}

// Function to get a random float in a range (0.0 to 1.0)
float prng_float(void) {
    // Generate a random 32-bit integer and normalize to [0, 1]
    return (float)simple_prng() / 0xFFFFFFFF;
}

static void draw_starfield(JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis) {
    const int NUM_STARS = 64; // Increased for a better effect
    const float ZOOM_SPEED = 0.05f; // Adjusted for a more pleasant speed
    const float MAX_DEPTH = 100.0f;
    const float CENTER_X = WIDTH / 2.0f;
    const float CENTER_Y = HEIGHT / 2.0f;

    for (int star_id = 0; star_id < NUM_STARS; star_id++) {
        // --- Star Property Generation (THE FIX) ---
        
        // 1. Set a DETERMINISTIC seed based *only* on the star's unique ID.
        // This is the key to the fix. Every star now has its own set of
        // predictable "random" properties that will be the same on every frame.
        prng_seed = (star_id + 1) * 19937;


        // 2. Generate the star's persistent properties from this seed.
        // These values are now constant for a given star_id.
        float star_x_3d = (prng_float() * 2.0f - 1.0f) * MAX_DEPTH;
        float star_y_3d = (prng_float() * 2.0f - 1.0f) * MAX_DEPTH;
        float initial_z_offset = prng_float() * MAX_DEPTH; // Each star starts at a random depth

        // --- Animation ---
        
        // 3. Animate the Z position based on time. The initial offset and
        // the passage of time (`millis`) create the movement.
        // The modulo operator (%) wraps the star back to the farthest depth
        // once it passes the camera, creating an infinite tunnel effect.
        float current_z = fmodf(initial_z_offset - (float)millis * ZOOM_SPEED, MAX_DEPTH);
        
        // Ensure Z is always positive after the modulo operation
        if (current_z < 0) {
            current_z += MAX_DEPTH;
        }

        // --- 3D to 2D Projection (Perspective) ---
        
        // Avoid division by zero and make stars appear at a distance
        if (current_z > 1.0f) { 
            // Perspective projection: screen_coord = (3d_coord * focal_length) / z_depth
            // A "focal length" is implied by how we scale the result.
            // Here we scale by the screen center to spread the stars out.
            float projected_x = star_x_3d / current_z;
            float projected_y = star_y_3d / current_z;

            // Map from normalized coordinates to screen coordinates
            int screen_x = (int)(projected_x * CENTER_X + CENTER_X);
            int screen_y = (int)(projected_y * CENTER_Y + CENTER_Y);

            // --- Drawing ---
            
            // Check if the star is within the screen boundaries
            if (screen_x >= 0 && screen_x < WIDTH &&
                screen_y >= 0 && screen_y < HEIGHT) {
                
                // Closer stars (smaller Z) should be brighter.
                int brightness = (int)(255.0f * (1.0f - (current_z / MAX_DEPTH)));
                
                // Clamp brightness to a valid 0-255 range
                brightness = (brightness < 0) ? 0 : (brightness > 255) ? 255 : brightness;
                
                // Draw the star
                drawPixel(canvas, screen_x, screen_y, brightness);
            }
        }
    }
}


// --- Corrected draw_game_over function ---
void draw_game_over(JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis) {
    draw_starfield(canvas, millis);
    canvas.drawText("GAME OVER", 14, 25, 2, Colors::White, false);
    canvas.drawText("GAME OVER", 15, 25, 2, Colors::White, false);
}

/**
 * @brief Renders the 2D game state in a 3D world using the "On-the-Fly Interpretation" strategy.
 *
 * This function reads a pure 2D GameState and generates 3D visual data (like Y-coordinates
 * and rotations) temporarily for rendering purposes only. The core GameState is not modified
 * and contains no 3D data.
 *
 * @param state   A pointer to the const 2D game state.
 * @param canvas  Pointer to the canvas to draw on.
 * @param millis  Current timestamp in milliseconds for procedural effects like rotation.
 */
static void draw_game_3d(const GameState* state, JaDraw<WIDTH, HEIGHT>& canvas, unsigned long millis) {
    clear_canvas(canvas);
    draw_starfield(canvas, millis);

    if (state->gameOver) {
        draw_game_over(canvas, millis);
        static char scoreReport[16];
        snprintf(scoreReport, sizeof(scoreReport), "%05d", state->score);
        canvas.drawText(scoreReport, 2, 2, 1, Colors::White, false);
        return;
    }

    // --- Setup Camera and Projection (once per frame) ---
    Vec3f camera_pos = {0.0f, -0.20f, -1.3f};
    Vec3f camera_target = {0.0f, 0.0f, 0.0f};
    Vec3f up_vector = {0.0f, -1.0f, 0.0f};
    Mat4f view_matrix = matrix_look_at(camera_pos, camera_target, up_vector);
    
    float fov_rad = 70.0f * (3.14159f / 180.0f);
    float aspect_ratio = (float)WIDTH / (float)HEIGHT;
    Mat4f proj_matrix = matrix_perspective(fov_rad, aspect_ratio, 0.1f, 100.0f);

    Mat4f vp_matrix = matrix_multiply(proj_matrix, view_matrix);
    //Vec3f sun_direction = vec3_normalize((Vec3f){0.5f, 0.8f, -0.3f});
    Vec3f sun_direction = vec3_normalize((Vec3f){0.7f, -0.7f, -0.3f});
    // --- Draw Player ---
    {
        Vec3f player_pos_3d = {state->player.pos.x, 0.0f, state->player.pos.y};
        if (state->player.laserChargeTime > LASER_CHARGE_TIME_FOR_BULLET) {
             // show pulsing 
             float charge_ratio = state->player.laserChargeTime / LASER_MAX_CHARGE_TIME;
             if (charge_ratio > 1.0f) { charge_ratio = 1.0f; }
             float pulseSize = 0.06f * charge_ratio + 0.02f * cos(millis * 0.05f);
             Vec3f pulsePos = {player_pos_3d.x, player_pos_3d.y, player_pos_3d.z + 0.2f};
             draw_3d_point(canvas, &vp_matrix, pulsePos, pulseSize);
        }   
        // The player ship can remain unrotated for a clean look.
        float player_rotation_y = 3.1f; 
        float player_scale = 0.25f;
        float tilt = state->player.vel * -35.0f;
        draw_3d_model(canvas, millis, &vp_matrix, SHIP_VERTICES, SHIP_INDICES, SHIP_NUM_INDICES,
                      player_pos_3d, 0, player_rotation_y, tilt, player_scale, &sun_direction);
    }

    // --- Draw Bullets ---
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            // ADAPTATION: Convert 2D bullet position to 3D world position.
            Vec3f bullet_pos_3d = {state->bullets[i].pos.x, 0.0f, state->bullets[i].pos.y};
            
            // Bullets are simple; no rotation needed.
            // draw_3d_model(canvas, millis, &vp_matrix, CUBE_VERTICES, CUBE_INDICES, CUBE_NUM_INDICES,
            //               bullet_pos_3d, 0, 0.0f, 0, 0.04f, &sun_direction);
            draw_3d_point(canvas, &vp_matrix, bullet_pos_3d, 0.03f);
        }
    }

    // --- Draw Asteroids ---
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        if (state->asteroids[i].active) {
            // ADAPTATION: Convert 2D asteroid position to 3D world position.
            Vec3f asteroid_pos_3d = {state->asteroids[i].pos.x, 0.0f, state->asteroids[i].pos.y};
            
            // GENERATION: Create a procedural rotation that doesn't exist in the game state.
            // This makes them spin visually without affecting gameplay. The offset `i * 0.77f`
            // ensures they don't all spin in perfect sync.
            float time_sec = millis / 1000.0f;
            float rotation_y = (time_sec * 1.2f) + (i * 0.77f);
            
            // Use the asteroid's 2D size directly for 3D scaling.
            float scale = state->asteroids[i].size * 1.0f; // Adjust scale factor as needed
            Vec3f flash_dir = vec3_normalize((Vec3f){0.0f, 0.0f, -1.0f});
            Vec3f* light_dir = state->asteroids[i].flashTimerMs > 0 ? &flash_dir : &sun_direction;
            draw_3d_model(canvas, millis, &vp_matrix, ASTEROID_VERTICES, ASTEROID_INDICES, ASTEROID_NUM_INDICES,
                          asteroid_pos_3d, rotation_y, rotation_y, 0, scale, light_dir);
        }
    }
    
    // --- Draw Laser ---
    if (state->laser.active) {
        // goes from 1.0 to 0.0 as the laser nears dissapearing time
        float laserLifetimeProportion = state->laser.duration / LASER_DURATION;
        for (int i = 0; i < 15; i++)
        {
            Vec3f laser_pos_3d = { state->laser.x_pos, 0.0f, i * 0.2f - (0.6f + 0.2f * cos(millis * 0.07f)) };
            float laserBaseSize = state->laser.power * 0.11f;
            draw_3d_point(canvas, &vp_matrix, laser_pos_3d, laserBaseSize * laserLifetimeProportion);
        }
    }
    
    // --- Draw UI (HP Bar) ---
    uint32_t hp_bar_width = (uint32_t)((state->player.hp / PLAYER_INITIAL_HP) * 29);
    draw_filled_rect(canvas, 2, 10, hp_bar_width, 1, true);
    static char scoreReport[16];
    snprintf(scoreReport, sizeof(scoreReport), "%05d", state->score);
    canvas.drawText(scoreReport, 2, 2, 1, Colors::White, false);
}
