#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"

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
#define BULLET_DAMAGE 10.0f
#define LASER_CHARGE_TIME_FOR_BULLET 0.2f
#define LASER_MAX_CHARGE_TIME 2.0f
#define LASER_WIDTH 0.04f
#define LASER_DURATION 0.15f
#define LASER_BASE_DAMAGE 50.0f

#define ASTEROID_SPAWN_INTERVAL 1.5f
#define ASTEROID_MIN_SPEED 0.2f
#define ASTEROID_MAX_SPEED 0.6f
#define ASTEROID_MIN_SIZE 0.08f
#define ASTEROID_MAX_SIZE 0.2f

typedef struct {
    float x, y;
} Vector2D;

typedef struct {
    Vector2D pos;
    float hp;
    float laserChargeTime;
} Player;

typedef struct {
    bool active;
    Vector2D pos;
    Vector2D vel;
} Bullet;

typedef struct {
    bool active;
    Vector2D pos;
    Vector2D vel;
    float size;
    float hp;
} Asteroid;

typedef struct {
    bool active;
    float x_pos; // Laser is a vertical beam
    float power;
    float duration;
} Laser;

typedef struct {
    Player player;
    Asteroid asteroids[MAX_ASTEROIDS];
    Bullet bullets[MAX_BULLETS];
    Laser laser;

    float asteroidSpawnTimer;
    bool fireBtnWasPressed;
    bool gameOver;

} GameState;

struct GameInputData {
    bool fireBtnPressed;
    float stickX; // -1.0 (left) to 1.0 (right)
};

static GameInputData gameInputData;

class SpaceGameApplet : public IApplet {
public:
    SpaceGameApplet();
    ~SpaceGameApplet() override = default;
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
static void draw_game(const GameState* state, JaDraw<WIDTH, HEIGHT>& canvas);
static void world_to_screen(float wx, float wy, uint8_t* sx, uint8_t* sy);
static void draw_filled_rect(JaDraw<WIDTH, HEIGHT>& canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool white);

static float rand_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

SpaceGameApplet::SpaceGameApplet() { }

static void drawPixel(JaDraw<WIDTH, HEIGHT>& canvas, uint8_t x, uint8_t y, bool white)
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
    state->player.hp = PLAYER_INITIAL_HP;
    state->player.laserChargeTime = 0.0f;

    // Init timers and flags
    state->asteroidSpawnTimer = ASTEROID_SPAWN_INTERVAL;
    state->gameOver = false;
    state->fireBtnWasPressed = false;
    
    // Seed random number generator. In a real embedded system without a time source,
    // you might use an unconnected ADC pin or some other source of entropy.
    // For now, this is fine but will produce the same sequence on each run.
    srand(12345);
}

void SpaceGameApplet::setup() { }

void SpaceGameApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    gameInputData.fireBtnPressed = inputs.pressed;
    gameInputData.stickX = (float)inputs.rotation / 100.0f;
    GameInputData* gameInputs = &gameInputData;
    canvas.clear(0);
    canvas.drawPixel(0, 0, 0xFFFFFFFF);
    static GameState state;
    static bool initialized = false;

    // Initialize the game state on the first call
    if (!initialized) {
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

        // Check for game over condition
        if (state.player.hp <= 0) {
            state.gameOver = true;
        }
    } else {
        // If game is over, maybe listen for a reset button
        if (gameInputs->fireBtnPressed && !state.fireBtnWasPressed) {
            init_game(&state); // Restart the game
        }
        state.fireBtnWasPressed = gameInputs->fireBtnPressed;
    }


    // --- Render the Game State to the Canvas ---
    draw_game(&state, canvas);
}

const char* SpaceGameApplet::getName() const {
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
    // Movement
    state->player.pos.x += inputs->stickX * PLAYER_SPEED * dt;

    // Screen bounds check
    float half_width = PLAYER_WIDTH / 2.0f;
    if (state->player.pos.x < -1.0f + half_width) {
        state->player.pos.x = -1.0f + half_width;
    }
    if (state->player.pos.x > 1.0f - half_width) {
        state->player.pos.x = 1.0f - half_width;
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
            if (state->bullets[i].pos.y > 1.1f) {
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
                state->asteroids[i].pos.x = rand_float(-1.0f, 1.0f);
                state->asteroids[i].pos.y = 1.1f; // Spawn just above the screen
                state->asteroids[i].vel.x = rand_float(-0.1f, 0.1f);
                state->asteroids[i].vel.y = -rand_float(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED);
                state->asteroids[i].size = rand_float(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE);
                state->asteroids[i].hp = state->asteroids[i].size * 150.0f; // Bigger asteroids have more HP
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
                state->bullets[b].active = false; // Bullet is used up
                if (state->asteroids[a].hp <= 0) {
                    state->asteroids[a].active = false;
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

// Helper to convert world coordinates [-1, 1] to screen pixels [0, Res-1]
static void world_to_screen(float wx, float wy, uint8_t* sx, uint8_t* sy) {
    *sx = (uint8_t)((wx + 1.0f) * 0.5f * WIDTH);
    // Y is inverted: world +1 is top, screen 0 is top
    *sy = (uint8_t)((-wy + 1.0f) * 0.5f * HEIGHT);
}

// Helper to draw a simple filled rectangle
static void draw_filled_rect(JaDraw<WIDTH, HEIGHT>& canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool white) {
    // Clamp width and height to avoid overflow and infinite loops
    if (x >= WIDTH || y >= HEIGHT) return;
    uint8_t max_w = (x + w > WIDTH) ? (WIDTH - x) : w;
    uint8_t max_h = (y + h > HEIGHT) ? (HEIGHT - y) : h;
    for (uint8_t i = x; i < x + max_w; ++i) {
        for (uint8_t j = y; j < y + max_h; ++j) {
            drawPixel(canvas, i, j, white);
        }
    }
}

// Draws the GAME OVER message using rectangles
void draw_game_over(JaDraw<WIDTH, HEIGHT>& canvas) {
    // Simple "GAME OVER" text using rectangles
    // G
    draw_filled_rect(canvas, 20, 20, 15, 3, true); draw_filled_rect(canvas, 20, 20, 3, 15, true); draw_filled_rect(canvas, 20, 32, 15, 3, true); draw_filled_rect(canvas, 32, 26, 3, 9, true); draw_filled_rect(canvas, 28, 26, 5, 3, true);
    // O
    draw_filled_rect(canvas, 40, 20, 15, 3, true); draw_filled_rect(canvas, 40, 20, 3, 15, true); draw_filled_rect(canvas, 52, 20, 3, 15, true); draw_filled_rect(canvas, 40, 32, 15, 3, true);
    // ... and so on for the rest of the text. This is tedious but works without fonts.
    // For brevity, we'll just show one letter.
}


static void draw_game(const GameState* state, JaDraw<WIDTH, HEIGHT>& canvas) {
    clear_canvas(canvas);

    if (state->gameOver) {
        draw_game_over(canvas);
        // A simple text would be better, but rectangles work. "GAME OVER"
        // This is a placeholder for a more elaborate screen.
        draw_filled_rect(canvas, 30, 28, 68, 8, true);
        return;
    }

    uint8_t sx, sy, sw, sh;

    // --- Draw Player ---
    world_to_screen(state->player.pos.x - PLAYER_WIDTH / 2.0f, state->player.pos.y + PLAYER_HEIGHT / 2.0f, &sx, &sy);
    sw = (uint8_t)(PLAYER_WIDTH * WIDTH * 0.5f);
    sh = (uint8_t)(PLAYER_HEIGHT * HEIGHT * 0.5f);
    draw_filled_rect(canvas, sx, sy, sw, sh, true);

    // --- Draw Bullets ---
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (state->bullets[i].active) {
            world_to_screen(state->bullets[i].pos.x - BULLET_WIDTH / 2.0f, state->bullets[i].pos.y + BULLET_HEIGHT / 2.0f, &sx, &sy);
            sw = (uint8_t)(BULLET_WIDTH * WIDTH * 0.5f);
            sh = (uint8_t)(BULLET_HEIGHT * HEIGHT * 0.5f);
            draw_filled_rect(canvas, sx, sy, sw, sh, true);
        }
    }

    // --- Draw Asteroids ---
    for (int i = 0; i < MAX_ASTEROIDS; ++i) {
        if (state->asteroids[i].active) {
            world_to_screen(state->asteroids[i].pos.x - state->asteroids[i].size / 2.0f, state->asteroids[i].pos.y + state->asteroids[i].size / 2.0f, &sx, &sy);
            sw = sh = (uint8_t)(state->asteroids[i].size * WIDTH * 0.5f); // Asteroids are square
            draw_filled_rect(canvas, sx, sy, sw, sh, true);
        }
    }
    
    // --- Draw Laser ---
    if (state->laser.active) {
        world_to_screen(state->laser.x_pos - LASER_WIDTH / 2.0f, 1.0f, &sx, &sy);
        sw = (uint8_t)(LASER_WIDTH * WIDTH * 0.5f);
        draw_filled_rect(canvas, sx, 0, sw, HEIGHT, true);
    }
    
    // --- Draw UI (HP Bar) ---
    uint8_t hp_bar_width = (uint8_t)((state->player.hp / PLAYER_INITIAL_HP) * (WIDTH - 4));
    draw_filled_rect(canvas, 2, HEIGHT - 5, hp_bar_width, 3, true);

}
