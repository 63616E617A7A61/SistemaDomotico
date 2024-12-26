#include "../include/House.h"
#include "../include/Id.h"
#include "../include/Device.h"
#include "../include/Auto.h"
#include "../include/Manual.h"
#include <stdexcept>
#include <map>
#include <fstream>

/* TODO PER ME:
- Test generale
- note per gli altri (es: ha senso rimuiovere i timer (la tv non puo' essere che dura solo 1h))
*/

/*
Il limite massimo di potenza che si può assorbire dalla rete è fornito in
fase di inizializzazione al systema (tramite una costante definita nel codice 
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
    out += "Il systema ha prodotto/consumato finora " + std::to_string(enTot);
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
    try{
        if(!skipTime.isValid()){
            return "L'orario inserito non e' valido";
        }    
    }
    catch(const std::exception& e){
        return "L'orario inserito non e' valido";
    }

    std::multimap<Clock, std::pair<Device&, bool>> events;  //bool serve da flag true se e' un accensione, false se e' uno spegnimento
    for(Device i : devices){  // costruisco la mappa con tutti gli eventi ordinati
        if(i.check(skipTime)) {
            if(i.getTimeOn() > currTime){ //evento on
                events.insert(std::pair<Clock, std::pair<Device&, bool>>(i.getTimeOn(), std::pair<Device&, bool>(i, true)));
                // se il dispositivo in quel lasso di tempo si accende ma si spegne anche
                try{
                    if(i.getTimer().isValid() && (i.getTimeOn() + i.getTimer() < skipTime)){
                        events.insert(std::pair<Clock, std::pair<Device&, bool>>(i.getTimeOn() + i.getTimer(), std::pair<Device&, bool>(i, false)));
                    }
                }
                catch(const std::exception& e){
                    continue;
                }
            }else {  //evento off
                events.insert(std::pair<Clock, std::pair<Device&, bool>>(i.getTimeOn() + i.getTimer(), std::pair<Device&, bool>(i, false)));
            }
        }
    }

    std::string out = "";

    for(std::pair<Clock, std::pair<Device&, bool>> i : events){
        Clock key = i.first;
        Device& value = i.second.first;
        bool flag = i.second.second;
        std::string out = "";

        currTime = key;
        if(flag){ // evento on
            out += setOn(value.getName());
        }else{ //evento off
            out += setOff(value.getName());
        }
        out += "\n";
    }

    // Eliminare gli ultimi due caratteri --> ovvero l'ultimo \n
    out.resize(out.size() - 2);

    return out; 
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
        Device d = search(name);
        if(d.isActive()){
            return "Dispositivo " + d.getName() + " e' gia' acceso";
        }
        d.turnOn(currTime);
        currEnCost += d.getEnergy();
        activeD.push_back(d);
        out += "Dispositivo " + d.getName() + " acceso";
        //controllo se energicamente è tutto ok
        if(checkOvrload()) out += "\nIl systema e' in sovraccarico!";
        while (checkOvrload()){
            Device r = activeD[0];
            r.turnOff(currTime);
            currEnCost -= r.getEnergy();
            out += "\nDispositivo " + d.getName() + " spento";
            deactivateDevice(r); // cancella il primo elemento
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
Resetta il tempo del systema, riportandolo all’orario 00:00.
Riporta tutti i dispositivi alle condizioni iniziali.
Gli eventuali timer aggiunti dopo l’avvio del systema vengono mantenuti.
*/
std::string House::resetTime(){
    currTime.reset();
    for(Device d : devices){
        d.deactivate();
        d.setEnTotal(0);
        d.removeSchedule();
    }
    return "Resettato tempo del systema, riportati tutti i dispositivi allo stato di partenza";
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
std::string House::loadsDevices(const std::string& filePath){
    Id sys;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return "Errore nell'apertura del file " + filePath;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> words;
        std::string buff = "";
        for(char i : line){  // split 
            if (i == ',' || i == ':') {
                words.push_back(buff);
                buff = "";
            }else{
                buff += i;
            }
        }
        words.push_back(buff);
        
        if(words.size() > 2) {  // e' auto
            try{
                Clock c(std::stoi(words[1]), std::stoi(words[2]));   
                devices.push_back(Auto(sys.getId(), words[0], std::stof(words[3]), c));
            }
            catch(const std::exception& e) {
                return "Errore nel data set del file: " + line;
            }
        }else{ // e' manual
            devices.push_back(Manual(sys.getId(), words[0], std::stof(words[1])));
        }
    }

    file.close();
    return "Inizializzato correttamente tutti i dispositivi";
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

//Testata, rimuove d da activeD
void House::deactivateDevice(Device d){  
    for(auto i = activeD.begin(); i != activeD.end(); ){
        if (i->getName() == d.getName() && i->getId() == d.getId()) {
            i = activeD.erase(i);
        }else{
            ++i;
        }
    }
}