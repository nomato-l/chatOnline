#ifndef _CHATONSERVER_H
#define _CHATONSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#define CLOSESOCKET(s) closesocket(s)
typedef int socklen_t;
#define SOCKET SOCKET
#define INVALID_SOCKET INVALID_SOCKET
DWORD WINAPI threadFunc(LPVOID lpParam);
#include <winsock2.h>

#else
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define INVALID_SOCKET (-1)
void* threadFunc(void* arg);
#include <fcntl.h>

#endif

#endif


