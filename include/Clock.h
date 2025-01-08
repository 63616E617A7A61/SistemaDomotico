#ifndef CLOCK
#define CLOCK

#include <string>

class Clock{
    private:
        int hh;
        int mm;
        void init(int hh, int mm);
    public:
        Clock(); //imposta l'orario a 0,0
        Clock(int hh, int mm); //inizializza l'orario se e' valido senno' lancia exception
        Clock(std::string time);    //estrae da stringa hh e mm e chiama Clock(int, int)
        
        int getHh() const { return hh; }
        int getMm() const { return mm; }

        void reset();
        
        void setHh(int hh);
        void setMm(int mm);
        
        std::string toString(); // ritorna [hh:mm]
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