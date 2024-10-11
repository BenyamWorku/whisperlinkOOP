# WhisperLink: P2P Chat Application

WhisperLink is a peer-to-peer chat application that enables direct communication between two users over a local network. It's designed to be simple, efficient, and easy to use, providing a seamless chatting experience without the need for a central server.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Building](#building)
- [Usage](#usage)
- [How It Works](#how-it-works)
- [Chat Commands](#chat-commands)
- [Contributing](#contributing)
- [Future Improvements](#future-improvements)
- [Challenges](#challenges)

## Features

- Automatic peer discovery using UDP broadcasting
- Direct TCP connection between peers
- Dynamic port allocation for increased flexibility
- Simple and intuitive command-line interface
- Colorized chat messages for improved readability
- Graceful handling of peer disconnections

## Requirements

- C++11 compatible compiler
- CMake (version 3.10 or higher)
- POSIX-compliant operating system (Linux, macOS)

## Project Structure

The project consists of the following main files:

- `main.cpp`: Entry point of the application
- `Peer.h`: Header file defining the Peer class
- `Peer.cpp`: Implementation of the Peer class
- `CMakeLists.txt`: CMake configuration file

## Building

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/whisperlink.git
   cd whisperlink
   ```

2. Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```

3. Generate the build files with CMake:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   make
   ```

## Usage

Run the application with your desired username:

```
./whisperlink <username>
```

The application will automatically discover peers on the local network and establish a connection.

## How It Works

1. When started, WhisperLink broadcasts its presence on the local network using UDP.
2. It simultaneously listens for broadcasts from other peers.
3. Upon discovering a peer, it attempts to establish a TCP connection.
4. The peer with the lower port number acts as the client and initiates the connection.
5. Once connected, users can exchange messages in real-time.

## Chat Commands

- Type your message and press Enter to send
- Type `/exit` to end the chat session

## Contributing

Contributions to WhisperLink are welcome! Please follow these steps:

1. Fork the repository
2. Create a new branch for your feature
3. Commit your changes
4. Push to your branch
5. Create a new Pull Request

## Future Improvements

- Implement end-to-end encryption for secure communication
- Add support for group chats
- Develop a graphical user interface
- Implement file transfer functionality
- Add support for audio and video calls

## Challenges

During the development of WhisperLink, we encountered several challenges:

1. **Encryption Implementation**: Attempts to integrate OpenSSL and libsodium for message encryption were unsuccessful. OpenSSL didn't work as expected, and libsodium faced issues even with hardcoded keys. Implementing robust encryption remains a priority for future development to ensure secure communication.

2. **NAT Traversal**: The current implementation works well on local networks but may face issues with NAT traversal in wider network scenarios.

3. **Peer Discovery**: While the UDP broadcast method works for local networks, it may not be suitable for larger or segmented networks. Alternative peer discovery mechanisms may need to be explored.

4. **Error Handling**: Robust error handling and recovery mechanisms need to be implemented to handle various network scenarios and potential failures.

These challenges present opportunities for future improvements and contributions to the project.