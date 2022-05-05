CXX = g++
CXXFLAGS = -std=c++17
SFML = -lsfml-graphics -lsfml-window -lsfml-system
FILES = *.cpp
OBJECTS = *.o
HEADERS = *.h

all: 
	make linux_server linux_client

linux_server: linux_server.cpp common.cpp
	$(CXX) $(CXXFLAGS) linux_server.cpp common.cpp -o linux_server

linux_client: linux_client.cpp common.cpp
	$(CXX) $(CXXFLAGS) linux_client.cpp common.cpp -o linux_client

run_server: linux_server
	./linux_server
	
run_client: linux_client
	./linux_client