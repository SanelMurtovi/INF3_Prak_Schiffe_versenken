#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "TASK3.H"
#include "SIMPLESOCKET.H"

using namespace std;

// This is the main class for the server, inheriting from TCPserver and implementing the game logic
class BattleShipServer : public TCPserver {
private:
    TASK3::World gameWorld; // The game world object that manages the board and game state

public:
    // The constructor initializes the server and creates a new game board with defined dimensions and ship count
    BattleShipServer(int port, int maxDataSizeRecv) : TCPserver(port, maxDataSizeRecv), gameWorld(10, 10, 1, 2, 3, 4) {
        gameWorld.printBoard(); // Print the initial game board for visualization
    }

protected:
    // This method processes incoming messages from the client and returns the appropriate response
    string myResponse(string input) {
        int x, y;
        if (input == "BYEBYE") { // Check if the client wants to terminate the connection
            return "BYEBYE";
        }

        // Parse coordinates from the input string and check their validity
        if (sscanf(input.c_str(), "%d %d", &x, &y) != 2) {
            return "ERROR: Invalid input"; // Return an error if parsing fails
        }

        // Execute a shot on the game board and store the result
        TASK3::ShootResult result = gameWorld.shoot(x, y);

        // Based on the shot result, generate the corresponding response
        switch (result) {
            case TASK3::WATER:
                return "MISS";
            case TASK3::SHIP_HIT:
                return "HIT";
            case TASK3::SHIP_DESTROYED:
                return "SHIP DESTROYED";
            case TASK3::ALL_SHIPS_DESTROYED:
                return "ALL SHIPS DESTROYED";
            case TASK3::GAME_OVER:
                return "GAME OVER";
            default:
                return "UNKNOWN RESULT";
        }
    }
};

int main() {
    srand(time(NULL)); // Changed nullptr to NULL for C++98 compatibility
    BattleShipServer server(2022, 128); // Create the server on port 2022 with a buffer size of 128
    server.run(); // Start the server to listen and process incoming connections
    return 0;
}
