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

#define DATA_BUFFER 4096 // Размер буфера для данных
#define CMD_BUFFER 4096  // Размер буфера команд и заголовков

int server_socket(char port[]);

#endif
