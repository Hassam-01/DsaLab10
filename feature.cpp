#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <unordered_map>



using namespace std;

// take user name and password and assign user id and store user id and password in password.txt and user id and name in login.txt
void register_user(string name, string password) {
    ifstream file("password.txt");
    string line;
    vector<string> passwords;
    while (getline(file, line)) {
        passwords.push_back(line);
        cout<<"Line: "<<line<<endl;
    }
    file.close();
    ofstream file2("password.txt");
    for (int i = 0; i < passwords.size(); i++) {
        file2 << passwords[i] << endl;
        cout<< "Password: "<<passwords[i]<<endl;
    }
    file2 << password;
    file2.close();

    ifstream file3("login.txt");
    vector<string> logins;
    while (getline(file3, line)) {
        logins.push_back(line);
    }
    file3.close();
    ofstream file4("login.txt");
    for (int i = 0; i < logins.size(); i++) {
        file4 << logins[i] << endl;
    }
    file4 << name;
    file4.close();
}

// take user id and password and check if the user id and password are correct
bool login_user(string name, string password) {
    ifstream file("password.txt");
    string line;
    vector<string> passwords;
    while (getline(file, line)) {
        passwords.push_back(line);
    }
    file.close();
    ifstream file2("login.txt");
    vector<string> logins;
    while (getline(file2, line)) {
        logins.push_back(line);
    }
    file2.close();
    for (int i = 0; i < logins.size(); i++) {
        if (logins[i] == name) {
            if (passwords[i] == password) {
                return true;
            }
        }
    }
    return false;
}

void loadData(unordered_map<string, string>& map) {
    ifstream file("password.txt");
    string line;
    while (getline(file, line)) {
        string name = line.substr(0, line.find(" "));
        string password = line.substr(line.find(" ") + 1);
        map[name] = password;
    }
    file.close();
}

int main(){
    string name, password;
    int i =0;
    while(i<5){

    cout << "Enter your name: ";
    cin >> name;
    cout << "Enter your password: ";
    cin >> password;
    register_user(name, password);
    i++; 
    }
    unordered_map<string, string> map;

    // unordered_map<int, string> map;

    loadData(map); // loading all the data from the file.

    for(auto const& x : map) {
        cout << x.first << " " << x.second << endl;
    }
    // cout << "Enter your name: ";
    // cin >> name;
    // cout << "Enter your password: ";
    // cin >> password;
    // if (login_user(name, password)) {
    //     cout << "Login successful" << endl;
    // } else {
    //     cout << "Login failed" << endl;
    // }
    return 0;
}
