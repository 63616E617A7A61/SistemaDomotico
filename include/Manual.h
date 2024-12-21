#include "Device.h"
#include "Clock.h"

class Manual : public Device{
private:
    
public:
    Manual();
    void setTimer(Clock timer);
    bool check(Clock skipTime);
};