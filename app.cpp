#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <csignal>
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono>

#include "cerror.h"
#include "server.hpp"

using namespace std;

void exit_and_signals();

vector<string> messages_to_send ({"ping", "ping", "ping", "ping", "ping", "ping"});
vector<int> file_descriptor_register;
std::string serv_sock_path {"/home/oskar/Documents/Coding/CppP2P/server_sock\0"};
std::string peer_sock_path {"/home/oskar/Documents/Coding/CppP2P/peer_sock\0"};
int connHandler(Peer* peer);
std::vector<Peer*> peers{};

int main(int argc, char **argv) {

    exit_and_signals();

    serv_sock_path = argv[1];
    peer_sock_path = argc > 2 ? argv[2] : "";

    cout << serv_sock_path << '\n' << peer_sock_path << '\n';

    Server s {serv_sock_path};
    s.registerConnectionHandler(connHandler);

    {
        using namespace chrono_literals;
        this_thread::sleep_for(5s);
    }

    if (!peer_sock_path.empty())
    {
        s.connectTo(peer_sock_path.c_str());
    }

    while (true)
    {
        
        s.acceptConnections();


        for (Peer* p : peers)
        {
            char buff[1024] {};
            recv(p->sock, buff, sizeof(buff), MSG_DONTWAIT);

            std::cout   << "FD: " << p->sock \
                        << "\tADDR: " << p->unix_path \
                        << "\tMSSG: " << buff << '\n';

            p->send_message((char *)"ping");
            
        }

        {
            using namespace chrono_literals;
            this_thread::sleep_for(5s);
        }
    }

    return 0;
}

int connHandler(Peer* peer)
{
    peers.push_back(peer);
    file_descriptor_register.push_back(peer->sock);

    return 0;
}

void cleanup ()
{
    if (-1 == unlink(serv_sock_path.c_str()));
        cerror(__LINE__, __FILE__, "unlink()");
}

/* general signal handling function*/
void handleSignal(int signum)
{
    for (int fd : file_descriptor_register)
        if (-1 == close(fd));
            cerror(__LINE__, __FILE__, "close(), closing, application open, file descriptors");

    exit(signum);
}

/* handle SIGHUP */
void handleSighup(int signum) 
{
    cleanup;
    handleSignal(signum);
}

/* register exit and signal clean-up functions */
void exit_and_signals() 
{
    signal(SIGABRT, handleSignal);
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
    signal(SIGQUIT, handleSignal);
    signal(SIGILL, handleSignal);
    signal(SIGHUP, handleSighup);

    atexit(cleanup);
}