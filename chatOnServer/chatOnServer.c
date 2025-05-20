#include "chatOnServer.h"

int main(int argc, char *argv[])
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif

    // 创建socket套接字
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET)
    {
#ifdef _WIN32
        printf("socket failed\nerrcode:%ld\n", WSAGetLastError());
#else
        printf("socket failed\nerrcode:%d\n", errno);
#endif
        return -1;
    }

    // 给socket绑定一个端口号
    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = inet_addr("0.0.0.0"); // 服务端IP

    if (
#ifdef _WIN32
        bind(listen_sock, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR
#else
        bind(listen_sock, (struct sockaddr *)&local, sizeof(local)) == -1
#endif
    )
    {
#ifdef _WIN32
        printf("bind socket failed !!! errcode:%ld\n", WSAGetLastError());
#else
        printf("bind socket failed !!! errcode:%d\n", errno);
#endif
        CLOSESOCKET(listen_sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    // 给socket开启监听属性
    if (
#ifdef _WIN32
        listen(listen_sock, 10) == SOCKET_ERROR
#else
        listen(listen_sock, 10) == -1
#endif
    )
    {
#ifdef _WIN32
        printf("start listen failed ! errcode:%ld\n", WSAGetLastError());
#else
        printf("start listen failed ! errcode:%d\n", errno);
#endif
        CLOSESOCKET(listen_sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    // 设置socket为非阻塞模式
#ifdef _WIN32
    u_long mode = 1;
    if (ioctlsocket(listen_sock, FIONBIO, &mode) != 0)
#else
    int flags = fcntl(listen_sock, F_GETFL, 0);
    if (fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK) == -1)
#endif
    {
#ifdef _WIN32
        printf("set socket non-blocking failed ! errcode:%ld\n", WSAGetLastError());
#else
        printf("set socket non-blocking failed ! errcode:%d\n", errno);
#endif
        CLOSESOCKET(listen_sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    // 进入循环，等待客户端连接
    printf("Waiting for client connection...\n");

    while (1)
    {
        SOCKET client_sock = accept(listen_sock, NULL, NULL);
        if (client_sock == INVALID_SOCKET)
            continue;

        SOCKET *pclient = malloc(sizeof(SOCKET));
        if (!pclient) {
            printf("malloc failed\n");
            CLOSESOCKET(client_sock);
            continue;
        }
        *pclient = client_sock;

#ifdef _WIN32
        HANDLE hThread = CreateThread(NULL, 0, threadFunc, pclient, 0, NULL);
        if (hThread) CloseHandle(hThread);
#else
        pthread_t tid;
        if (pthread_create(&tid, NULL, threadFunc, pclient) == 0)
            pthread_detach(tid);
#endif
    }
    // 关闭监听socket
    CLOSESOCKET(listen_sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

#ifdef _WIN32
DWORD WINAPI threadFunc(LPVOID lpParam)
{
    SOCKET client_sock = *(SOCKET *)lpParam;
    free(lpParam);
    printf("Thread started\n");
    while (1)
    {
        char buffer[1024] = {0};
        int recv_len = recv(client_sock, buffer, 1024, 0);
        if (recv_len <= 0)
            break;
        printf("%s\n", buffer);
        send(client_sock, buffer, strlen(buffer), 0);
    }
    CLOSESOCKET(client_sock);
    return 0;
}
#else
void* threadFunc(void* arg)
{
    SOCKET client_sock = *(SOCKET *)arg;
    free(arg);
    printf("Thread started\n");
    while (1)
    {
        char buffer[1024] = {0};
        int recv_len = recv(client_sock, buffer, 1024, 0);
        if (recv_len <= 0)
            break;
        printf("%s\n", buffer);
        send(client_sock, buffer, strlen(buffer), 0);
    }
    CLOSESOCKET(client_sock);
    return NULL;
}
#endif