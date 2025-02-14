#include <string>
#include <iostream>
#include <unistd.h> //contains various constants
#include <random>
#include <set>
#include <fstream>

#include "SIMPLESOCKET.H"

using namespace std;


TCPclient c;

int tactic1();
int tactic2();
int tactic3();


unsigned char attemps = 0;
unsigned int tries = 100;


int main(){
    //Client Setup
    cout << "Client initiated" << endl;
    srand(time(NULL));
    string host = "localhost";

    unsigned char atmpsArr[tries];

    cout << "Establishing Connection ..." << endl;
    c.conn(host, 2027);

    //Statistical Data File
    ofstream StatisticalData;
    StatisticalData.open("StatisticalData.csv");

    if(StatisticalData.is_open()){
        cout << "The File for the saving of statistical Data was openend sucessfully, Proceeding ..." << endl;
    }
    else{
        cout << "Unexpected Error opening the file" << endl;
    }

    //Strategy Request
    int strategy;
    cout << "What Strategy should be statistically analysed ?:" << endl;
    cout << "1. Strategy 1" << endl;
    cout << "2. Strategy 2" << endl;
    cout << "3. Strategy 3" << endl;
    cin >> strategy;

    switch(strategy){
        case 1: cout << "Strategy 1 has been choosen" << endl;
                break;
        case 2: cout << "Strategy 2 has been choosen" << endl;
                break;
        case 3: cout << "Strategy 3 has been choosen" << endl;
                break;
        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 0;
        }

     for(int n = 1; n<=tries; n++){
        atmpsArr[n] = 0;
        c.sendData("NEWGAME");
        string msg = c.receive(32);
        int currRun = 0;

        switch(strategy){
        case 1: currRun = tactic1();
                break;
        case 2: currRun = tactic2();
                break;
        case 3: currRun = tactic3();
                break;
        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 0;
        }
        atmpsArr[n] = currRun;
        if(atmpsArr[n] == 0){
            cout << "ERROR! No Strategy is implemented, terminating ..." << endl;
            sleep(15);
            return 0;
        }
        StatisticalData << n << ";" << currRun << endl;
        cout << "Nmb" << n << ": " << to_string(atmpsArr[n]) << endl;
    }

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



int tactic1(){
    string msg;
    int attemps = 0;
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            attemps++;
            msg = "COORD[" + to_string(j) + ";" + to_string(i) + "]";
            c.sendData(msg);
            msg = c.receive(32);
            if(msg == "ALL SHIPS DESTROYED, TERMINATING SESSION"){
                return attemps;
            }
        }
    }
        return -1;
}

int tactic2(){
    return 0;
}

int tactic3(){
    return 0;
}



/*
int main() {
	srand(time(NULL));
	TCPclient c;
	string host = "localhost";
	string msg;

	//connect to host
	c.conn(host , 2022);

	int i=0;
	bool goOn=1;
	while(goOn){ // send and receive data
		if((rand() % 20) < i++){
			msg = string("BYEBYE");
			goOn = 0;
		}else{
			msg = string("client wants this");
		}
		cout << "client sends:" << msg << endl;
		c.sendData(msg);
		msg = c.receive(32);
		cout << "got response:" << msg << endl;
		sleep(1);

	}
}
*/

