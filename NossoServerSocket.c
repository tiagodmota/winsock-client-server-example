#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define RECV_BUFFER_SIZE (size_t) 512
#define SERVER_PORT "27015"
#define SERVER_IP "127.0.0.1"

void getHostName(char *hostnameArr);
void getHostIp(char *hostname);
void wsaStartup(WSADATA* wsaData);

struct in_addr server_ip_addr;
int sock_status = -1;

int main(void)
{

    WSADATA wsaData;
    wsaStartup(&wsaData);
    
    char hostname[NI_MAXHOST];
    getHostName(hostname);
    getHostIp(hostname);
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Criacao do socket falhou com o erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverSockAddrInfo = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(SERVER_PORT)), //host to network short
        .sin_addr.s_addr = inet_addr(SERVER_IP) // ou inet_pton(AF_INET, SERVER_IP, &serverSockAddrInfo.sin_addr); ¬
    }; 
    
    sock_status = bind(serverSocket, (struct sockaddr*)&serverSockAddrInfo, sizeof(serverSockAddrInfo));
    if (sock_status == SOCKET_ERROR) {
        printf("Nao foi possivel fazer o bind() do socket servidor. Codigo de erro: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    sock_status = listen(serverSocket, SOMAXCONN);
    if (sock_status == SOCKET_ERROR) {
        printf("listen() falhou com o erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    SOCKET clientSocket;
    printf("Servidor %s aguardando conexao na porta %u...\n", hostname, ntohs(serverSockAddrInfo.sin_port));

    int addrlen = sizeof(serverSockAddrInfo);
    clientSocket = accept(serverSocket, (struct sockaddr*)&serverSockAddrInfo, &addrlen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro na aprovacao de conexao com o client. Codigo: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("\n       ========================================\n");
    printf("         A conexao foi estabelecida.\n");
    printf("       ========================================\n");
    char recvbuf[RECV_BUFFER_SIZE];     
    sock_status = 1;
    while (sock_status > 0) {
        sock_status = recv(clientSocket, recvbuf, RECV_BUFFER_SIZE, 0);
        printf("%s\n", recvbuf);
        ZeroMemory(recvbuf, sizeof(recvbuf));
    }
    printf("\nOps. Conexao interrompida por algum motivo ai. Vou ver e te aviso.\n");
    closesocket(serverSocket);
    closesocket(clientSocket);
    WSACleanup();
    system("pause");
    return EXIT_SUCCESS;

}

void getHostName(char *hostnameArr)
{
    gethostname(hostnameArr, NI_MAXHOST);
    printf("*************\nServidor: Meu nome: %s\n", hostnameArr);
}

void getHostIp(char *hostname)
{
    PHOSTENT pHostEnt = gethostbyname(hostname);
    for (int i = 0; pHostEnt->h_addr_list[i] != 0; ++i)
    {
        memcpy(&server_ip_addr, pHostEnt->h_addr_list[i], sizeof(struct in_addr));
        printf("Servidor: Meu endereco IP: %s\n", inet_ntoa(server_ip_addr));
    }
}

void wsaStartup(WSADATA* wsaData) {
    sock_status = WSAStartup(MAKEWORD(2, 2), wsaData);
    if (sock_status != 0)
    {
        printf("WSAStartup() falhou com o seguinte codigo de erro: %d\n", sock_status);
        system("pause");
        exit(EXIT_FAILURE);
    }
}