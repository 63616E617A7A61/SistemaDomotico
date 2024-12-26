#ifndef CLOCK
#define CLOCK

#include <string>

class Clock{
private:
    int hh;
    int mm;
    bool active;
public:
    Clock(); //imposta l'orario a 0,0
    Clock(int hh, int mm); //inizializza l'orario se e' valido senno' lancia exception
    bool isActive();
    bool isValid(); //verifica se l'orario è "corretto" e se non lo è lancia eccezione e torna false
    Clock operator+ (Clock &time);
    Clock operator- (Clock &time);
    void reset();
    bool operator> (Clock &time);
    bool operator<= (Clock &time);
    bool operator< (Clock &time);
    bool operator== (Clock &time);
    void setHh(int hh);
    void setMm(int mm);
    int getHh();
    int getMm();
    std::string toString(); // ritorna [hh:mm]
};

#endif