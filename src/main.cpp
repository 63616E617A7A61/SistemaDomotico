#include "../include/Auto.h"
#include "../include/Clock.h"
#include "../include/Device.h"
#include "../include/House.h"
#include "../include/Id.h"
#include "../include/Manual.h"
#include <fstream>
#include <iostream>
#include <time.h>

void print(std::string& out, std::fstream& log) { //funzione di utility, stampa l'output a schermo e lo salva nel file di log
    std::cout << out << std::endl;
    log << out;
}

int main() {
    const int gridPower = 3.5; //da stabilire se questo valore lo teniamo fisso o lo faccimao inserire all'utente
    House impianto(gridPower);

    std::string logFileName = ".txt";
    time_t currUnix;    //crea oggetto capace di contenere il tempo
    time(&currUnix);    //setta currUnix al orario corrente
    logFileName = ctime(&currUnix) + logFileName;   //setta il nome del file di log

    try {
        std::fstream log(logFileName, std::fstream::out);
        std::string strInput;
        std::vector<std::string> vecInput;
        while(impianto.isActive()) {
            std::getline(std::cin, strInput);   //ottiene in input la riga intera
            log << "Input: " << strInput;       //la salva in una riga del file
            int i = 0, j = 1;
            while(j <= strInput.length()) {
                if(strInput[j] == ' ' || j == strInput.length()) {
                    vecInput.push_back(strInput.substr(i, j));  //la separa in parole
                    j++;
                    i = j++;
                } else {
                    j++;
                }
            }

            try {
                std::string out;
                if (vecInput.at(0) == "set") {     //c++ non supporta switch case con le string. se lo riteniamo necessario troverò una soluzione più carina
                    if (vecInput.at(1) == "time") {    
                        impianto.setTime(Clock(vecInput.at(2)));
                    } else if (vecInput.at(2) == "on") {
                        impianto.setOn(vecInput.at(1));
                    } else if (vecInput.at(2) == "off") {
                        impianto.setOff(vecInput.at(1));
                    } else {
                        if (vecInput.size() == 3) {
                            impianto.setScheduledOn(vecInput.at(1), Clock(vecInput.at(2)));
                        } else {
                            impianto.setScheduledOn(vecInput.at(1), Clock(vecInput.at(2)), Clock(vecInput.at(3)));
                        }
                    }
                } else if (vecInput.at(0) == "rm") {
                    impianto.remove(vecInput.at(1));
                } else if (vecInput.at(0) == "show") {
                    if (vecInput.size() == 1) {
                        impianto.show();
                    } else {
                        impianto.show(vecInput.at(1));
                    }
                } else if (vecInput.at(0) == "reset") {
                    if (vecInput.at(1) == "time") {    
                        impianto.resetTime();
                    } else if (vecInput.at(1) == "timers") {    
                        impianto.resetTimers();
                    } else if (vecInput.at(1) == "all") {    
                        impianto.resetAll();
                    } else {
                        throw std::invalid_argument("");
                    }
                } else {
                    throw std::invalid_argument("");
                }

                print(out, log);
            } catch (...) {
                std::cout << "Input invalido\n, inserisci un nuovo comando\n";
            }
        }    
        std::string endMsg = "Fine Giornata!";
        print(endMsg, log);

        log.close(); //in caso di errore il file viene comunque chiuso quando l'oggetto viene distrutto
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}