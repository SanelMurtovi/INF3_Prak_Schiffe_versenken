#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time
#include <sstream>

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.H"
#include "TASK3.H"
#include "TASK3.C"

using namespace std;

class myServerShip : public TCPserver{

public:

    myServerShip(int portNmb, int bSize):TCPserver(portNmb, bSize){
    world = new TASK3::World();
    cout <<"Server started!" << endl;
    cout << "The board is 10x10 in size and contains:" << endl << "1x ship with length 5, 2x length 4, 3x length 3 and 4x length 2" << endl;
    }
    ~myServerShip(){ delete world;}

private:
    myServerShip();
    TASK3::World* world = nullptr;

protected:

string myResponse(string input)override{

    if(input.compare(0,7,"NEWGAME") == 0){
        delete world;
        world = new TASK3::World();
        return string("NEWGAME");
   }

    else if(input.compare(0,6,"COORD[")== 0){
        int x=-1, y=-1;

    if (sscanf(input.c_str(), "COORD[%d;%d]", &x, &y) !=2){
        return "ERROR: Invalid input format: " + input;
 }
    TASK3::ShootResult result = world -> shoot(x,y);

    cout << "-----------------------------------" << endl;
    cout << "Shot on coordinates (" << x << "," << y << "):" << endl;
    cout << "Board after this shot:" << endl;
    world -> printBoard();
    cout << "-----------------------------------" << endl << endl;

    switch (result){
        case TASK3::WATER:
            return "MISS";
        case TASK3::SHIP_HIT:
            return "HIT";
        case TASK3::SHIP_DESTROYED:
            return "SHIP DESTROYED";
        case TASK3::GAME_OVER:
            /* Ueberpruefen, wie das Spielfeld im Endstand aussieht
            cout << "-----------------------------------" << endl;
            cout << "ALL SHIPS HIT - FINAL RESULT:" << endl;
            world -> printBoard();
            cout << "-----------------------------------" << endl << endl;
            */
            return "GAME OVER";
        default:
            return "ERROR, UNKNOWN RESULT";

        }
        }
    else {
        return "ERROR: Invalid input format: " + input;
        }
    }
};




int main(){
	srand(time(nullptr));
	myServerShip srv(2027,25); //Instanziiert mit den parametriseriten Konstruktor
	srv.run();
	return 0;
}



