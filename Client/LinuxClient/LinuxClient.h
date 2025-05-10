#pragma once
#if defined(__linux__)
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../Misc/Misc.h"
#include "../Handler/ClientHandler.h"
#define DATA_BUFFER 4096
#define CMD_BUFFER 4096
const extern int PORT;
int client_socket(char server_address[], char port[]);
#endif