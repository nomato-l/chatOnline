#ifndef _CHATONLINE_H
#define _CHATONLINE_H


#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")



#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#endif


#endif