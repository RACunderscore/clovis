#pragma once

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Client_tcp {
private:
    int socket_client;
    sockaddr_in address{};

public:
    Client_tcp() {
        socket_client = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_client < 0) {
            perror("socket");
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(8080);

        if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
            perror("inet_pton");
        }
    }

    bool connect_to_server() {
        if (connect(socket_client, (sockaddr*)&address, sizeof(address)) < 0) {
            perror("connect");
            return false;
        }
        return true;
    }

    bool send_data(const std::string& data) {
        if (send(socket_client, data.c_str(), data.size(), 0) < 0) {
            perror("send");
            return false;
        }
        return true;
    }

    std::string receive_data() {
        char buffer[1024];
        ssize_t bytes = recv(socket_client, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            return "";
        }
        buffer[bytes] = '\0';
        return std::string(buffer);
    }

    void close_connection() {
        close(socket_client);
    }

    ~Client_tcp() {
        close(socket_client);
    }
};