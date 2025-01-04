#include "../include/House.h"
#include "../include/Id.h"
#include "../include/Device.h"
#include "../include/Auto.h"
#include "../include/Manual.h"
#include <stdexcept>
#include <map>
#include <fstream>
#include <cmath>
#include <iostream>

/* TODO PER ME:
- tradurre tutto in inglese
*/

/*
Il limite massimo di potenza che si può assorbire dalla rete è fornito in
fase di inizializzazione al systema (tramite una costante definita nel codice 
o un argomento da riga di comando) ed è pari a 3,5kW.
*/
House::House(float maxPower) : grid(maxPower){
    currEnCost = 0;
    active = true;
}

/*
[13:00] Attualmente il sistema ha prodotto XX kWh e consumato YY kWh. Nello specifico:
- Il dispositivo ‘${DEVICENAME}’ ha consumato XX kWh
- Il dispositivo ‘${DEVICENAME}’ ha consumato XX kWh
*/
std::string House::show(){
    std::string out = "";
    float absorbed = 0;
    float wasted = 0;
    for(Device* d : devices){
        out += "- " + d->show(currTime) + "\n";
        if (d->getEnTotal() < 0)
            wasted += d->getEnTotal();
        else
            absorbed += d->getEnTotal();
    }
    out.resize(out.size() - 1); // Eliminare gli ultimi due caratteri --> ovvero l'ultimo \n
    return currTime.toString() + " Attualmente il sistema ha prodotto " + std::to_string(absorbed) + " kWh e consumato " + std::to_string(wasted) +" kWh. Nello specifico:\n" + out;
}

std::string House::show(std::string name){
    try {
        Device* d = search(name);
        return currTime.toString() + d->show(currTime);   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::setTime(Clock skipTime){
    if (skipTime<currTime) {
        active = false;
        skipTime = Clock(23, 59);
    }
    std::multimap<Clock, std::pair<Device*, bool>> events;  //bool serve da flag true se e' un accensione, false se e' uno spegnimento
    for(Device* i : devices){  // costruisco la mappa con tutti gli eventi ordinati
        if(i->check(skipTime, currTime)) {
            if(i->getTimeOn() > currTime){ //evento on
                events.insert(std::pair<Clock, std::pair<Device*, bool>>(i->getTimeOn(), std::pair<Device*, bool>(i, true)));
                // se il dispositivo in quel lasso di tempo si accende ma si spegne anche
                if(i->getTimer() != nullptr && ((i->getTimeOn() + i->getTimer()) <= skipTime)){
                    events.insert(std::pair<Clock, std::pair<Device*, bool>>((i->getTimeOn() + i->getTimer()), std::pair<Device*, bool>(i, false)));
                }
            }else {  //evento off
                events.insert(std::pair<Clock, std::pair<Device*, bool>>(i->getTimeOn() + i->getTimer(), std::pair<Device*, bool>(i, false)));
            }
        }
    }

    std::string out = "";

    for(std::pair<Clock, std::pair<Device*, bool>> i : events){ //esegue gli eventi in ordine
        Clock key = i.first;
        Device* value = i.second.first;
        bool flag = i.second.second;

        currTime = key;
        if(flag){ // evento on
            out += setOn(value->getName());
        }else{ //evento off
            out += setOff(value->getName());
        }
        out += "\n";
    }

    currTime = skipTime;

    return out + getCurrentTime(); 
}

/*
se la potenza assorbita da tutti i dispositivi è maggiore della potenza massima 
che si può assorbire dalla rete il systema inizia a spegnere i dispositivi 
nell’ordine inverso rispetto all’accensione fino ad ottenere una potenza 
assorbita minore di quella prodotta, riportando a schermo le azioni eseguite.
*/
std::string House::setOn(std::string name){
    try {
        std::string out = "";
        Device* d = search(name);
        if(d->isActive()){
            return "Dispositivo " + d->getName() + " e' gia' acceso";
        }
        d->turnOn(currTime);
        currEnCost += d->getEnergy();
        activeD.push_back(d);
        out += currTime.toString() + " Dispositivo " + d->getName() + " acceso";
        //controllo se energicamente è tutto ok
        if(checkOvrload()){
            out += "\nIl systema e' in sovraccarico!";
            restoreEnergyLimit();
        }
        return out;
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::setOff(std::string name){
    std::string out = "";
    try {
        Device* d = search(name);
        try{
            d->turnOff(currTime);
        }
        catch(const std::exception& e) {
            return "Dispositivo " + d->getName() + " e' gia' spento";
        }
        currEnCost -= d->getEnergy();
        deactivateDevice(*d); //rimuove d da activeD
        out += currTime.toString() + " Dispositivo " + d->getName() + " spento";
        if(checkOvrload()){
            out += "\nIl systema e' in sovraccarico!";
            restoreEnergyLimit();
        }
        return out;
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

//SOLO DISPOSITIVI MANUAL (per scelta)
/*
[13:00] Rimosso il timer dal dispositivo ‘${DEVICENAME}’
*/
std::string House::remove(std::string name){
    try {
        Device* d = search(name);
        if (isManual(*d)){
            d->removeTimer();
            return currTime.toString() + " Rimosso il timer dal dispositivo " + d->getName();
        }else{
            return "Impossibile rimuovere il timer a un device a ciclo prefissato";
        }
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

//PER EVITARE BUG NOTEVOLI NEL SET TIME SI PUO' FARE SOLO SE IL DISPOSITIVO E' SPENTO
std::string House::setScheduledOn(std::string name, Clock start){
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

//SOLO DISPOSITIVI MANUAL
//PER EVITARE BUG NOTEVOLI NEL SET TIME SI PUO' FARE SOLO SE IL DISPOSITIVO E' SPENTO
std::string House::setScheduledOn(std::string name, Clock start, Clock stop){
    try {
        Device* d = search(name);
        if (isManual(*d)){
            Manual* derivedPtr = dynamic_cast<Manual*>(d);
            try {  // per vedere se l'orario è valido o no
                if(derivedPtr->isActive())
                    return "Impossibile impostare un orario di accensione ad un dispositivo gia' acceso";

                derivedPtr->setTimer(start-stop); // Clock start - Clock stop = durata del "timer"
                derivedPtr->setSchedule(start);
            }
            catch(const std::exception& e) {
                delete derivedPtr; 
                return "Orario di avvio/spegnimento non validi!";
            }
            delete derivedPtr; 
            return currTime.toString() + " Impostato un timer per il dispositivo " + d->getName() + " dalle " + start.toString() + " alle "+ stop.toString();
        }else{
            return "Impossibile rimuovere il imopostare l'orario di spegnimento ad un device a ciclo prefissato";
        }   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

/*
Comando per il debug. 
Resetta il tempo del systema, riportandolo all’orario 00:00.
Riporta tutti i dispositivi alle condizioni iniziali.
Gli eventuali timer aggiunti dopo l’avvio del systema vengono mantenuti.
*/
std::string House::resetTime(){
    currTime.reset();
    for(Device* d : devices){
        d->deactivate();
        d->setEnTotal(0);
        d->removeSchedule();
    }
    return getCurrentTime();
}

/*
Comando per il debug. 
Rimuove i timer di tutti i dispositivi. 
Tutti i dispositivi rimangono nel loro stato attuale (accesi o spenti).
NOTA TOMMI:
è di debug quindi lo rimuovo a tutti i dispositivi sbatte il cazzo se sono
auto o manual.
*/
std::string House::resetTimers(){
    for(Device* d : devices){
        d->removeTimer();
    }
    return "Rimossi tutti i timer di tutti i dispositivi";
}

/*
Comando per il debug. 
Riporta il systema alle condizioni iniziali. 
L’orario viene impostato a 00:00, tutti i timer vengono rimossi. 
Tutti i dispositivi vengono spenti.
*/
std::string House::resetAll(){
    resetTime();
    resetTimers();
    return "Resettato tutto";
}

/*
FUNZIONE POPOLA DEVICES CHE RICEVE COME PARAMETRO UNA STRINGA CON UN PATH (FILE.TXT) 
E LEGGE UN FILE CON TUTTE LE SPECIFICHE DEI DEVICE E POPOLA IL VETTORE
*/
std::string House::loadsDevices(const std::string& filePath) {
    Id sys;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return "Errore nell'apertura del file " + filePath;
    }
    
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

        if (words.size() > 2) {  // è auto
            try {
                Clock c(std::stoi(words[1]), std::stoi(words[2]));
                Auto* a = new Auto(sys.getId(), words[0], std::stof(words[3]), c);
                devices.push_back(a);
            } catch(const std::exception& e) {
                file.close();
                throw std::invalid_argument("ahahahahah");
            }
        } else { // è manual
            Manual* m = new Manual(sys.getId(), words[0], std::stof(words[1]));
            devices.push_back(m);
        }
    }

    file.close();

    return "Inizializzato correttamente tutti i dispositivi";
}

std::string House::getCurrentTime(){
    std::string str = currTime.toString();
    return str + " L'orario attuale e' " + str.substr(1, str.length()-2);
}

Device* House::search(std::string name){
    for (Device* d : devices){
        if(d->getName() == name){
            return d;
        }
    }
    throw std::invalid_argument("Bad parameter.");
}
    
bool House::checkOvrload(){ // vero se c'è overload, falso se è tutto ok
    return abs(currEnCost) > grid; //in c++ in <cmath> abs fa overload per tutti i tipi ti dato primitivo
}

bool House::isManual(Device d){ // testato (funziona) non lancia eccezioni
    Device* basePtr = &d;

    // Verifica se il downcasting è lecito
    Manual* derivedPtr = dynamic_cast<Manual*>(basePtr);

    if (derivedPtr) {
        // Il downcasting è lecito
        delete basePtr;
        delete derivedPtr;
        return true;
    } else {
        // Il downcasting non è lecito
        delete basePtr;
        delete derivedPtr;
        return false;
    }
}

//Testata, rimuove d da activeD
void House::deactivateDevice(Device d){  
    for(auto i = activeD.begin(); i != activeD.end(); ){
        if ((*i)->getName() == d.getName() && (*i)->getId() == d.getId()) {
            i = activeD.erase(i);
        }else{  // senno' non cancella l'ultimo
            ++i;
        }
    }
}

std::string House::restoreEnergyLimit(){
    std::string out = "";
    while (checkOvrload()){ 
        Device r = *activeD[activeD.size()-1];
        r.turnOff(currTime);
        currEnCost -= r.getEnergy();
        out += "\n" + currTime.toString() + " Dispositivo " + r.getName() + " spento";
        activeD.pop_back();
    }
    return out;
}