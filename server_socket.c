#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define RECV_BUFFER_SIZE (size_t)512
#define SERVER_PORT (u_int)27015
#define SERVER_IP (char[]) "127.0.0.1"

#define WSASSTARTUP_SUCCESS 0

void assignHostname(char *hostNameArr);
void showHostIp(char *hostname);

struct in_addr ip_addr;

int main(void)
{

    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    char recvBuffer[RECV_BUFFER_SIZE];
    int status = -1;

    status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != WSASSTARTUP_SUCCESS)
    {
        printf("WSAStartup falhou com o código de erro: %d\n", status);
        system("pause");
        return EXIT_FAILURE;
    }
    else
    {
        char hostname[NI_MAXHOST];
        assignHostname(hostname);
        showHostIp(hostname);
    }
}

void assignHostname(char *hostNameArr)
{
    gethostname(hostNameArr, NI_MAXHOST);
    printf("*************\nServidor: Meu nome: %s\n", hostNameArr);
}

void showHostIp(char *hostname)
{
    PHOSTENT hostInfos = gethostbyname(hostname);
    for (int i = 0; hostInfos->h_addr_list[i] != 0; ++i)
    {
        memcpy(&ip_addr, hostInfos->h_addr_list[i], sizeof(struct in_addr));
        printf("Servidor: Meu endereco IP [%d]: %s\n", i, inet_ntoa(ip_addr));
    }
}
