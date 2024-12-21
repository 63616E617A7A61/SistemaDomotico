#include <string>
#include "Clock.h"

class Device{
private:
    int ID;
    std::string name;
    float energy;
    float enTotal;
    Clock timeOn;
    Clock timer;
public:
    Device(int id, std::string name, float energy);
    std::string show();
    bool virtual check(Clock skipTime);
    void turnOn(Clock currTime);
    void turnOff(Clock currTime);
    Device& operator=(const Device& other) = delete;
    // getter e setter
};
