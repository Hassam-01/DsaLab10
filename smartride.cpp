#include <iostream>
#include <string>
#include <stdexcept>
#include <limits> // For std::numeric_limits
#include <fstream>

using namespace std;

// Node for Ride History (Doubly Linked List)
struct RideHistoryNode {
    string pickupLocation;
    string destination;
    string status; // Pending or Accepted
    RideHistoryNode* next;
    RideHistoryNode* prev;

    RideHistoryNode(string pickup, string dest, string stat)
        : pickupLocation(pickup), destination(dest), status(stat), next(nullptr), prev(nullptr) {}
};

// Node for Linked List
struct Node {
    string id; // ID for user or driver
    string name;
    string password;
    RideHistoryNode* rideHistoryHead; // Head of ride history list
    RideHistoryNode* rideHistoryTail; // Tail of ride history list
    Node* next;

    Node(string i, string n, string p)
        : id(i), name(n), password(p), rideHistoryHead(nullptr), rideHistoryTail(nullptr), next(nullptr) {}
};

// Linked List for User and Driver Management
class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}

    void addRideToHistory(Node* user, string pickup, string destination, string status) {
        RideHistoryNode* newRide = new RideHistoryNode(pickup, destination, status);
        if (!user->rideHistoryTail) {
            user->rideHistoryHead = newRide;
            user->rideHistoryTail = newRide;
        } else {
            user->rideHistoryTail->next = newRide;
            newRide->prev = user->rideHistoryTail;
            user->rideHistoryTail = newRide;
        }
    }

    void saveToFile(const string& filename) {
        ofstream file(filename, ios::out);
        if (!file) {
            cerr << "Error: Could not open file " << filename << " for saving.\n";
            return;
        }
        Node* current = head;
        while (current) {
            file << current->id << "," << current->name << "," << current->password << "\n";
            
            // Save ride history
            RideHistoryNode* history = current->rideHistoryHead;
            while (history) {
                file << "RIDE," << history->pickupLocation << "," << history->destination << "," << history->status << "\n";
                history = history->next;
            }

            current = current->next;
        }
        file.close();
    }

    // Load the linked list from a file
    void loadFromFile(const string& filename) {
        ifstream file(filename, ios::in);
        if (!file) {
            cerr << "Error: Could not open file " << filename << " for loading.\n";
            return;
        }
        string line;
        Node* current = nullptr;

        while (getline(file, line)) {
            if (line.substr(0, 5) == "RIDE,") {
                // Ride history entry
                if (!current) continue; // Skip if no user/driver context
                string pickup, destination, status;
                size_t pos1 = line.find(',', 5);
                size_t pos2 = line.find(',', pos1 + 1);
                pickup = line.substr(5, pos1 - 5);
                destination = line.substr(pos1 + 1, pos2 - pos1 - 1);
                status = line.substr(pos2 + 1);

                addRideToHistory(current, pickup, destination, status);
            } else {
                // User/Driver entry
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string id = line.substr(0, pos1);
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string password = line.substr(pos2 + 1);

                add(id, name, password);
                current = head; // Update current context for ride history
            }
        }
        file.close();
    }

    void add(string id, string name, string password) {
        Node* newNode = new Node(id, name, password);
        newNode->next = head;
        head = newNode;
    }

    Node* login(string id, string password) {
        Node* current = head;
        while (current) {
            if (current->id == id && (password.empty() || current->password == password)) {
                return current;
            }
            current = current->next;
        }
        return nullptr; // User not found
    }

    void printAllUsers() {
        Node* current = head;
        while (current) {
            cout << "User: " << current->id << "\n";
            current = current->next;
        }
    }


    bool idExists(string id) {
        Node* current = head;
        while (current) {
            if (current->id == id) {
                return true; // ID exists
            }
            current = current->next;
        }
        return false; // ID does not exist
    }
};

// Ride Request Structure
struct RideRequest {
    string userId;
    string pickupLocation;
    string destination;

    RideRequest(string u, string p, string d) : userId(u), pickupLocation(p), destination(d) {}
};

// Queue for Ride Requests
class RideQueue {
private:
    struct RequestNode {
        RideRequest request;
        RequestNode* next;

        RequestNode(RideRequest r) : request(r), next(nullptr) {}
    };

    RequestNode* front;
    RequestNode* rear;

public:
    RideQueue() : front(nullptr), rear(nullptr) {}

    // Save the ride queue to a file
    void saveToFile(const string& filename) {
        ofstream file(filename, ios::out);
        if (!file) {
            cerr << "Error: Could not open file " << filename << " for saving.\n";
            return;
        }
        RequestNode* current = front;
        while (current) {
            file << current->request.userId << "," 
                << current->request.pickupLocation << "," 
                << current->request.destination << "\n";
            current = current->next;
        }
        file.close();
    }

    // Load the ride queue from a file
    void loadFromFile(const string& filename) {
        ifstream file(filename, ios::in);
        if (!file) {
            cerr << "Error: Could not open file " << filename << " for loading.\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            string userId = line.substr(0, pos1);
            string pickup = line.substr(pos1 + 1, pos2 - pos1 - 1);
            string destination = line.substr(pos2 + 1);
            enqueue(RideRequest(userId, pickup, destination));
        }
        file.close();
    }

    void enqueue(RideRequest request) {
        RequestNode* newNode = new RequestNode(request);
        if (rear) {
            rear->next = newNode;
        } else {
            front = newNode;
        }
        rear = newNode;
    }

    void displayAllRides() {
        if (!front) {
            cout << "No available rides.\n";
            return;
        }
        RequestNode* current = front;
        int index = 1;
        while (current) {
            cout << index << ". User ID: " << current->request.userId
                    << ", Pickup: " << current->request.pickupLocation
                    << ", Destination: " << current->request.destination << endl;
            current = current->next;
            index++;
        }
    }
    

    RideRequest dequeueAtIndex(int index) {
        if (!front) throw runtime_error("No rides in the queue.");

        RequestNode* temp = front;
        RequestNode* prev = nullptr;
        int currentIndex = 1;

        while (temp && currentIndex < index) {
            prev = temp;
            temp = temp->next;
            currentIndex++;
        }

        if (!temp) throw runtime_error("Invalid index.");

        RideRequest request = temp->request;
        if (prev) {
            prev->next = temp->next;
        } else {
            front = temp->next;
        }

        if (temp == rear) rear = prev;
        delete temp;
        return request;
    }

    bool isEmpty() {
        return front == nullptr;
    }
};

// Main System
class SmartRide {
private:
    LinkedList users;
    LinkedList drivers;
    RideQueue rideQueue;

    // Add these methods
    void saveData() {
        users.saveToFile("users.txt");
        drivers.saveToFile("drivers.txt");
        rideQueue.saveToFile("rides.txt");
    }

    void loadData() {
        users.loadFromFile("users.txt");
        drivers.loadFromFile("drivers.txt");
        rideQueue.loadFromFile("rides.txt");
    }

    void addRideToHistory(Node* user, string pickup, string destination, string status) {
        RideHistoryNode* newRide = new RideHistoryNode(pickup, destination, status);
        if (!user->rideHistoryTail) {
            user->rideHistoryHead = newRide;
            user->rideHistoryTail = newRide;
        } else {
            user->rideHistoryTail->next = newRide;
            newRide->prev = user->rideHistoryTail;
            user->rideHistoryTail = newRide;
        }
    }

    void updateRideStatus(Node* user, string pickup, string destination) {
        RideHistoryNode* current = user->rideHistoryHead;
        while (current) {
            if (current->pickupLocation == pickup && current->destination == destination && current->status == "Pending") {
                current->status = "Accepted";
                return;
            }
            current = current->next;
        }
    }

    void viewRideHistory(Node* user) {
        if (!user->rideHistoryHead) {
            cout << "No ride history available.\n";
            return;
        }

        cout << "--- Ride History ---\n";
        RideHistoryNode* current = user->rideHistoryHead;
        while (current) {
            cout << "Pickup: " << current->pickupLocation
                << ", Destination: " << current->destination
                << ", Status: " << current->status << endl;
            current = current->next;
        }
    }


public:
    SmartRide() {
        loadData(); // Load data at initialization
    }

    ~SmartRide() {
        saveData(); // Save data at termination
    }
    void registerUser(string id, string name, string password) {
        while (users.idExists(id) || drivers.idExists(id)) {
            cout << "ID already exists. Please enter a unique ID: ";
            cin >> id;
        }
        users.add(id, name, password);
        cout << "User registered successfully!\n";
    }

    void registerDriver(string id, string name, string password) {
        while (users.idExists(id) || drivers.idExists(id)) {
            cout << "ID already exists. Please enter a unique ID: ";
            cin >> id;
        }
        drivers.add(id, name, password);
        cout << "Driver registered successfully!\n";
    }
    

    Node* loginUser(string id, string password) {
        Node* user = users.login(id, password);
        if (user) {
            cout << "User logged in successfully!\n";
        } else {
            cout << "Invalid credentials or account does not exist!\n";
        }
        return user;
    }

    Node* loginDriver(string id, string password) {
        Node* driver = drivers.login(id, password);
        if (driver) {
            cout << "Driver logged in successfully!\n";
        } else {
            cout << "Invalid credentials or account does not exist!\n";
        }
        return driver;
    }

    void requestRide(string userId) {
        string pickup, destination;
        cin.ignore();
        cout << "Enter Pickup Location: ";
        getline(cin, pickup);
        cout << "Enter Destination: ";
        getline(cin, destination);

        Node* user = users.login(userId, ""); // Fetch user
        if (user) {
            rideQueue.enqueue(RideRequest(userId, pickup, destination));
            addRideToHistory(user, pickup, destination, "Pending");
            cout << "Ride requested successfully and added to history!\n";
        } else {
            cout << "Error: User not found. Verify that the user is logged in and exists in the system.\n";
            users.printAllUsers(); // Debugging
        }
    }
    void acceptRide() {
        if (rideQueue.isEmpty()) {
            cout << "No rides available to accept.\n";
            return;
        }

        cout << "--- Available Rides ---\n";
        rideQueue.displayAllRides();

        int choice;
        cout << "Enter the number of the ride to accept: ";
        cin >> choice;

        try {
            RideRequest request = rideQueue.dequeueAtIndex(choice);
            cout << "Ride accepted!\n";
            cout << "User ID: " << request.userId
                 << ", Pickup: " << request.pickupLocation
                 << ", Destination: " << request.destination << endl;

            Node* user = users.login(request.userId, ""); // Find user
            if (user) {
                updateRideStatus(user, request.pickupLocation, request.destination);
            }
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }

    void showRideHistory(Node* user) {
        viewRideHistory(user);
    }
};

// * fucntiom to check if the entered id(string) is numeric
bool checkID(string id) {
    for (char c : id) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Main Function
int main() {
    SmartRide system;
    int choice, subChoice;
    string id, name, password;

    while (true) {
        cout << "\n--- SmartRide Home ---\n";
        cout << "1. Register User\n2. Register Driver\n3. Login User\n4. Login Driver\n5. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            // Input is not an integer
            cout << "Invalid input. Please enter a number from 1-5.\n";
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            continue; // Return to the main menu
        }

        switch (choice) {
            case 1:
                cout << "Enter ID: ";
                cin >> id;
                while(!checkID(id)) {
                    cout << "Invalid ID. Please enter a numeric ID: ";
                    cin >> id;
                }
                cout << "Enter Name: ";
                cin >> name;
                cout << "Enter Password: ";
                cin >> password;
                system.registerUser(id, name, password); 
                break;

            case 2:
                cout << "Enter ID: ";
                cin >> id;
                while(!checkID(id)) {
                    cout << "Invalid ID. Please enter a numeric ID: ";
                    cin >> id;
                }
                cout << "Enter Name: ";
                cin >> name;
                cout << "Enter Password: ";
                cin >> password;
                system.registerDriver(id, name, password); 
                break;

            case 3: {
                cout << "Enter ID: ";
                cin >> id;
                cout << "Enter Password: ";
                cin >> password;
                Node* user = system.loginUser(id, password); 
                if (user) {
                    while (true) {
                        cout << "\n--- User Menu ---\n";
                        cout << "1. Request Ride\n2. View Ride History\n3. Exit\n";
                        cout << "Enter your choice: ";

                        if (!(cin >> subChoice)) {
                            cout << "Invalid input. Returning to User Menu.\n";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            continue;
                        }

                        if (subChoice == 1) {
                            system.requestRide(user->id); 
                        } else if (subChoice == 2) {
                            system.showRideHistory(user); 
                        } else if (subChoice == 3) {
                            break;
                        } else {
                            cout << "Invalid choice. Try again.\n";
                        }
                    }
                }
                break;
            }

            case 4: {
                cout << "Enter ID: ";
                cin >> id;
                cout << "Enter Password: ";
                cin >> password;
                Node* driver = system.loginDriver(id, password); 
                if (driver) {
                    while (true) {
                        cout << "\n--- Driver Menu ---\n";
                        cout << "1. Accept Ride\n2. Exit\n";
                        cout << "Enter your choice: ";

                        if (!(cin >> subChoice)) {
                            cout << "Invalid input. Returning to Driver Menu.\n";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            continue;
                        }

                        if (subChoice == 1) {
                            system.acceptRide(); 
                        } else if (subChoice == 2) {
                            break;
                        } else {
                            cout << "Invalid choice. Try again.\n";
                        }
                    }
                }
                break;
            }

            case 5:
                cout << "Exiting SmartRide. Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}
