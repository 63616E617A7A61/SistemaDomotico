#ifndef CLOCK
#define CLOCK

class Clock{
private:
    int hh;
    int mm;
    bool active;
public:
    Clock(); //imposta l'orario a 0,0
    Clock(int hh, int mm);
    bool isActive();
    bool isValid(); //verifica se l'orario è "corretto" e se non lo è lancia eccezione
    Clock operator+ (Clock &time);
    Clock operator- (Clock &time);
    void reset();
    bool operator> (Clock &time);
    bool operator< (Clock &time);
    bool operator== (Clock &time);
    void setHh(int hh);
    void setMm(int mm);
    int getHh();
    int getMm();
};

#endif