#ifndef HOUSE
#define HOUSE

#include <vector>
#include <string>
#include "Device.h"
#include "Manual.h"
#include "Clock.h"

class House{
private:
    std::vector<Device> devices;
    std::vector<Device> activeD;
    const float grid;
    float currEnCost;
    Clock currTime;

    Device search(std::string name);
    bool checkOvrload();
    bool isManual(Device d);
    void deactivateDevice(Device d);
public:
    House(float grid);
    bool isActive();    //necessario per il loop del main
    std::string show();
    std::string show(std::string name);
    std::string setTime(Clock skipTime);
    std::string setOn(std::string name);
    std::string setOff(std::string name);
    std::string remove(std::string name);
    std::string setScheduledOn(std::string name, Clock start);
    std::string setScheduledOn(std::string name, Clock start, Clock stop);
    std::string resetTime();
    std::string resetTimers();
    std::string resetAll();
    std::string loadsDevices(const std::string& filePath);
};

#endif