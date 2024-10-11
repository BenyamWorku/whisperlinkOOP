#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <cstdlib> // system++
#include <ctime>
#include <ifaddrs.h>
#include <sys/select.h>
#include <thread>
#include <fcntl.h>
#include <chrono>
#include <iomanip>

class Peer
{
public:
    // Constructor to initialise the peer
    Peer(const std::string &local_name);

    // Destructor to clean up sockets
    ~Peer();

    // Method to initialise the listener
    void initialize_listener();

    // Method to broadcast presence
    void broadcast_presence();

    // Method to discover peers
    bool discover_peers();

    // Method to decide whether to act as a client or server
    bool should_act_as_client();

    // Method to establish a connection
    bool establish_connection();

    // Method to exchange names with the connected peer
    bool exchange_names();

    // Method to display chat messages
    void display_message(const std::string &message, bool is_local);

    // Method to handle the chat session
    void handle_chat_session();

private:
    // Member variables
    std::string name;
    std::string local_ip;
    int tcp_port;
    int udp_sock;
    int connection_sock;
    int listening_sock;

    // Discovered peer information
    std::string peer_name;
    std::string peer_ip;
    int peer_port;

    // Method to clean up sockets and resources
    void cleanup();

    // Method to get the local IP address
    std::string get_local_ip();
};