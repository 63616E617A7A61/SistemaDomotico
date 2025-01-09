# SistemaDomotico
[Specifica](./Progetto Finale - Sistema domotico - v2.pdf)

## Note per l'esecuzione
 - Il programma va eseguito dalla cartella in cui è presente devices.txt
 - Esso genera ad ogni esecuzione un file di Log dal nome: "Log - [anno].[mese].[giorno] [ore]h[minuti]m.txt"
 - Quando un orario viene impostato per lo spegnimento col comando set ```${DEVICENAME} ${START} [${STOP}]``` , il dispositivo ricorda l'orario di accensione e la durata della sua accensione (STOP - START). Per noi "timer" è questa durata.
 - Risolta politica di spegnimento in fase di sovraccarico per evitare lo spegnimento di dispositivi necessari. Es: Frigorifero. A questo scopo l'ultimo valore nella entry di ogni dispositivo in devices.txt è un valore booleano:
    - 0 = dispositivo non necessario
    - 1 = dispositivo necessario.