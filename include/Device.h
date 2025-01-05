#ifndef DEVICE
#define DEVICE

#include <string>
#include "Clock.h"

class Device{
protected: 
    Clock* timer;
private:
    int ID;
    std::string name;
    float energy;
    float enTotal;
    Clock* timeOn;
    bool active;
public:
    Device(int id, std::string name, float energy);
    Device();
    float show(Clock currentTime);
    bool check(Clock skipTim, Clock currTime);
    void turnOn(Clock currTime);
    void turnOff(Clock currTime); //spegne il dispositivo calcola i consumi e chiama deactivate()
    // Device& operator=(const Device& other) = delete;  da problemi nella classe House.cpp
    std::string getName();
    float getEnTotal();
    float getEnergy();
    int getId();
    Clock* getTimeOn();
    Clock* getTimer();
    void deactivate(); // set active to false
    void setEnTotal(float val);
    void removeSchedule(); // svuota timeOn
    void setSchedule(Clock start); //se l'orario è valico imposta timeOn altrimenti lancia eccezione (invalid_argument)
    void removeTimer();
    bool isActive();
    virtual ~Device();
};

#endif
