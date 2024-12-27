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
                if (vecInput[0] == "set") {     //c++ non supporta switch case con le string. se lo riteniamo necessario troverò una soluzione più carina
                    if (vecInput[1] == "time") {    

                    }
                } else if (vecInput[0] == "rm") {

                } else if (vecInput[0] == "show") {

                } else if (vecInput[0] == "reset") {
                    if (vecInput[1] == "time") {    

                    } else if (vecInput[1] == "timers") {    

                    } else if (vecInput[1] == "all") {    

                    }
                } else {
                    throw std::invalid_argument("Input invalido riprova");
                }

                print(out, log);
            } catch (const std::invalid_argument& e) {
                std::cout << e.what() << std::endl;
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