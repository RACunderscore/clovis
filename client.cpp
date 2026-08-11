#include <iostream>
#include <string>

using namespace std;

#include "client_tcp.h"

string get_user_command() {
    cout << "User command: ";
    string data;
    getline(cin, data);
    return data;
}

int main() {
    Client_tcp client;
    if (!client.connect_to_server()) {
        return 1;
    }

    while (true) {
        string data = get_user_command();
        if (data == "exit")
            break;

        if (!client.send_data(data + "\n")) {
            break;
        }

        string response = client.receive_data();

        if (response.empty()) {
            cout << "Server disconnected" << endl;
            break;
        }

        cout << response;
    }

    client.close_connection();

    return 0;
}