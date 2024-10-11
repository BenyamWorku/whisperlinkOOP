cmake_minimum_required(VERSION 3.10)

# Project name
project(PeerToPeerChatApp)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(whisperlink main.cpp Peer.cpp)

# Include directories for header files
target_include_directories(whisperlink PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link any additional libraries if necessary
# target_link_libraries(PeerToPeerChatApp PRIVATE pthread)
