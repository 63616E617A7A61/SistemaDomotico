#ifndef CLOCK
#define CLOCK

#include <string>

class Clock{
    private:
        int hh;
        int mm;
        void init(int hh, int mm);
    public:
        Clock();
        Clock(int hh, int mm);
        Clock(std::string time);
        
        int getHh() const { return hh; }
        int getMm() const { return mm; }

        void reset();
        
        void setHh(int hh);
        void setMm(int mm);
        
        std::string toString();
};

Clock operator+ (const Clock& c1, const Clock& c2);
Clock operator- (const Clock& c1, const Clock& c2);
bool operator> (const Clock& c1, const Clock& c2);
bool operator>= (const Clock& c1, const Clock& c2);
bool operator<= (const Clock& c1, const Clock& c2);
bool operator< (const Clock& c1, const Clock& c2);
bool operator== (const Clock& c1, const Clock& c2);
bool operator== (const Clock& c1, std::nullptr_t nptr);
bool operator!= (const Clock& c1, const Clock& c2);
bool operator!= (const Clock& c1, std::nullptr_t nptr);

#endif