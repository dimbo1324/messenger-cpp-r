#pragma once
#if defined(_WIN64) || defined(_WIN32)

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Logger/Logger.h"
#include "../SVCLI/SVCLI.h"

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#include "../Handler/ServerHandler.h"
#include "../../../Misc/Misc.h"

#define DATA_BUFFER 4096 // Размер буфера для данных
#define CMD_BUFFER 4096  // Размер буфера команд и заголовков

int server_socket(char port[]);

#endif