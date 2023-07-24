#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define RECV_BUFFER_SIZE (size_t)512
#define SERVER_PORT "27015"
#define SERVER_IP "127.0.0.1"

void getHostName(char *hostnameArr);
void getHostIp(char *hostname);

struct in_addr server_ip_addr;

int main(void)
{

    WSADATA wsaData;
    int status = -1;
    status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != 0)
    {
        printf("WSAStartup() falhou com o seguinte código de erro: %d\n", status);
        system("pause");
        return EXIT_FAILURE;
    }
    
    char hostname[NI_MAXHOST];
    getHostName(hostname);
    getHostIp(hostname);
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Criação do socket falhou com o erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in sockAddrInfo = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(SERVER_PORT)), //host to network short
        .sin_addr = server_ip_addr // .sin_addr.s_addr = inet_addr(SERVER_IP); ou inet_pton(AF_INET, SERVER_IP, &sockAddrInfo.sin_addr); ¬
    }; 
    
    status = bind(serverSocket, &sockAddrInfo, sizeof(sockAddrInfo));
    if (status == SOCKET_ERROR) {
        printf("Não foi possível fazer o bind() do socket servidor. Código de erro: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    status = listen(serverSocket, SOMAXCONN);
    if (status = SOCKET_ERROR) {
        printf("listen() falhou com o erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    SOCKET clientSocket;
    printf("Servidor %s aguardando conexão na porta %u...\n", hostname, ntohs(sockAddrInfo.sin_port)); // ¬

    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro na aprovação de conexão com o client. Código: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("\n\n========================================\n");
    printf("A conexão com o cliente foi estabelecida\n");
    printf("========================================\n\n");
    char recvbuf[RECV_BUFFER_SIZE];     
    status = 1;
    while (status > 0) {
        status = recv(clientSocket, recvbuf, RECV_BUFFER_SIZE, 0);
        printf("%s", recvbuf);
    }
    printf("Conexão interrompida por algum motivo aí. Vovê e te aviso...");
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
        printf("Servidor: Meu endereco IP [%d]: %s\n", i, inet_ntoa(server_ip_addr));
    }
}
