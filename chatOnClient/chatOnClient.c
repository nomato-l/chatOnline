#include "chatOnClient.h"

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return -1;
    }
#endif

    // 创建socket套接字
#ifdef _WIN32
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
#else
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
#endif
    {
        printf("socket failed\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

    // 连接服务器
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(8080);
    target.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (
#ifdef _WIN32
        connect(client_socket, (struct sockaddr *)&target, sizeof(target)) == SOCKET_ERROR
#else
        connect(client_socket, (struct sockaddr *)&target, sizeof(target)) == -1
#endif
    )
    {
        printf("连接服务器失败!!\n");
#ifdef _WIN32
        closesocket(client_socket);
        WSACleanup();
#else
        close(client_socket);
#endif
        return -1;
    }

    // 开始通讯
    while (1)
    {
        char sbuffer[1024] = {0};
        printf("please enter:");
        scanf("%s", sbuffer);

        send(client_socket, sbuffer, strlen(sbuffer), 0);

        char rbuffer[1024] = {0};
#ifdef _WIN32
        int ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
#else
        ssize_t ret = recv(client_socket, rbuffer, sizeof(rbuffer), 0);
#endif
        if (ret <= 0)
            break;
        printf("%s\n", rbuffer);
    }

    // 关闭连接
#ifdef _WIN32
    closesocket(client_socket);
    WSACleanup();
#else
    close(client_socket);
#endif
    return 0;
}