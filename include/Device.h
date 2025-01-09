#ifndef DEVICE
#define DEVICE

#include <string>
#include "Clock.h"
#include <stdexcept>

class Device{
private:
    int ID;
    std::string name;
    float energy;
    float enTotal;
    Clock* timeOn;
    bool active;
    bool essential;
protected: 
    Clock* timer;
    Device(int id, std::string name, float energy);
    Device();
public:
    float show(Clock& currentTime) const;
    bool check(Clock& skipTim, Clock& currTime);
    void turnOn(Clock& currTime);
    void turnOff(Clock& currTime);
    std::string getName() const;
    float getEnTotal() const;
    float getEnergy() const;
    int getId() const;
    Clock getTimeOn() const;
    Clock getTimer() const;
    bool timerExist() const;
    void deactivate(); 
    void setEnTotal(float val);
    void removeSchedule(); 
    void setSchedule(Clock& start);
    void removeTimer();
    bool isActive() const;
    bool isEssential() const;
    void setEssential(bool val);
    virtual ~Device();
};

#endif
