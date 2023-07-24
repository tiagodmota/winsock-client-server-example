#include <stdio.h>
#include <winsock2.h>

#define BUFFER_TAM 512
#define PORTA_DEFAULT "27015"

int main(void) {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET; // socket listen() do servidor
    SOCKET ClientSocket = INVALID_SOCKET; // socket de conexão com o cliente
    struct sockaddr_in serverAddress, clientAddress;
    int clientAddressSize;
    char recvbuf[BUFFER_TAM];
    int recvbuflen = BUFFER_TAM;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup falhou. Erro: %d\n", iResult);
        return 1;
    }

    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Criação do socket falhou. Erro: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(PORTA_DEFAULT));

    iResult = bind(ListenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR) {
        printf("Failed to bind socket. Error code: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("Listen() falhou. Erro: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Servidor ouvindo na porta %s...\n", PORTA_DEFAULT);

    // Accept conexão com o cliente
    clientAddressSize = sizeof(clientAddress);
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
    if (ClientSocket == INVALID_SOCKET) {
        printf("Erro no accept(). Código: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Cliente conectado.\n");

    // enviar e receber mensagem
    while (1) {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Mensagem recebida do cliente: %s\n", recvbuf);
        } else if (iResult == 0) {
            printf("Cliente desconectado.\n");
            break;
        } else {
            printf("Falha na recepção de dados do cliente. Código: %ld\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
