#include "../include/House.h"
#include "../include/Id.h"
#include "../include/Auto.h"
#include "../include/Manual.h"
#include <stdexcept>
#include <map>
#include <fstream>
#include <cmath>
#include <set>

/** 
 * @brief Constructor for the House class. 
 * @param maxPower The maximum power the house grid can handle. 
*/
House::House(float maxPower) : grid(maxPower){
    currEnCost = 0;
    active = true;
}

/**
 * @brief Shows the current status of all devices in the house. 
 * @return A string representing the current status of all devices.
 */
std::string House::show(){
    std::string out = "";
    float absorbed = 0;
    float wasted = 0;
    for(Device* d : devices){
        float en = d->show(currTime);
        if(en > 0) out += "- Il dispositivo " + d->getName() + " ha prodotto " + format(en) + " Wh\n";
        else out += "- Il dispositivo " + d->getName() + " ha consumato " + format(en) + " Wh\n";
        if (en < 0)
            wasted += en;
        else
            absorbed += en;
    }
    out.resize(out.size() - 1);
    return currTime.toString() + " Attualmente il sistema ha prodotto " + format(absorbed) + " kWh e consumato " + format(wasted) +" kWh. Nello specifico:\n" + out;
}

/**
 * @brief Shows the current status of a specific device. 
 * @param name The name of the device. 
 * @return A string representing the current status of the specified device.
 */
std::string House::show(std::string& name) {
    try {
        Device* d = search(name);
        float en = d->show(currTime);
        if(en > 0) return currTime.toString() + " Il dispositivo " + d->getName() + " ha prodotto " + format(en) + " Wh";
        else return currTime.toString() + " Il dispositivo " + d->getName() + " ha consumato " + format(en) + " Wh";
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/**
 * @brief Sets the time for the house system. 
 * @param skipTime The time to set. 
 * @return A string representing the events that occurred during the time skip.
 */
std::string House::setTime(Clock& skipTime){
    if (skipTime<currTime) {
        active = false;
        skipTime = Clock(23, 59);
    }
    /*
    I use a multimap with a key a Clock that represents the time at which an event must be resolved, 
    and with a value a pair given by a device where to operate an on or off and a boolean value that represents the type of event, precisely on or off. 
    The multimap is a data structure that orders the elements inside it based on the key so that then the resolution of the events becomes trivial, in a way unrelated to time.
    */
    std::multimap<Clock, std::pair<Device*, bool>> events; 
    for(Device* i : devices){   
        if(i->check(skipTime, currTime)) {
            if(i->getTimeOn() > currTime){ 
                events.insert(std::pair<Clock, std::pair<Device*, bool>>(i->getTimeOn(), std::pair<Device*, bool>(i, true)));
                if(i->timerExist() && ((i->getTimeOn() + i->getTimer()) <= skipTime)){
                    events.insert(std::pair<Clock, std::pair<Device*, bool>>((i->getTimeOn() + i->getTimer()), std::pair<Device*, bool>(i, false)));
                }
            }else { 
                events.insert(std::pair<Clock, std::pair<Device*, bool>>((i->getTimeOn() + i->getTimer()), std::pair<Device*, bool>(i, false)));
            }
        }
    }

    std::string out = "";

    for(std::pair<Clock, std::pair<Device*, bool>> i : events){
        Clock key = i.first;
        Device* value = i.second.first;
        bool flag = i.second.second;

        currTime = key;
        if(flag){ 
            out += setOn(value->getName());
        }else{ 
            out += setOff(value->getName());
        }
        out += "\n";
    }

    currTime = skipTime;

    return out + getCurrentTime(); 
}

/**
 * @brief Turns on a specific device. 
 * @param name The name of the device. 
 * @return A string representing the status of the device after attempting to turn it on.
 */
std::string House::setOn(std::string& name){
    try {
        std::string out = "";
        Device* d = search(name);
        if(d->isActive()){
            return "Dispositivo " + d->getName() + " e' gia' acceso";
        }
        d->turnOn(currTime);
        currEnCost += d->getEnergy();
        activeD.push_back(getDevIndex(name));
        out += currTime.toString() + " Dispositivo " + d->getName() + " acceso";
        if(checkOvrload()){
            out += "\nIl sistema e' in sovraccarico!";
            out += restoreEnergyLimit();
        }
        return out;
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/**
 * @brief Turns off a specific device. 
 * @param name The name of the device. 
 * @return A string representing the status of the device after attempting to turn it off.
 */
std::string House::setOff(std::string& name){
    std::string out = "";
    try {
        Device* d = search(name);
        if(!d->isActive()){
            return "Dispositivo " + d->getName() + " e' gia' spento";
        }
        d->turnOff(currTime);
        currEnCost -= d->getEnergy();
        deactivateDevice(name);
        out += currTime.toString() + " Dispositivo " + d->getName() + " spento";
        if(checkOvrload()){
            out += "\nIl sistema e' in sovraccarico!";
            out += restoreEnergyLimit();
        }
        return out;
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
    
}

/**
 * @brief Removes the timer from a specific device. 
 * @param name The name of the device. 
 * @return A string representing the result of the operation after attempting to remove the timer.
 */
std::string House::remove(std::string& name){
    try {
        Device* d = search(name);
        if (d->timerExist()){
            d->removeTimer();
            return currTime.toString() + " Rimosso il timer dal dispositivo " + d->getName();
        }else{
            return "Impossibile rimuovere il timer a questo device";
        }
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/**
 * @brief Set a scheduled time to turn on a specific device, the device must be turned off. 
 * @param name The name of the device. 
 * @param start The time to turn on the device. 
 * @return A string representing the result of the operation after attempting to set the schedule.
 */
std::string House::setScheduledOn(std::string& name, Clock& start){
    if (start <= currTime) {
        return "Impossibile impostare un orario di accensione, si prega di utilizzare un orario nel futuro";
    }
    try {
        Device* d = search(name);
        if(d->isActive())
            return "Impossibile impostare un orario di accensione ad un dispositivo gia' acceso";

        d->setSchedule(start);
        return currTime.toString() + " Impostato orario di accensione per il dispositivo " + d->getName() + " alle " + d->getTimeOn().toString();   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/**
 * @brief Set a scheduled time to turn on and off a specific device, if it is a CP device. 
 * @param name The name of the device. 
 * @param start The time to turn on the device. 
 * @param stop The time to turn off the device. 
 * @return A string representing the result of the operation after attempting to set the schedule.
 */
std::string House::setScheduledOn(std::string& name, Clock& start, Clock& stop){
    try {
        Device* d = search(name);
        if (start <= currTime) {
            return "Impossibile impostare un orario di accensione, si prega di utilizzare un orario nel futuro";
        }
        if (start >= stop){
            return "Orario di avvio/spegnimento non validi!";
        }
        Manual* derivedPtr = dynamic_cast<Manual*>(d);
        if (derivedPtr){
            try { 
                if(derivedPtr->isActive())
                    return "Impossibile impostare un orario di accensione ad un dispositivo gia' acceso";

                derivedPtr->setTimer(stop-start);
                derivedPtr->setSchedule(start);
            }
            catch(const std::exception& e) {
                return "Orario di avvio/spegnimento non validi!";
            }
            return currTime.toString() + " Impostato un timer per il dispositivo " + d->getName() + " dalle " + start.toString() + " alle "+ stop.toString();
        }else{
            return "Impossibile imopostare l'orario di spegnimento ad un device a ciclo prefissato";
        }   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/**
 * @brief Reset the current time of your home system, restore all devices to their initial condition.. 
 * @return A string representing the current time after reset.
 */
std::string House::resetTime(){
    currTime.reset();
    for(Device* d : devices){
        d->deactivate();
        d->setEnTotal(0);
    }
    return getCurrentTime();
}

/**
 * @brief Resets all timers of all devices in the house. 
 * @return A string indicating that all timers have been removed.
 */
std::string House::resetTimers(){
    for(Device* d : devices){
        d->removeTimer();
    }
    return "Rimossi tutti i timer di tutti i dispositivi";
}

/**
 * @brief Resets the entire house system, including time and timers. 
 * @return A string indicating that everything has been reset.
 */
std::string House::resetAll(){
    resetTime();
    resetTimers();
    return "Resettato tutto";
}

/**
 * @brief Loads devices from a file. 
 * @param filePath The path to the file containing device information. 
 * @return A string indicating the outcome of the operation.
 * @throws std::runtime_error if the file input has been corrupted.
 */
std::string House::loadsDevices(const std::string& filePath) {
    Id sys;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Errore nell'apertura del file " + filePath);
    }
    
    std::set<std::string> names;
    float essentialEn = 0;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> words;
        std::string buff = "";
        for (char i : line) {  // split 
            if (i == ',' || i == ':') {
                words.push_back(buff);
                buff = "";
            } else {
                buff += i;
            }
        }
        words.push_back(buff);

        if (names.insert(words[0]).second == false){
            throw std::runtime_error("Nel dataset ci sono due dispositivi con lo stesso nome!");
        }

        if (words.size() > 3) { 
            try {
                Clock c(std::stoi(words[1]), std::stoi(words[2]));
                Auto* a = new Auto(sys.getId(), words[0], std::stof(words[3]), c);
                a->setEssential((bool) std::stoi(words[4]));
                if(a->isEssential()) essentialEn += a->getEnergy(); 
                devices.push_back(a);
            } catch(const std::exception& e) {
                file.close();
                throw std::invalid_argument("Orario non valido");
            }
        } else { 
            Manual* m = new Manual(sys.getId(), words[0], std::stof(words[1]));
            m->setEssential((bool) std::stoi(words[2]));
            if(m->isEssential()) essentialEn += m->getEnergy(); 
            devices.push_back(m);
        }
    }

    if (fabs(essentialEn) >= grid) {
        throw std::runtime_error("Nel dataset ci sono troppi dispositivi essenziali!");
    }

    file.close();

    return "Inizializzato correttamente tutti i dispositivi";
}

/**
 * @brief Gets the current time of the house system. 
 * @return A string representing the current time.
 */
std::string House::getCurrentTime(){
    std::string str = currTime.toString();
    return str + " L'orario attuale e' " + str.substr(1, str.length()-2);
}

/**
 * @brief Searches for a device by name. 
 * @param name The name of the device. 
 * @return A pointer to the device if found. 
 * @throws std::invalid_argument if the device is not found.
 */
Device* House::search(std::string& name){
    for (Device* d : devices){
        if(d->getName() == name){
            return d;
        }
    }
    throw std::invalid_argument("Bad parameter.");
}

/**
 * @brief Checks if the current energy cost exceeds the grid capacity. 
 * @return True if the current energy cost exceeds the grid capacity, false otherwise.
 */
bool House::checkOvrload(){ 
    return fabs(currEnCost) > grid; 
}

/**
 * @brief Restores the energy limit by turning off devices until the energy cost is within the grid capacity. 
 * @return A string indicating the devices that were turned off to restore the energy limit.
 */
std::string House::restoreEnergyLimit(){
    std::string out = "";
    while (checkOvrload()){ 
        int i = activeD.size()-1;
        Device* r = devices[activeD[i]];
        // If the device cannot be turned off due to overload, or if it is necessary, find another device to turn off always in the reverse order to turning it on
        while (r->isEssential()) {
            out += "\nImpossibile spegnere " + r->getName() + " a causa di un sovraccarico";
            i--;
            r = devices[activeD[i]];
        }
        r->turnOff(currTime);
        currEnCost -= r->getEnergy();
        out += "\n" + currTime.toString() + " Dispositivo " + r->getName() + " spento";
        deactivateDevice(r->getName());
    }
    return out;
}

/**
 * @brief Gets the index of a device from the devices vector by name. 
 * @param name The name of the device. 
 * @return The index of the device if found, -1 otherwise.
 */
int House::getDevIndex(std::string& name){
    for (int i = 0; i < devices.size(); i++) {
        if (name == devices[i]->getName()) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Removes the indicated device from the active devices vector. 
 * @param name The name of the device.
 */
void House::deactivateDevice(std::string& name){
    int k = getDevIndex(name);
    for (int i = 0; i < activeD.size(); i++) {
        if (activeD[i] == k) {
            activeD.erase(activeD.begin() + i);
            return;
        }
    }
}

/**
 * @brief Formats a float value to a string with a specific format ('f.ff').
 * @param val The float value to be formatted.
 * @return A string representing the formatted float value in the 'f.ff' format.
 */
std::string format(float val){
    std::string out = std::to_string(fabs(val));
    return out.substr(0, out.find('.') + 3);
}