#pragma once
#if defined(__linux__)
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../Handler/ServerHandler.h"
#include "../../../Misc/Misc.h"
#include "../../../Misc/BufferActions.h"
#include "../Logger/Logger.h"
#include "../SVCLI/SVCLI.h"
#define DATA_BUFFER 4096
#define CMD_BUFFER 4096
int server_socket(char port[]);
#endif
