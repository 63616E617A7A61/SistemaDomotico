class Clock{
private:
    int hh;
    int mm;
    bool active;
public:
    Clock();
    bool isActive();
    Clock operator+ (Clock &time);
    Clock operator- (Clock &time);
    bool operator> (Clock &time);
    bool operator< (Clock &time);
    bool operator== (Clock &time);
    void setHh(int hh);
    void setMm(int mm);
    int getHh();
    int getMm();
};
