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
#pragma comment(lib, "Ws2_32.lib")
#include "../Handler/ServerHandler.h"
#include "../../../Misc/Misc.h"
#define DATA_BUFFER 4096
#define CMD_BUFFER 4096
int server_socket(char port[]);
#endif