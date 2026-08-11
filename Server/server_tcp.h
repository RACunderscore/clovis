#pragma once

#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

enum class Event_type {
    READ,
    WRITE,
    READ_WRITE
};

class Server_tcp {
private:
    int socket_server;
    sockaddr_in address{};

public:
    Server_tcp(int port = 8080) {
        socket_server = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_server < 0) {
            perror("socket");
            throw std::runtime_error("Failed to create socket");
        }

        make_non_blocking(socket_server);

        int opt = 1;

        if (setsockopt(socket_server,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) < 0) {
            perror("setsockopt");
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = INADDR_ANY;

        if (bind(socket_server,reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0) {
            perror("bind");
            close(socket_server);
            throw std::runtime_error("Failed to bind socket");
        }

        if (listen(socket_server, 5) < 0) {
            perror("listen");
            close(socket_server);
            throw std::runtime_error("Failed to listen");
        }

        std::cout << "Server listening on port " << port << std::endl;
    }

    int accept_client() {
        int client = accept(socket_server,nullptr,nullptr);

        if (client == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return -1;
            perror("accept");
            throw std::runtime_error("accept failed");
        }

        make_non_blocking(client);

        std::cout << "Client connected." << std::endl;
        return client;
    }

    int get_server_socket() const {
        return socket_server;
    }

    epoll_event make_event(int fd,Event_type type ) const {
        epoll_event event{};
        event.data.fd = fd;
        switch (type) {
            case Event_type::READ:
                event.events = EPOLLIN;
                break;
            case Event_type::WRITE:
                event.events = EPOLLOUT;
                break;
            case Event_type::READ_WRITE:
                event.events = EPOLLIN | EPOLLOUT;
                break;
        }
        return event;
    }

    void add_event(int epoll_fd,int fd,Event_type type = Event_type::READ) const {
        epoll_event event = make_event(fd, type);

        if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event) == -1) {
            perror("epoll_ctl ADD");
            throw std::runtime_error("Failed to add fd to epoll");
        }
    }

    void modify_event(int epoll_fd,int fd,Event_type type) const {
        epoll_event event = make_event(fd, type);

        if (epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&event) == -1) {
            perror("epoll_ctl MOD");
            throw std::runtime_error("Failed to modify epoll event");
        }
    }

    void remove_event(int epoll_fd,int fd) const {
        if (epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,nullptr) == -1) {
            if (errno != ENOENT) {
                perror("epoll_ctl DEL");
            }
        }
    }

    void make_non_blocking(int fd) {
        int flags = fcntl(fd,F_GETFL,0);

        if (flags == -1) {
            perror("fcntl(F_GETFL)");
            throw std::runtime_error("fcntl failed");
        }

        if (fcntl(fd,F_SETFL,flags | O_NONBLOCK) == -1) {
            perror("fcntl(F_SETFL)");
            throw std::runtime_error("fcntl failed");
        }
    }

    ~Server_tcp() {
        if (socket_server >= 0)
            close(socket_server);
    }
};