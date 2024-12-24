#include "../include/House.h"
#include "../include/Device.h"
#include <stdexcept>
#include "House.h"

/*
Il limite massimo di potenza che si può assorbire dalla rete è fornito in
fase di inizializzazione al sistema (tramite una costante definita nel codice 
o un argomento da riga di comando) ed è pari a 3,5kW.
*/
House::House(float maxPower) : grid(maxPower){
    currEnCost = 0;
}

std::string House::show(){
    std::string out = "";
    int enTot = 0;
    for(Device d : devices){
        out += d.show() + "\n";
        enTot += d.getEnTotal();
    }
    out += "Il sistema ha prodotto/consumato finora " + std::to_string(enTot);
}

std::string House::show(std::string name){
    try {
        Device d = search(name);
        return d.show();   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::setTime(Clock skipTime){
    //DIFFICILE
}

/*
se la potenza assorbita da tutti i dispositivi è maggiore della potenza massima 
che si può assorbire dalla rete il sistema inizia a spegnere i dispositivi 
nell’ordine inverso rispetto all’accensione fino ad ottenere una potenza 
assorbita minore di quella prodotta, riportando a schermo le azioni eseguite.
*/
std::string House::setOn(std::string name){
    try {
        std::string out = "";
        Device d = search(name);
        if(d.isActive()){
            return "Dispositivo " + d.getName() + " e' gia' acceso";
        }
        d.turnOn(currTime);
        currEnCost += d.getEnergy();
        activeD.push_back(d);
        out += "Dispositivo " + d.getName() + " acceso";
        //controllo se energicamente è tutto ok
        if(checkOvrload()) out += "\nIl sistema e' in sovraccarico!";
        while (checkOvrload()){
            Device r = activeD[0];
            r.turnOff(currTime);
            currEnCost -= r.getEnergy();
            out += "\nDispositivo " + d.getName() + " spento";
            activeD.erase(activeD.begin()); // cancella il primo elemento
        }
        return out;
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::setOff(std::string name){
    try {
        Device d = search(name);
        if(!d.isActive()){
            return "Dispositivo " + d.getName() + " e' gia' spento";
        }
        d.turnOff(currTime);
        currEnCost -= d.getEnergy();
        deactivateDevice(d); //rimuove d da activeD
        return "Dispositivo " + d.getName() + " spento";
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::remove(std::string name){
    try {
        Device d = search(name);
        if (isManual(d)){
            d.removeTimer();
            return "Timer rimosso con successo";
        }else{
            return "Impossibile rimuovere il timer a un device a ciclo prefissato";
        }
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

std::string House::setScheduledOn(std::string name, Clock start){
    try {
        Device d = search(name);
        d.setSchedule(start);
        return "Impostato correttamente l'orario di accensione";   
    }
    catch(const std::exception& e) {
        return "Dispositivo non trovato, nome scritto in maniera errata";
    }
}

//SOLO DISPOSITIVI MANUAL (per scelta)
std::string House::setScheduledOn(std::string name, Clock start, Clock stop){
    try {
        Device d = search(name);
        if (isManual(d)){
            Manual* derivedPtr = dynamic_cast<Manual*>(&d);
            try {  // per vedere se l'orario è valido o no
                derivedPtr->setTimer(start-stop); // Clock start - Clock stop = durata del "timer"
            }
            catch(const std::exception& e) {
                delete derivedPtr; 
                return "Orario di avvio/spegnimento non validi!";
            }
            delete derivedPtr; 
            return "Impostato correttamente l'orario di accensione e di spegnimento";
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
Resetta il tempo del sistema, riportandolo all’orario 00:00.
Riporta tutti i dispositivi alle condizioni iniziali.
Gli eventuali timer aggiunti dopo l’avvio del sistema vengono mantenuti.
*/
std::string House::resetTime(){
    currTime.reset();
    for(Device d : devices){
        d.deactivate();
        d.setEnTotal(0);
        d.removeSchedule();
    }
    return "Resettato tempo del sistema, riportati tutti i dispositivi allo stato di partenza";
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
    for(Device d : devices){
        d.removeTimer();
    }
    return "Rimossi tutti i timer di tutti i dispositivi. ";
}

/*
Comando per il debug. 
Riporta il sistema alle condizioni iniziali. 
L’orario viene impostato a 00:00, tutti i timer vengono rimossi. 
Tutti i dispositivi vengono spenti.
*/
std::string House::resetAll(){
    resetTime();
    resetTimers();
    return "Resettato tutto";
}

Device House::search(std::string name){
    for (Device d : devices){
        if(d.getName() == name){
            return d;
        }
    }
    throw std::invalid_argument("Bad parameter.");
}
    
bool House::checkOvrload(){ // vero se c'è overload, falso se è tutto ok
    return fabs(currEnCost) > grid; //fabs: valore assoluto per i float
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

void House::deactivateDevice(Device d){  //DA PROVARE
    for(auto i = activeD.begin(); i != activeD.end(); ){
        if (i->getName() == d.getName()) {
            i = activeD.erase(i);
        }else{
            ++i;
        }
    }
}
