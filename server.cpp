#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "server.hpp"
#include "cerror.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <sys/select.h>

  //////////////////
 //    SERVER    //
//////////////////

Server::Server(std::string const &unix_addr) :unix_addr{unix_addr}
{

    int new_sock {socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)};
    if (-1 == new_sock) {
        cerror(__LINE__, __FILE__, "socket()");
        return;
    }

    this->server_sock = new_sock;

    this->unix_sa.sun_family = AF_UNIX;
    strncpy(this->unix_sa.sun_path, unix_addr.c_str(), unix_addr.length()+1);
    this->unix_sa_len = SUN_LEN(&this->unix_sa);

    if (-1 == bind(this->server_sock, (const sockaddr*)&this->unix_sa, this->unix_sa_len)) {
        cerror(__LINE__, __FILE__, "bind()");
        return;
    }

    if (-1 == listen(this->server_sock, 256)) {
        cerror(__LINE__, __FILE__, "listen()");
        return;
    }

}

void Server::registerConnectionHandler(int (*func)(Peer*)) 
{
    this->handler = func;
}

int Server::acceptConnections()
{

    struct sockaddr_un new_peer_sock_addr {};
    socklen_t len { SUN_LEN(&new_peer_sock_addr) };

    int new_peer_sock {accept(this->server_sock, (sockaddr*)&new_peer_sock_addr, &len)};
    if (-1 == new_peer_sock) {
        if (!errno == EAGAIN && !errno == EWOULDBLOCK)
            cerror(__LINE__, __FILE__, "accept()");
            
        return -1;
    }

    Peer *np = new Peer(AF_UNIX, std::string(new_peer_sock_addr.sun_path));
    np->sock = new_peer_sock;
    np->unix_path = new_peer_sock_addr.sun_path;
    this->handler(np);

    return 0;
}

Peer* Server::connectTo(const char* addr)
{
    int peer_sock {socket(AF_UNIX, SOCK_STREAM, 0)};
    if (peer_sock == -1) cerror_exit(__LINE__, __FILE__, "socket()");

    sockaddr_un peer_sock_addr {};
    peer_sock_addr.sun_family = AF_UNIX;
    strncpy(peer_sock_addr.sun_path, addr, strlen(addr));
    socklen_t len {SUN_LEN(&peer_sock_addr)};

    if (-1 == connect(peer_sock, (const struct sockaddr*)&peer_sock_addr, len))
        cerror_exit(__LINE__, __FILE__, "connect()");

    Peer *np = new Peer(AF_UNIX, std::string(peer_sock_addr.sun_path));
    np->sock = peer_sock;
    np->unix_path = peer_sock_addr.sun_path;
    this->handler(np);

    return np;
}

  ////////////////
 //    PEER    //
////////////////

//Peer::Peer(){}

Peer::Peer(int domain, std::string const &sock_addr) : domain {domain}, unix_path {sock_addr} 
{
    UNIX_ADDR.sun_family = domain;
    strncpy(UNIX_ADDR.sun_path, unix_path.c_str(), unix_path.length()+1);
}

int Peer::send_message(char mssg[])
{
    int peer_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (-1 == peer_sock) {
        cerror(__LINE__, __FILE__, "socket()");
        return -1;
    }

    // if (-1 == connect(peer_sock, (const sockaddr*)&this->UNIX_ADDR, SUN_LEN(&this->UNIX_ADDR))) {
    //     cerror(__LINE__, __FILE__, "connect()");
    //     return -1;
    // }

    if (-1 == send(this->sock, mssg, strlen(mssg), 0)) {
        cerror(__LINE__, __FILE__, "send()");
        return -1;
    }

    if (-1 == close(peer_sock)) {
        cerror(__LINE__, __FILE__, "close()");
        return -1;
    }

    return 0;
}