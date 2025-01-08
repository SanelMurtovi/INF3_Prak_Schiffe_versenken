#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "SIMPLESOCKET.H"

using namespace std;

class BattleShipClient {
private:
    TCPclient client;
    string serverAddress;
    int serverPort;
    vector<pair<int, int> > shots; // Updated to use C++98 compatible syntax
    int boardSizeX;
    int boardSizeY;

public:
    // Constructor initializes connection details and board size
    BattleShipClient(string address, int port, int boardX, int boardY)
        : serverAddress(address), serverPort(port), boardSizeX(boardX), boardSizeY(boardY) {
        srand(time(NULL)); // Changed nullptr to NULL for C++98 compatibility
    }

    // This method generates random shots by shuffling all board positions randomly
    void generateRandomShots() {
        shots.clear();
        for (int x = 1; x <= boardSizeX; ++x) {
            for (int y = 1; y <= boardSizeY; ++y) {
                shots.push_back(make_pair(x, y)); // Replaced emplace_back with push_back and make_pair for C++98 compatibility
            }
        }
        random_shuffle(shots.begin(), shots.end());
    }

    // This method generates shots in a fixed sequential order
    void generateSequentialShots() {
        shots.clear();
        for (int x = 1; x <= boardSizeX; ++x) {
            for (int y = 1; y <= boardSizeY; ++y) {
                shots.push_back(make_pair(x, y)); // Replaced emplace_back with push_back and make_pair for C++98 compatibility
            }
        }
    }

    // This method generates shots in a checkerboard pattern to cover fields more efficiently
    void generateCheckerboardShots() {
        shots.clear();
        for (int x = 1; x <= boardSizeX; ++x) {
            for (int y = 1; y <= boardSizeY; ++y) {
                if ((x + y) % 2 == 0) {
                    shots.push_back(make_pair(x, y)); // Replaced emplace_back with push_back and make_pair for C++98 compatibility
                }
            }
        }
    }

    // This method plays the game using a specified shooting strategy by invoking the respective shot generation method
    int playGame(void (BattleShipClient::*strategy)()) {
        // Establish a connection to the server
        if (!client.conn(serverAddress, serverPort)) {
            cerr << "Error: Unable to connect to server." << endl;
            return -1;
        }

        // Generate shots based on the chosen strategy
        (this->*strategy)();

        int shotsFired = 0;
        for (vector<pair<int, int> >::iterator it = shots.begin(); it != shots.end(); ++it) { // Replaced range-based for loop with iterator for C++98 compatibility
            string msg = to_string(it->first) + " " + to_string(it->second);
            cout << "Sending: " << msg << endl;

            // Send the coordinates of the shot to the server
            client.sendData(msg);
            ++shotsFired;

            // Receive the server's response to the shot
            string response = client.receive(128);
            cout << "Server response: " << response << endl;

            if (response == "GAME OVER" || response == "ALL SHIPS DESTROYED") {
                cout << "Game finished!" << endl;
                break;
            }
        }

        // Send BYEBYE to terminate connection
        client.sendData("BYEBYE"); // Inform the server that the client is terminating the connection

        return shotsFired;
    }
};

int main() {
    string strategyChoice;
    int attempts;

    cout << "Select a strategy to solve the game:" << endl;
    cout << "1. Random Shots" << endl;
    cout << "2. Sequential Shots" << endl;
    cout << "3. Checkerboard Shots" << endl;
    cout << "Enter your choice (1-3): ";
    cin >> strategyChoice;

    cout << "Enter the number of attempts to perform: ";
    cin >> attempts;

    void (BattleShipClient::*selectedStrategy)() = NULL; // Changed nullptr to NULL for C++98 compatibility

    if (strategyChoice == "1") {
        selectedStrategy = &BattleShipClient::generateRandomShots;
    } else if (strategyChoice == "2") {
        selectedStrategy = &BattleShipClient::generateSequentialShots;
    } else if (strategyChoice == "3") {
        selectedStrategy = &BattleShipClient::generateCheckerboardShots;
    } else {
        cerr << "Invalid choice! Exiting." << endl;
        return 1;
    }

    BattleShipClient client("localhost", 2022, 10, 10);

    int totalShots = 0;
    for (int i = 0; i < attempts; ++i) {
        cout << "\nAttempt " << (i + 1) << ":" << endl;
        int shots = client.playGame(selectedStrategy);
        if (shots >= 0) {
            totalShots += shots;
        }
    }

    if (attempts > 0) {
        double averageShots = static_cast<double>(totalShots) / attempts;
        cout << "\nAverage shots per game: " << averageShots << endl;
    }

    return 0;
}
