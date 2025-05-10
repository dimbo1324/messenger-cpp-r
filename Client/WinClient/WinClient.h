#pragma once
#if defined(_WIN64) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Handler/ClientHandler.h"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DATA_BUFFER 4096 // Размер буфера для данных
#define CMD_BUFFER 4096  // Размер буфера команд и заголовков

int client_socket(char server_address[], char port[]);
#endif