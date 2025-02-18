#include <string>
#include <iostream>
#include <unistd.h> //contains various constants
#include <random>
#include <set>
#include <fstream>
#include <ctime>    //time(NULL)
#include <cstdlib> //rand(), srand()
#include <queue> //queue fuer Zerstoerung in tactic4

#include "SIMPLESOCKET.H"

using namespace std;


TCPclient c;

int tactic1();  //Zeilenweiser Durchgang
int tactic2();  //Durchgang im Schachbrett Muster
int tactic3();  //Zufaellige Reihenfolge
int tactic4();  //Zufaelliges Suchen und Zerstoeren


unsigned int tries = 1000; //Anzahl der Durchlaeufe

//Hilfsfunktion fuer den Fisher-Yates Shuffle fuer tactic3
void FYShuffle(int arr[], int size);
int getX(int idx); //Konvertiert idx in x
int getY(int idx); //Konvertiert idx in y


int main(){
    //Client Setup
    cout << "Client initiated" << endl;

    //Fuer die tactic3 und die dazugehoerige Shuffle Funktion
    srand(time(NULL));

    // Verbindung zum Server wird hergestellt
    string host = "localhost";
    cout << "Establishing Connection ..." << endl;
    c.conn(host, 2027);

    //CSV Datei fuer die statistische Auswertung
    ofstream StatisticalData;
    StatisticalData.open("StatisticalData.csv");

    if(StatisticalData.is_open()){
        cout << "The File for the saving of statistical Data was openend sucessfully, Proceeding ..." << endl;
    }
    else{
        cout << "Unexpected Error opening the file" << endl;
    }

    //Auswaehlen der Strategie
    int strategy;
    cout << "What Strategy should be statistically analysed ?:" << endl;
    cout << "1. Strategy 1 (line by line Pattern)" << endl;
    cout << "2. Strategy 2 (Chessboard Pattern)" << endl;
    cout << "3. Strategy 3 (Random Pattern)" << endl;
    cout << "4. Strategy 4 (Random + Search and Destroy)" << endl;
    cin >> strategy;

    switch(strategy){
        case 1: cout << "Strategy 1 was choosen" << endl;
                break;
        case 2: cout << "Strategy 2 was choosen" << endl;
                break;
        case 3: cout << "Strategy 3 was choosen" << endl;
                break;
        case 4: cout << "Strategy 4 was choosen" << endl;
                break;
        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 0;
        }

    // Durchgang von mehreren Spielen
    int atmpsArr[1000];
    for(int n = 0; n < (int)tries; n++){
        atmpsArr[n] = 0;

        //Anfrage an den Server fuer ein neues Spiel
        c.sendData("NEWGAME");
        string msg = c.receive(32); //Empfang
        //Erwartet wird "NEWGAME"

        int currRun = 0;

        switch(strategy){
        case 1: currRun = tactic1();
                break;
        case 2: currRun = tactic2();
                break;
        case 3: currRun = tactic3();
                break;
        case 4: currRun = tactic4();
                break;
        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 1;
        }
        atmpsArr[n] = currRun;

        //Fehler-Abfang
        if(atmpsArr[n] == 0){
            cout << "ERROR! No Strategy is implemented, terminating ..." << endl;
            sleep(15);
            return 0;
        }
        StatisticalData << (n+1) << ";" << currRun << endl;
        cout << "Game " << (n+1) << ": needed " << to_string(atmpsArr[n]) << "attemps" << endl;
    }

    //Schließen der CSV Datei
    StatisticalData.close();
    if(StatisticalData.is_open() == false){
            cout << "Statistical Data exported" << endl;
    }
    else{
            cout << "Unknown Error while trying to export Statistical Data" << endl;
    }

    sleep(50);

    return 0;

}

/*
- tactic1:
- Geht das Feld zeilenweise durch
- Zum Beispiel Zeile y = 1..10, Spalte x = 1..10
*/

int tactic1(){
    string msg;
    int attemps = 0;
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            attemps++;
            msg = "COORD[" + to_string(j) + ";" + to_string(i) + "]";
            c.sendData(msg);
            msg = c.receive(32);
            if(msg == "GAME OVER"){
                return attemps;
            }
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
        return -1;
}

/*
- tactic2:
- Geht das Feld im Schachbrettmuster durch
- Erst Felder (x,y) mit (x+y)%2 == 0
- Dann Felder (x,y) mit (x+y)%2 == 1
*/

int tactic2(){
    string msg;
    int attemps = 0;

    //Erster Durchgang: (x+y) % 2 == 0
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            if(((i + j) % 2) == 0){
                attemps++;
                msg = "COORD[" + to_string(j) + ";" + to_string(i) + "]";
                c.sendData(msg);

                msg = c.receive(64);
                if(msg == "GAME OVER"){
                    return attemps;
                }
            }
        }
    }

    //Zweiter Durchgang: (x+y) % 2 == 1
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            if(((i + j) % 2) == 1){
                attemps++;
                msg = "COORD[" + to_string(j) + ";" + to_string(i) + "]";
                c.sendData(msg);

                msg = c.receive(64);
                if(msg == "GAME OVER"){
                    return attemps;
                }
            }
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
    return -1;
}

/*
- tactic3:
- Zufaellige Reihenfolge aller Felder(1..10,1..10)
- Es wird definiert:
- idx = (y-1)*10 + (x-1) -> also von 0 bis 99
- x = (idx % 10) +1
- y = (idx / 10) +1
- Folgend wird das Array mit dem FY-Shuffle gemischt
*/

int tactic3(){
    //1-D Array mit den Werten 0 bis 99
    int field1[100];
    for(int i = 0; i < 100; i++){
        field1[i] = i;
    }

    //FY-Shuffle fuer das 1-D Array
    FYShuffle(field1, 100);

    int attempts = 0;
    for(int i = 0; i < 100; i++){
        //idx => (x,y)
        int idx = field1[i];
        int x = getX(idx); //Spalte
        int y = getY(idx); //Zeile

        attempts++;
        string msg = "COORD[" + to_string(x) + ";" + to_string(y) + "]";
        c.sendData(msg);

        string reply = c.receive(64);
        if(reply == "GAME OVER"){
            return attempts;
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
    return -1;
}

/*
- tactic4:
- Suchen erfolgt nach "Zufall" wie bei tactic3
- Zerstoeren besteht aus Schuesse um den Treffer bis SHIP DESTROYED
- Folgend wieder Uebergang zu Suchen
*/

int tactic4(){
    //2-D Array, welches die Koordinaten makiert die schon beschossen wurden
    bool visited[11][11];
    for(int y = 1; y <= 10; y++){
        for(int x = 1; x <= 10; x++){
            visited[y][x] = false;
        }
    }

    //1-D Array mit "Zufalls" Reihenfolge aller Felder 0 bis 99
    //Gleiches Prinzip wie in tactic3

    int field2[100];
    for(int i = 0; i < 100; i++){
        field2[i] = i; //Feldindexe werden spaeter in die Koordinaten (x,y) umgerechnet
    }

    FYShuffle(field2, 100);

    int attempts = 0;

    //Hilffunktion zum Schiessen
    //Lambda Funktion, die als Parameter (xx,yy) annimmt und einen string zurueckgibt
    //& im Capture bedeutet, dass die Funktion auf Variablen im aeusseren Scope zugreifen kann (visited, attempts und c)
    auto shoot = [&](int xx, int yy)->string{
        visited[yy][xx] = true; //Markiert das Feld als beschossen
        attempts++;
        string s = "COORD[" + to_string(xx) + ";" + to_string(yy) + "]";
        c.sendData(s); //Schickt Befehl an den Server
        return c.receive(64); //Wartet auf antwort und gibt diese als Rueckgabewert des Lambdas zurueck
    };

    //Zerstoeren Hilfsfunktion => Schiessen um die Koordinaten (x,y)
    //Weitere Lambda Funktion, gleiches Prinzip wie bei "auto shoot" im generellen
    //"q" ist die Warteschlange und "sx" / "sy" sind die aktuellen Koordinaten von denen die Nachbarn hinzugefuegt werden
    auto addNeighbors = [&](queue<pair<int,int>>&q, int sx, int sy){
        static int dx[4] = {0,1,0,-1};//Definition der Offsets fuer oben und unten(Wo die Nachbarn liegen)
        static int dy[4] = {-1,0,1,0};
        for(int i = 0; i < 4; i++){
            int fx = sx + dx[i]; //Berechnung der Nachbarkoordinaten
            int fy = sy + dy[i];
            if(fx >= 1 && fx <= 10 && fy >= 1 && fy <= 10){ //Grenzpruefung, da nur Koordinaten im 10x10 Bereich zulaessig sind
                if(!visited[fy][fx]){ //Sofern die Koordinaten noch nicht beschossen wurden, werden diese in die Warteschlange eingefuegt
                    q.push({fx,fy});
                }
            }
        }
    };

    //Suchen indem ueber die zufaelligen field2 iteriert wird
    //Sobald ein "HIT" registriert wird, wird zu "Zerstoeren" gewechselt
    for(int i = 0; i < 100; i++){
    //Iteration ueber die zufaellige Reihenfolge im Feld
    //Anschliessend werden die Indexe in die Koordinaten umgerechnet
        int idx = field2[i];
        int x = getX(idx);
        int y = getY(idx);

        if(!visited[y][x]){
            //Schiessen auf Felder die noch nicht beschossen worden sind
            string r = shoot(x,y); //r enthaelt die Antwort des Servers und ruft das obige Lambda auf
            if(r == "GAME OVER"){
                return attempts;
            } else if(r == "HIT" || r == "SHIP_HIT" || r == "SHIP DESTROYED"){
                //Zerstoeren Modus
                //Falls nicht gleich "SHIP DESTROYED" werden Nachbarn abgesucht
                bool DESTR = false; //Bool fuer Zerstoert Zustand
                queue<pair<int,int>> toCheck; //Warteschlange "toCheck"
                if(r != "SHIP DESTROYED"){
                    addNeighbors(toCheck,x,y);
        //Wenn das Schiff nicht direkt zerstoert wurde, werden die Nachbarfelder in die Warteschlange "toCkeck" eingefuegt
                }
                while(!toCheck.empty() && !DESTR){ //Es wird solange geschossen bis die Warteschlange leer oder das Schiff versenkt ist
                    auto front = toCheck.front(); //Das erste element aus der Warteschlange wird geholt
                    toCheck.pop();
                    int fx = front.first; //Koordinaten fuer den naechsten Schuss
                    int fy = front.second;
                    if(!visited[fy][fx]){ //Nur schiessen wo noch nicht geschossen wurde
                        string r2 = shoot(fx,fy); //Wird beschossen und Antwort "r2" wird erhalten
                        if(r2 == "GAME OVER"){
                            return attempts;
                        } else if(r2 == "HIT" || r2 == "SHIP_HIT"){ //Falls ein weitere Treffer erzielt wird, wird die Warteschlange erweitert
                            addNeighbors(toCheck,fx,fy);
                        }
                        // FALLS "MISS" nichts tun
                    }
                }
                //Nach Zerstoerung zueruck zur Suche
            }
        }
    }

    //Fehlerabfang, falls nie alle Schiffe versenkt werden
    return -1;

}

/*
Lambda Funktionen sind kleinere Einheiten die man im Code definierne kann ohne, dass man eine freie Funkton/Methode schreiben muss
Dies Funktioniert nach dem Grundgeruest
[capture] (parameter) -> returnType {Funktionstext};
[capture] => [&] bedeutet das Erfassen aller im Lambda verwendeten Variablen "by reference" aus dem umgebenden Gueltigkeitsbereich
Ohne capture muesste man diese Variablen im parameternamen oder als globale Variablen definieren
*/

/*
Die Warteschlange funktioniert nach dem Prinzip FIFO(first in, first out)
-push() legt ein neues Element an das Ende
-front() gibt das vorderste Element
-pop() entfernt das vorderste Element aus der Warteschlange
*/

/*
- Fisher-Yates Shuffle
- basierend auf rand() und srand(time(NULL))
*/

void FYShuffle(int arr[], int size){
    for(int i = size -1; i > 0; i--){
        //Zufallsindex j in [0..i]
        int j = rand() % (i+1);
        //vertauschen
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Umrechnen idx in x
int getX(int idx){
    // idx=0..99 => x =(idx%10)+1
    return (idx%10)+1;
}

// Umrechnen idx in y
int getY(int idx){
    // idx=0..99 => x =(idx%10)+1
    return (idx/10)+1;
}

