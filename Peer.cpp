#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <ifaddrs.h>
#include <sys/select.h>
#include <thread>
#include <fcntl.h>
#include <chrono>

class Peer
{
public:
    // Constructor to initialise the peer
    Peer(const std::string &local_name)
        : name(local_name), udp_sock(-1), connection_sock(-1), listening_sock(-1)
    {
        local_ip = get_local_ip();
        srand(time(nullptr)); // Seed random number generator for dynamic ports
    }

    // Destructor to clean up sockets
    ~Peer()
    {
        cleanup();
    }

    // Method to initialise the listener
    void initialize_listener()
    {
        listening_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (listening_sock == -1)
        {
            std::cerr << "Failed to create TCP listening socket." << std::endl;
            return;
        }

        sockaddr_in listen_addr;
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = INADDR_ANY;
        listen_addr.sin_port = 0; // Let the OS assign a free port

        if (bind(listening_sock, (sockaddr *)&listen_addr, sizeof(listen_addr)) == -1)
        {
            std::cerr << "Failed to bind TCP listening socket." << std::endl;
            close(listening_sock);
            return;
        }

        // Get the assigned port number
        socklen_t len = sizeof(listen_addr);
        getsockname(listening_sock, (sockaddr *)&listen_addr, &len);
        tcp_port = ntohs(listen_addr.sin_port);

        if (listen(listening_sock, 5) == -1)
        {
            std::cerr << "Failed to start listening." << std::endl;
            close(listening_sock);
            return;
        }

        std::cout << "TCP listener initialized on " << local_ip << ":" << tcp_port << std::endl;
    }

    // Method to broadcast presence
    void broadcast_presence()
    {
        udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sock == -1)
        {
            std::cerr << "Error: Failed to create UDP socket." << std::endl;
            return;
        }

        int broadcast_enable = 1;
        if (setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
        {
            std::cerr << "Error: Failed to enable broadcast." << std::endl;
            close(udp_sock);
            return;
        }

        sockaddr_in broadcast_addr;
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_port = htons(25000);
        broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

        std::string message = name + ":" + std::to_string(tcp_port);
        int bytes_sent = sendto(udp_sock, message.c_str(), message.size(), 0,
                                (sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

        if (bytes_sent == -1)
        {
            std::cerr << "Error: Failed to broadcast presence." << std::endl;
        }
        else
        {
            std::cout << "Broadcasting presence on port " << tcp_port << "..." << std::endl;
        }
        // grace cooloff period before shutting down socket
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        close(udp_sock);
        udp_sock = -1;
    }

    // Method to discover peers once
    bool discover_peers()
    {
        udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sock == -1)
        {
            std::cerr << "Error: Failed to create UDP socket." << std::endl;
            return false;
        }

        int reuse = 1;
        if (setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            std::cerr << "Error: Failed to set SO_REUSEADDR on UDP socket." << std::endl;
            close(udp_sock);
            return false;
        }

        sockaddr_in listen_addr;
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_addr.s_addr = INADDR_ANY;
        listen_addr.sin_port = htons(25000);

        if (bind(udp_sock, (sockaddr *)&listen_addr, sizeof(listen_addr)) == -1)
        {
            std::cerr << "Error: Failed to bind UDP socket to port 25000." << std::endl;
            close(udp_sock);
            return false;
        }

        struct timeval timeout;
        timeout.tv_sec = 5; // Shorter timeout to quickly discover peers
        timeout.tv_usec = 0;
        setsockopt(udp_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        char buffer[1024];
        sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);

        std::cout << "Listening for peer broadcasts on UDP port 25000..." << std::endl;

        int bytes_received = recvfrom(udp_sock, buffer, sizeof(buffer), 0, (sockaddr *)&sender_addr, &sender_len);
        if (bytes_received > 0)
        {
            std::string message(buffer, bytes_received);
            std::cout << "Received broadcast: " << message << std::endl;

            size_t separator_pos = message.find(":");
            if (separator_pos != std::string::npos)
            {
                peer_name = message.substr(0, separator_pos);
                peer_port = std::stoi(message.substr(separator_pos + 1));
                peer_ip = inet_ntoa(sender_addr.sin_addr);
                std::cout << "Discovered peer: " << peer_name << " at " << peer_ip << ":" << peer_port << std::endl;
                close(udp_sock);
                udp_sock = -1;
                return true; // Peer discovered successfully
            }
            else
            {
                std::cerr << "Error: Incorrect message format." << std::endl;
            }
        }
        else
        {
            std::cout << "Timeout reached while waiting for broadcast. No peers discovered." << std::endl;
        }

        close(udp_sock);
        udp_sock = -1;
        return false;
    }

    bool should_act_as_client()
    {
        if (tcp_port < peer_port)
        {
            std::cout << "Local port is lower, acting as client." << std::endl;
            return true;
        }
        else
        {
            std::cout << "Local port is higher or equal, acting as server." << std::endl;
            return false;
        }
    }

    bool establish_connection()
    {
        bool connected = false;

        // Try to connect as a client if we have peer information
        if (!peer_ip.empty() && peer_port > 0)
        {
            connection_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (connection_sock != -1)
            {
                sockaddr_in peer_addr;
                peer_addr.sin_family = AF_INET;
                peer_addr.sin_port = htons(peer_port);
                inet_pton(AF_INET, peer_ip.c_str(), &peer_addr.sin_addr);

                if (connect(connection_sock, (sockaddr *)&peer_addr, sizeof(peer_addr)) == 0)
                {
                    std::cout << "Successfully connected to peer: " << peer_name << " at " << peer_ip << ":" << peer_port << std::endl;
                    connected = true;

                    // introductory message- workaround to identify the late joining peer's name
                    std::string initial_message = name + ":Hello!";
                    int response = send(connection_sock, initial_message.c_str(), initial_message.size(), 0);
                    if (response == -1)
                    {
                        std::cerr << "Error:failed to send intitial identification message." << std::endl;
                    }
                }
                else
                {
                    std::cerr << "Failed to connect to peer at " << peer_ip << ":" << peer_port << std::endl;
                    close(connection_sock);
                    connection_sock = -1;
                }
            }
        }

        // If not connected, listen for incoming connections
        if (!connected)
        {
            std::cout << "Waiting for incoming connections..." << std::endl;

            fd_set read_fds;
            struct timeval tv;
            FD_ZERO(&read_fds);
            FD_SET(listening_sock, &read_fds);
            tv.tv_sec = 20; // was 5 second timeout
            tv.tv_usec = 0;

            int select_result = select(listening_sock + 1, &read_fds, NULL, NULL, &tv);
            if (select_result > 0)
            {
                sockaddr_in client_addr;
                socklen_t client_size = sizeof(client_addr);
                connection_sock = accept(listening_sock, (sockaddr *)&client_addr, &client_size);

                if (connection_sock != -1)
                {   // Extract peer information from the accepted connection
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                peer_ip = std::string(client_ip);
                peer_port = ntohs(client_addr.sin_port);
                peer_name = "Unknown_Peer";  // Assign a default name since I don't know it yet
                std::cout << "Accepted incoming connection from peer!" << std::endl;
                connected = true;
                }
                else
                {
                    std::cerr << "Failed to accept incoming connection." << std::endl;
                }
            }
            else if (select_result == 0)
            {
                std::cout << "Timeout waiting for incoming connection." << std::endl;
            }
            else
            {
                std::cerr << "Error in select(): " << strerror(errno) << std::endl;
            }
        }

        return connected;
    }

    void handle_chat_session()
    {
        if (connection_sock == -1)
        {
            std::cerr << "No connection established." << std::endl;
            return;
        }

        char buffer[256];
        std::string input_message;
        fd_set read_fds;
        struct timeval tv;

        // Set up for chat session
        std::cout << "Chat session started with peer: " << peer_name << std::endl;
        while (true)
        {
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO, &read_fds);
            FD_SET(connection_sock, &read_fds);

            tv.tv_sec = 0;
            tv.tv_usec = 100000; // 100ms timeout

            int max_fd = std::max(STDIN_FILENO, connection_sock) + 1;
            int activity = select(max_fd, &read_fds, NULL, NULL, &tv);

            if (activity < 0)
            {
                std::cerr << "Error in select: " << strerror(errno) << std::endl;
                break;
            }
            if (FD_ISSET(STDIN_FILENO, &read_fds))
            {
                std::cout << "You: ";
                std::getline(std::cin, input_message);
                std::cout << std::flush;
                if (send(connection_sock, input_message.c_str(), input_message.size(), 0) == -1)
                {
                    std::cerr << "Error: Failed to send message." << std::endl;
                    break;
                }

                if (input_message == "/exit")
                {
                    std::cout << "Ending chat session." << std::endl;
                    break;
                }
            }

            if (FD_ISSET(connection_sock, &read_fds))
            {
                memset(buffer, 0, sizeof(buffer));
                int bytes_received = recv(connection_sock, buffer, sizeof(buffer) - 1, 0);

                if (bytes_received > 0)
                {
                    buffer[bytes_received] = '\0';
                    if (peer_name == "Unknown_Peer")
                    {
                        peer_name = extract_peer_name(buffer); // Update peer name if it's the first message
                    }
                    std::cout << peer_name << ": " << buffer << std::endl;
                }
                else if (bytes_received == 0)
                {
                    std::cout << peer_name << " has disconnected." << std::endl;
                    break;
                }
                else
                {
                    std::cerr << "Error on receiving data: " << strerror(errno) << std::endl;
                    break;
                }
            }
        }
    }

private:
    std::string extract_peer_name(const std::string &message)
    {
        // Simple workaround to extract peer name from the message
        size_t colon_pos = message.find(':');
        if (colon_pos != std::string::npos)
        {
            return message.substr(0, colon_pos);
        }
        return "Unknown_Peer";
    }

    std::string name;
    std::string local_ip;
    int tcp_port;
    int udp_sock;
    int connection_sock;
    int listening_sock;

    // Discovered peer information
    std::string peer_name;
    std::string peer_ip;
    int peer_port{0}; // initialised to avoid unpredictable default behaviour

    // Method to clean up sockets and resources
    void cleanup()
    {
        if (udp_sock != -1)
        {
            close(udp_sock);
            std::cout << "UDP socket closed." << std::endl;
        }
        if (connection_sock != -1)
        {
            close(connection_sock);
            std::cout << "TCP connection socket closed." << std::endl;
        }
        if (listening_sock != -1)
        {
            close(listening_sock);
            std::cout << "Listening socket closed." << std::endl;
        }
    }

    std::string get_local_ip()
    {
        struct ifaddrs *ifap, *ifa;
        struct sockaddr_in *sa;
        std::string local_ip;

        // Get the network interfaces
        if (getifaddrs(&ifap) == -1)
        {
            std::cerr << "Error getting network interfaces." << std::endl;
            return "";
        }

        // Iterate over the interfaces
        for (ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET)
            {
                sa = (struct sockaddr_in *)ifa->ifa_addr;
                char *ip = inet_ntoa(sa->sin_addr);
                // Ignore the loopback interface
                if (std::string(ifa->ifa_name) != "lo")
                {
                    local_ip = ip;
                    break;
                }
            }
        }

        // Free the interface list
        freeifaddrs(ifap);

        if (local_ip.empty())
        {
            std::cerr << "No non-loopback IP found." << std::endl;
        }

        return local_ip;
    }
};

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
        std::cout << "Connection established. Starting chat session..." << std::endl;
        peer.handle_chat_session();
    }
    else
    {
        std::cerr << "Failed to establish connection." << std::endl;
    }

    return 0;
}