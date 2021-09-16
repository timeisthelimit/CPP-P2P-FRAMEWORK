#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <string>

#include "./cerror.h"

using namespace std;

vector<int> file_descriptor_register {};
vector<string> unix_socket_paths {};
void exit_and_signals();

int main()
{
    errno = 0;

    exit_and_signals();

    /* create client socket */
    int sock_fd {socket(AF_UNIX, SOCK_STREAM, 0)};
    if (sock_fd == -1) cerror_exit(__LINE__, __FILE__, "socket()");
    file_descriptor_register.push_back(sock_fd);

    /* prepare address of our socket*/
    string sock_path {"/home/oskar/Documents/Coding/CppP2P/uno"};
    unix_socket_paths.push_back(sock_path);
    sockaddr_un sock_addr;
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, \
            sock_path.c_str(), \
            sock_path.size()+1);
    socklen_t sock_addr_len { SUN_LEN(&sock_addr) };

    /* bind our socket to legible address*/
    unlink(sock_path.c_str());
    if (-1 == bind(sock_fd, (const struct sockaddr*)&sock_addr, sock_addr_len))
        cerror(__LINE__, __FILE__, "bind()");

    /* initialize server address */
    sockaddr_un serv_sock_addr {};
    serv_sock_addr.sun_family = AF_UNIX;
    {
        char sock_path[] {"/home/oskar/Documents/Coding/CppP2P/server\x5Fsock\0"};
        strncpy(serv_sock_addr.sun_path, sock_path, strlen(sock_path));
    }

    /* get length of 'serv_sock_addr' struct */
    socklen_t len { SUN_LEN(&serv_sock_addr) };

    /* connect with server socket*/
    if (connect(sock_fd, (const struct sockaddr*)&serv_sock_addr, len) == -1)
        cerror_exit(__LINE__, __FILE__, "connect()");

    /* send message to server socket*/
    {
        const char* buff {"Hello, world!"};
        if (-1 == send(sock_fd, buff, strlen(buff), 0))
            cerror_exit(__LINE__, __FILE__, "send()");
    }

    {
        using namespace chrono_literals;
        this_thread::sleep_for(15s);
    }

    {
        const char* buff {"My address is: /home/oskar/Documents/Coding/CppP2P/server\x5Fsock"};
        if (-1 == send(sock_fd, buff, strlen(buff), 0))
            cerror_exit(__LINE__, __FILE__, "send()");
    }

    return 0;
}

void cleanup ()
{
    for (string sock_path : unix_socket_paths)
        if (-1 == unlink(sock_path.c_str()))
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