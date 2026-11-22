#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#include <replxx.hxx>

#include "client_tcp.h"

using namespace std;

int main() {
    Client_tcp client;
    if (!client.connect_to_server()) {
        return 1;
    }

    replxx::Replxx input;

    while (true) {
        const char* line = input.input("CLOVIS >> ");

        if (line == nullptr) {
            cout << '\n';
            break;
        }

        const string data(line);
        if (data.empty()) {
            continue;
        }

        input.history_add(data);

        string command = data;

        std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c) {
            return std::tolower(c);
        });

        if (command == "exit") {
            break;
        }

        if (!client.send_data(data + "\n")) {
            break;
        }

        const string response = client.receive_data();
        if (response.empty()) {
            cout << "Server disconnected" << endl;
            break;
        }

        cout << response;
    }

    client.close_connection();

    return 0;
}
