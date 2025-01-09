#ifndef HOUSE
#define HOUSE

#include <vector>
#include <string>
#include "Device.h"
#include "Clock.h"

class House{
private:
    std::vector<Device*> devices;
    std::vector<int> activeD;
    const float grid;
    float currEnCost;
    Clock currTime;
    bool active;

    Device* search(std::string& name);
    bool checkOvrload();
    std::string restoreEnergyLimit();
    int getDevIndex(std::string& name);
    void deactivateDevice(std::string& name);
public:
    House(float grid);
    bool isActive() const {return active;};
    std::string show();
    std::string show(std::string& name);
    std::string setTime(Clock& skipTime);
    std::string setOn(std::string& name);
    std::string setOff(std::string& name);
    std::string remove(std::string& name);
    std::string setScheduledOn(std::string& name, Clock& start);
    std::string setScheduledOn(std::string& name, Clock& start, Clock& stop);
    std::string resetTime();
    std::string resetTimers();
    std::string resetAll();
    std::string loadsDevices(const std::string& filePath);
    std::string getCurrentTime();
};

std::string format(float val);

#endif