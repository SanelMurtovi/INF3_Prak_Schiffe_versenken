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
    cout <<"Server gestartet!" << endl;
    cout << "Das Spielfeld ist dimensioniert (10x10), bestehend aus folgenden Schifflaengen: 1xFuenf, 2xVier, 3xDrei und 4xZwei"<<endl;
    }
    ~myServerShip(){};

private:
    myServerShip();
    TASK3::World *world;

protected:

string myResponse(string input){
    int x = -1,y = -1;
    stringstream o;

    if(input.compare(0,7,"NEWGAME") == 0){
        delete world;
        world = new TASK3::World();
        return string("NEWGAME");
   }

    if(sscanf(input.c_str(), "%d %d", &x, &y) !=2)
    return string ("ERROR: Invalid input");

    TASK3::ShootResult result = world -> shoot(x,y);

    switch (result){
        case TASK3::WATER:
            return "MISS";
        case TASK3::SHIP_HIT:
            return "HIT";
        case TASK3::SHIP_DESTROYED:
            return "SHIP DESTROYED";
        case TASK3::ALL_SHIPS_DESTROYED:
            return "ALL SHIPS DESTROYED, TERMINATING SESSION";
        default:
            return "ERROR, UNKNOWN RESULT";

        }
}

};




/*myServerShip::~myServerShip(){
}

myServerShip::myServerShip(int portNmb, int bSize) : TCPserver(portNmb, bSize){

}*/

int main(){
	srand(time(nullptr));
	myServerShip srv(2027,25); //Instanziiert mit den parametriseriten Konstruktor
	srv.run();
}


/*protected:
string myResponse(string input) override {
    int x= -1, y = -1;

    if(input.compare(0,7,"NEWGAME")==0){
    delete world;
    world = new TASK3::World();
    return string("DONE");
    }

    if(input.compare(0,6,"COORD[")==0)[
        if (sscanf (input.c str(), "COORD[%d,%d], &x, &y) !=2){
            return string("Error. Could not read coord-data.");
            }

 if(input.compare(0,6,"COORD[") == 0){
        e = sscanf(input.c_str(),"COORD[%i,%i]",&x,&y);
        if(e != 2){
            return string("COULD NOT READ COORDINATES.");
        }
        o << "SUMME[" << (x+y) << "]";
        return (o.str());
    }

    return string("#"+input+"#");
}
}*/
