#include <iostream>
#include <string>
#include <unordered_map>

#include <sys/epoll.h>
#include <unistd.h>

#include "server_tcp.h"
#include "data_storage.h"
#include "query_parser.h"
#include "query_result.h"


int main() {
    Server_tcp server;
    Data_storage data;
    Query_parser parser(data);

    std::unordered_map<int, std::string> pending;

    int epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    try {
        server.add_event(epoll_fd,server.get_server_socket(),Event_type::READ);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        close(epoll_fd);
        return 1;
    }

    constexpr int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    while (true) {
        int n = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if (n == -1) {
            if (errno == EINTR)
                continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            uint32_t event_type = events[i].events;

            if (event_type & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                if (fd != server.get_server_socket()) {
                    std::cout << "Client disconnected: " << fd << std::endl;
                    server.remove_event(epoll_fd, fd);
                    close(fd);
                    pending.erase(fd);
                }
                continue;
            }

            if (fd == server.get_server_socket()) {
                while (true) {
                    int client = server.accept_client();
                    if (client == -1)
                        break;

                    try {
                        server.add_event(epoll_fd,client,Event_type::READ);
                        std::cout << "New client connected: " << client << std::endl;
                    }
                    catch (const std::runtime_error& e) {
                        std::cerr << "Failed to add client: " << e.what() << std::endl;
                        close(client);
                    }
                }
                continue;
            }

            if (event_type & EPOLLIN) {
                char buffer[1024];
                ssize_t bytes = recv(fd,buffer,sizeof(buffer),0);

                if (bytes == 0) {
                    std::cout << "Client disconnected: " << fd << std::endl;
                    server.remove_event(epoll_fd,fd);
                    close(fd);
                    pending.erase(fd);
                    continue;
                }

                if (bytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    perror("recv");
                    server.remove_event(epoll_fd,fd);
                    close(fd);
                    pending.erase(fd);
                    continue;
                }

                pending[fd].append(buffer,bytes);

                while (true) {
                    auto pos = pending[fd].find('\n');
                    if (pos == std::string::npos)
                        break;
                    std::string command = pending[fd].substr(0,pos);
                    pending[fd].erase(0,pos + 1);

                    if (!command.empty() && command.back() == '\r') {
                        command.pop_back();
                    }

                    std::cout << "Received: " << command << std::endl;

                    Query_result result = parser.parser(command);
                    std::string response = result.serialize();

                    ssize_t sent = send(fd,response.c_str(),response.size(),0);
                    if (sent < 0) {
                        if (errno != EAGAIN && errno != EWOULDBLOCK) {
                            perror("send");
                            server.remove_event(epoll_fd,fd);
                            close(fd);
                            pending.erase(fd);
                            break;
                        }
                    }
                }
            }
        }
    }

    close(epoll_fd);
    return 0;
}