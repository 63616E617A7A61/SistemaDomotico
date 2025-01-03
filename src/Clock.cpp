#include "../include/Clock.h"

Clock::Clock() : Clock::Clock(0, 0) {}

Clock::Clock(int hh, int mm) {
    if((hh < 0 || hh > 23) && (mm < 0 || mm > 59)) {
        throw std::invalid_argument("Orario non valido");
    } else {
        this->hh = hh;
        this->mm = mm;
    }
}

Clock::Clock(std::string time) : Clock::Clock(std::stoi(time.substr(0, 2)), std::stoi(time.substr(3, 2))) {}    //std::stoi converte string a int

void Clock::reset() {
    this->hh = 0;
    this->mm = 0;
}

void Clock::setHh(int hh) {
    if(hh < 0 || hh > 23) {
        throw std::invalid_argument("Orario non valido");
    } else {
        this->hh = hh;
    }
}

void Clock::setMm(int mm) {
    if(mm < 0 || mm > 59) {
        throw std::invalid_argument("Orario non valido");
    } else {
        this->mm = mm;
    }
}

std::string Clock::toString() {
    std::string h = (hh < 10) ? "0" : "";
    std::string m = (mm < 10) ? "0" : "";
    return "[" + h + std::to_string(hh) + ':' + m + std::to_string(mm) + ']';
}

Clock operator+ (const Clock& c1, const Clock& c2) {
    int newMM = c1.getMm() + c2.getMm();
    int ofHH = newMM / 60; //HH aggiuntive date dall'OverFlow di minuti
    newMM%=60;             //calcolo dell'effettivo numero di minuti nel clock da ritornare
    int newHH = (c1.getHh() + c2.getHh() + ofHH) % 24;

    return Clock(newHH, newMM);
}   

Clock operator- (const Clock& c1, const Clock& c2) {
    int newMM = 0, newHH = 0;
    if(c1 < c2) {
        throw std::invalid_argument("Output would be negative");
    } else {
        if(c1.getMm() >= c2.getMm()) {
            newMM = c1.getMm() - c2.getMm();
            newHH = c1.getHh() - c2.getHh();
        } else {
            int tempC1HH = c1.getHh() - 1;
            newMM = 60 + c1.getMm() - c2.getMm();
            newHH = tempC1HH - c2.getHh();
        }
    }

    return Clock(newHH, newMM);
}

bool operator> (const Clock& c1, const Clock& c2) {
    if(c1.getHh() > c2.getHh()) {
        return true;
    } else if (c1.getHh() == c2.getHh()) {
        return c1.getMm() > c2.getMm();
    } else {
        return false;
    }
}

bool operator>= (const Clock& c1, const Clock& c2) {
    return c1 > c2 || c1 == c2;
}

bool operator<= (const Clock& c1, const Clock& c2) {
    return !(c1 > c2);
}

bool operator< (const Clock& c1, const Clock& c2) {
    return !(c1 > c2) && c1 != c2;
}

bool operator== (const Clock& c1, const Clock& c2) {
    return c1.getHh() == c2.getHh() && c1.getMm() == c2.getMm();
}

bool operator== (const Clock& c1, std::nullptr_t nptr) {
    return c1 == nptr;
}

bool operator!= (const Clock& c1, const Clock& c2) {
    return !(c1 == c2);
}

bool operator!= (const Clock& c1, std::nullptr_t nptr) {
    return c1 != nptr;
}