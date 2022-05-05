#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <limits>
#include <errno.h>

#define PORT 6969
#define BUFFER_SIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 20

#define MAX_CLIENT 100
#define UPLOAD_DESTINATION "./upload"
#define FINISH_TRANSFER "\t\r\t\r\n\r\n\r\n"

#define SA_IN sockaddr_in
#define SA sockaddr

using namespace std;

int check(int, const char*);


#endif