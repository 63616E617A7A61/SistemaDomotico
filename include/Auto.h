#include "Device.h"

class Auto : public Device{
private:
    Clock timer;
    
public:
    Auto(int id, std::string name, float energy, Clock timer);
    bool check(Clock skipTime);
};
    
