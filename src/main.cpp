// @author Leonardo Sivori

#include "../include/Auto.h"
#include "../include/Clock.h"
#include "../include/Device.h"
#include "../include/House.h"
#include "../include/Id.h"
#include "../include/Manual.h"
#include <fstream>
#include <iostream>
#include <time.h>

// helper function to print both to std output and log file
void print(std::string& out, std::fstream& log) { 
    std::cout << out << std::endl;
    log << out << std::endl;
}

int main() {
    const float gridPower = 3.5;
    House impianto(gridPower);

    try {
        impianto.loadsDevices("devices.txt");
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;       // the devices.txt file could not be read -> impossible to continue
    }

    time_t currUnix;
    time(&currUnix);    // sets currUnix to current time
    tm* timeinfo;       // pointer to struct used for time formatting
    timeinfo = localtime (&currUnix);   // assigns currUnix converted to Local Time to timeinfo
    const int buffSize = 32;
    char logFileName[buffSize];
    strftime (logFileName, buffSize, "Log - %Y.%m.%d %Hh%Mm.txt", timeinfo);

    try {
        std::fstream log(logFileName, std::fstream::out);
        std::string strInput;
        std::vector<std::string> vecInput;
        while(impianto.isActive()) {
            vecInput.clear();
            std::cout <<  ">> ";
            std::getline(std::cin, strInput);
            log << "Input: " << strInput << std::endl;
            int i = 0, j = 1;

            std::string buff = "";
            for (char i : strInput) {  // split input in separate words
                if (i == ' ') {
                    vecInput.push_back(buff);
                    buff = "";
                } else {
                    buff += i;
                }
            }
            vecInput.push_back(buff);   // each word is a different entry in vecInput

            std::string out = impianto.getCurrentTime() + "\n"; // adds current time to output string as requested
            try {
                if (vecInput.at(0) == "set") {     // c++ does not support switch(std::string)
                    if (vecInput.at(1) == "time") {    
                        out += impianto.setTime(Clock(vecInput.at(2)));
                    } else if (vecInput.at(2) == "on") {
                        out += impianto.setOn(vecInput.at(1));
                    } else if (vecInput.at(2) == "off") {
                        out += impianto.setOff(vecInput.at(1));
                    } else {
                        if (vecInput.size() == 3) {
                            out += impianto.setScheduledOn(vecInput.at(1), Clock(vecInput.at(2)));
                        } else {
                            out += impianto.setScheduledOn(vecInput.at(1), Clock(vecInput.at(2)), Clock(vecInput.at(3)));
                        }
                    }
                } else if (vecInput.at(0) == "rm") {
                    out += impianto.remove(vecInput.at(1));
                } else if (vecInput.at(0) == "show") {
                    if (vecInput.size() == 1) {
                        out += impianto.show();
                    } else {
                        out += impianto.show(vecInput.at(1));
                    }
                } else if (vecInput.at(0) == "reset") {
                    if (vecInput.at(1) == "time") {    
                        out += impianto.resetTime();
                    } else if (vecInput.at(1) == "timers") {    
                        out += impianto.resetTimers();
                    } else if (vecInput.at(1) == "all") {    
                        out += impianto.resetAll();
                    } else {
                        throw std::invalid_argument("");
                    }
                } else {
                    throw std::invalid_argument("");
                }

                print(out, log);
            } catch (...) {
                out = "Input invalido, inserisci un nuovo comando\n";
                print(out, log);
            }
        }    
        std::string endMsg = "Fine Giornata!";
        print(endMsg, log);

        log.close(); // file gets closed anyway when the 'log' object is destroyed
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
