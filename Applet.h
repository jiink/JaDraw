#pragma once
#include "system.h"
#include "JaDraw.h"

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

void MyApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    t += dt;
    step++;
    const float speed = 10.0f;
    x += speed * dt * 2.3f;
    y += speed * dt * 1.5f;
    if (x >= WIDTH) x = 0;
    if (y >= HEIGHT) y = 0;
    canvas.clear(0x001030FF);
    canvas.drawLineAA(0, 0, x, y, Colors::Red);
    canvas.drawLineAA(WIDTH - 1, 0, x, y, Colors::Orange);
    static char out[20];
    snprintf(out, sizeof(out), "hello? %c", (unsigned char)(t*10.0f));
    canvas.drawText(out,
        8 + 16 * sin(t * 1.0f),
        10 + 10 * cos(t * 0.8f),
        2.0f + 1.0f * cos(t * 0.5),
        Colors::Magenta);
}

const char* MyApplet::getName() const {
    return "Simple Clock";
}
