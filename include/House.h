#include <vector>
#include <string>
#include "Device.h"
#include "Clock.h"

class House{
private:
    std::vector<Device> devices;
    std::vector<Device> activeD;
    const float grid = 3.5;
    float currEnCost;
    Clock currTime;

    Device search(std::string name);
    bool checkOvrload();
public:
    House();
    std::string show();
    std::string show(std::string name);
    std::string setTime(Clock skipTime);
    std::string setOn(std::string name);
    std::string setOff(std::string name);
    std::string remove(std::string name);
    std::string setScheduledOn(std::string name, Clock start);
    std::string setScheduledOn(std::string name, Clock start, Clock stop);
    std::string resetTime();
    std::string resetTimer();
    std::string resetAll();
};



