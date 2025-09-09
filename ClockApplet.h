#pragma once
#include "IApplet.h"
#include "JaDraw.h"
#include "vmath_all.hpp"

struct TimeInfo {
    int month;
    int day;
    int hour;
    int minute;
    int second;
};


class ClockApplet : public IApplet {
public:
    ClockApplet();
    ~ClockApplet() override = default;
    void setup() override;
    void loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) override;
    const char* getName() const override;
private:
    float x = 0.0f;
    unsigned long millis = 0;
    bool dotsVisibile = false;
    TimeInfo currentTime = {0};
    void wallclockUpdate(TimeInfo& ti);
    static unsigned long timerDiff(unsigned long a, unsigned long b);
};

inline unsigned long ClockApplet::timerDiff(unsigned long a, unsigned long b) {
    return (a > b) ? (a - b) : (b - a);
}

ClockApplet::ClockApplet() { }

void ClockApplet::wallclockUpdate(TimeInfo& ti)
{
    const unsigned long WALLCLOCK_UPDATE_INTERVAL = 1000;
    static unsigned long wallclockUpdateTimer = 0;
    if (timerDiff(millis, wallclockUpdateTimer) > WALLCLOCK_UPDATE_INTERVAL)
    {
        wallclockUpdateTimer = millis;
        // real time
        // time_t now;
        // time(&now);
        // tm* localTime = localtime(&now);
        // ti.month = localTime->tm_mon + 1;
        // ti.day = localTime->tm_mday;
        // ti.hour = localTime->tm_hour % 12;
        // ti.minute = localTime->tm_min;
        // ti.second = localTime->tm_sec;
        // dotsVisibile = !dotsVisibile;
        
        // fake time
        ti.month = 1 + (rand() % 12);      
        ti.day = 1 + (rand() % 31);   
        ti.hour = rand() % 24;             
        ti.minute = rand() % 60;           
        ti.second = rand() % 60;           
        dotsVisibile = !dotsVisibile;
    }
}

void ClockApplet::setup() { }

void ClockApplet::loop(JaDraw<WIDTH, HEIGHT>& canvas, float dt, const InputData& inputs) {
    //t += dt;
    millis += dt * 1000;
    wallclockUpdate(currentTime);
    canvas.clear(0);
    static char out[20];
    snprintf(out, sizeof(out), "%02d:%02d",
        currentTime.hour,
        currentTime.minute,
        currentTime.second);
    float hue = (currentTime.hour % 12) / 12.0f;
    canvas.drawText(out, 8, 8, 2.5, canvas.hsvToRgba(hue, 0.6, 1), false);
}

const char* ClockApplet::getName() const {
    return "Simple Clock";
}
