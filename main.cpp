#include <iostream>
#include<string.h>
#include <string>
#include "Peer.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <local_name>" << std::endl;
        return 1;
    }

    std::string local_name = argv[1];
    Peer peer(local_name);

    // Initialize listener
    peer.initialize_listener();

    // Broadcast presence
    peer.broadcast_presence();

    // Attempt to discover peers
    bool peer_discovered = peer.discover_peers();

    if (peer_discovered)
    {
        std::cout << "Peer discovered. Attempting to establish connection..." << std::endl;
    }
    else
    {
        std::cout << "No peers discovered. Waiting for incoming connections..." << std::endl;
    }

    // Try to establish a connection
    if (peer.establish_connection())
    {
        // Connection established, start chat session

        // Exchange names after connection
        if (!peer.exchange_names())
        {
            std::cerr << "Failed to exchange names." << std::endl;
            return 1;
        }
        std::cout << "Connection established. Starting chat session..." << std::endl;
        peer.handle_chat_session();
    }
    else
    {
        std::cerr << "Failed to establish connection." << std::endl;
    }

    return 0;
}