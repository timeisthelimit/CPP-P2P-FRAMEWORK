#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <iostream>


class Peer
{
    public:
        Peer();
        Peer(int domain, std::string const &sock_addr);
        int send_message(char mssg[]);
        int receive_message(std::string* const mssg);
        int sock;
        int domain;
        std::string unix_path;
        int ID;
        sockaddr INET_ADDR;
        sockaddr_un UNIX_ADDR;

};

class Server
{
    private:
        std::string unix_addr;
        std::string host;
        std::string port;
        int (*handler)(Peer*);
        int server_sock;
        struct sockaddr_un unix_sa;
        socklen_t unix_sa_len;

    public:
        Server();
        Server(std::string const &unix_addr);
        Server(std::string const &host, std::string const &port) :host{host}, port{port}
        {
            std::cout << "test: 2" << '\n';
        };

        /**
         * @brief register a function to handle new connections
         */
        void registerConnectionHandler(int (*func)(Peer*));

        /**
         * @brief   accept incoming connections and call the
         *          registered function for handling new connections
         */
        int acceptConnections();

        /**
         * @brief attempt to connect to a peer
         * 
         * @details attempts to connect to a peer,
         *          and if a connection is established
         *          the registered connectionHandler is
         *          called with the new Peer
         * 
         * @return Peer*
         */
        Peer* connectTo(const char* addr);

        //~Server();
};
