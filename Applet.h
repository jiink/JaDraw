#pragma once
#include "system.h"
#include "JaDraw.h"
#include "sun_icon_img.h"
#include "vmath_all.hpp"

struct InputData {
    int rotation = 0; 
    bool pressed = false;
};

class IApplet {
public:
    virtual ~IApplet() = default;
    /// @brief Called once the applet is selected, before running any loop.
    virtual void setup() = 0;
    /// @brief Called every frame.
    /// @param canvas The framebuffer to show up on the screen.
    /// @param dt Time since last call, in seconds.
    /// @param inputs The controls that may be consumed (e.g. knob turns, button presses).
    virtual void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) = 0;
    virtual const char* getName() const = 0;
};

class MyApplet : public IApplet {
public:
    MyApplet();
    ~MyApplet() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
    float x = 0.0f;
    float y = 0.0f;
    float t = 0.0f;
    unsigned int step = 0;
};

MyApplet::MyApplet() { }

void MyApplet::setup() { }

struct fCol {
    float r;
    float g;
    float b;
};
using vmath_hpp::fvec2;
using vmath_hpp::fvec3;
/**
 * @brief Converts a floating-point color (fCol) to a 32-bit RGBA integer.
 * @param floatColor Reference to the floating-point color structure.
 * @return 32-bit unsigned integer representing the RGBA color.
 */
uint32_t fColToRGBA(fCol& floatColor)
{
    if (floatColor.r < 0.0f) { floatColor.r = 0.0f; }
    if (floatColor.r > 1.0f) { floatColor.r = 1.0f; }
    if (floatColor.g < 0.0f) { floatColor.g = 0.0f; }
    if (floatColor.g > 1.0f) { floatColor.g = 1.0f; }
    if (floatColor.b < 0.0f) { floatColor.b = 0.0f; }
    if (floatColor.b > 1.0f) { floatColor.b = 1.0f; }
    uint8_t r = static_cast<uint8_t>(floatColor.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(floatColor.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(floatColor.b * 255.0f);
    uint8_t a = 255;
    return JADRAW_RGBA(r, g, b, a);
}

static fvec3 spherePos(0.0f, 1.0f, 6.0f);
static float sphereRadius = 1.0f;

// Samples the 3D SDF and returns the distance between
// the given position and the defined geometery.
static float getDist(fvec3 pos)
{
    float planeDist = pos.y;
    float sphereDist = vmath_hpp::length(pos - spherePos) - sphereRadius;
    return std::min(planeDist, sphereDist);
}

// Raymarches the scene and returns the final distance from cam to that point
static float raymarch(fvec3 cam, fvec3 dir)
{
    float dist = 0;
    const int maxIter = 100;
    for (int i = 0; i < maxIter; i++)
    {
        fvec3 pos = cam + dir * dist;
        float stepDist = getDist(pos);
        dist += stepDist;
        const float maxDist = 100.0f;
        const float minDist = 0.01f;
        if (dist > maxDist || dist < minDist) { break; }
    }
    return dist;
}

void MyApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;
    spherePos.x = 2.0f * sin(t);
    // step++;
    // const float speed = 10.0f;
    // x += speed * dt * 2.3f;
    // y += speed * dt * 1.5f;
    // if (x >= WIDTH) x = 0;
    // if (y >= HEIGHT) y = 0;
    //canvas.clear(0x001030FF);
    // canvas.drawLineAA(0, 0, x, y, Colors::Red);
    // canvas.drawLineAA(WIDTH - 1, 0, x, y, Colors::Orange);
    // canvas.drawSprite(x, y, Sprites::sun_icon);
    // static char out[20];
    // snprintf(out, sizeof(out), "hello?%c", (unsigned char)(t*10.0f));
    // canvas.drawText(out,
    //     8 + 16 * sin(t * 1.0f),
    //     10 + 10 * cos(t * 0.8f),
    //     2.0f + 1.0f * cos(t * 0.5),
    //     Colors::Magenta);
    // std::vector<Vec2> triangle = {{12, 4}, {19, 11}, {26, 5},
    //                         {25, 18}, {11, 16}};
    // canvas.drawPolygon(triangle, Colors::Grey, false, DrawMode::ADDITIVE);
    const float inverseHeight = 1.0f / static_cast<float>(HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        int fragCoordX = i % WIDTH;
        int fragCoordY = i / WIDTH;
        fvec2 uv(
            (fragCoordX - WIDTH * 0.5f) * inverseHeight,
            -(fragCoordY - HEIGHT * 0.5f) * inverseHeight
        );
        fvec3 cam(0, 1, 1);
        fvec3 ray(uv.x, uv.y, cam.z);
        float dist = raymarch(cam, vmath_hpp::normalize(ray));
        fCol col = {
            .r = dist * 0.1f,
            .g = dist * 0.1f,
            .b = dist * 0.1f
        };
        canvas.drawPixel(fragCoordX, fragCoordY, fColToRGBA(col));
    }
}

const char* MyApplet::getName() const {
    return "Simple Clock";
}
