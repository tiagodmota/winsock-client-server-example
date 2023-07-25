/* 
Este programa implementa uma aplicacao cliente com Winsock2 

Autora: Prof.a Me. Patricia Melo Sales

Modificado por: Tiago Dantas Mota
*/

// Codigo da Versao Minima do Windows suportada pelo programa (requerido pelo ws2tcpip.h)
#define _WIN32_WINNT 0x0501 // Windows XP

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>

// Constantes
#define BUFFER_TAM 512            // tamanho do buffer de recepcao
#define PORTA_DEFAULT "27015"    // porta de comunicacao
#define IP_SERVIDOR "127.0.0.1" // IP do servidor

#define WSASSTARTUP_SUCCESS 0

// Estrutura de enderecos
struct addrinfo *result = NULL, *ptr = NULL, hintsForGetAddrInfo;    
struct in_addr ipAddr;

FILE* file = NULL;
void openFile(FILE* file, char* mode);

int main(void) {

    WSADATA wsaData;  // Variavel para o winsock
    int iResult;      // Variavel de status

    /*sockets para conexao com o servidor*/
    SOCKET ConnectSocket = INVALID_SOCKET;    

    /*buffers de recepcao e envio*/                   
    char sendbuf[BUFFER_TAM];

    // Inicializa o Winsock2
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != WSASSTARTUP_SUCCESS) {
        printf("WSAStartup falhou com o erro de código: %d\n", iResult);
        system("pause");
        return 1;
    }

    // Informacoes da maquina cliente
    char hostname[NI_MAXHOST];        // Objeto alvo que recebera o nome da maquina
    gethostname(hostname, NI_MAXHOST); // Recuperacao e atribuicao do nome da maquina ao objeto anterior
    printf("*************\nCliente: Meu nome: %s\n", hostname);

    // Recupera e exibe o IP da maquina
    PHOSTENT phe = gethostbyname(hostname);     
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        memcpy(&ipAddr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("Cliente: Meu endereco IP (%d): %s\n", i, inet_ntoa(ipAddr));        
    }
    
    // Define o tipo de conexao
    ZeroMemory(&hintsForGetAddrInfo, sizeof(hintsForGetAddrInfo));
    hintsForGetAddrInfo.ai_family = AF_INET; // Endereço da família IPv4
    hintsForGetAddrInfo.ai_socktype = SOCK_STREAM; // Stream de bytes bidirecional
    hintsForGetAddrInfo.ai_protocol = IPPROTO_TCP; // Protocolo TCP (Só pode ser usado em conjunto com SOCK_STREAM + AF_INET/AF_INET6)

    // Resolve endereco e porta do servidor
    iResult = getaddrinfo(IP_SERVIDOR, PORTA_DEFAULT, &hintsForGetAddrInfo, &result);
    if (iResult != 0) {
        printf("getaddrinfo() falhou com o erro: %d\n", iResult);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Tenta se conectar ao endereco do servidor
    printf("*************\nTentando se conectar ao servidor de endereco %s na porta %s...\n", IP_SERVIDOR, PORTA_DEFAULT);
    for(ptr = result; ptr != NULL ; ptr = ptr->ai_next) {

        /*cria um socket para se conectar ao servidor*/
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Socket falhou com o erro: %d\n", WSAGetLastError());
            WSACleanup();
            system("pause");
            return 1;
        }

        // Tenta se conectar ao servidor
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Nao foi possivel se conectar ao servidor!\n");
        system("pause");
        WSACleanup();
        return 1;
    }    

    /*********************************************************************
     Estabeleceu conexao com o servidor e pode trocar dados*/

    int i = 0;
    file = fopen(".//data.txt", "r");
    char* fgetsStatus;
    ZeroMemory(sendbuf, sizeof(sendbuf));
    while(!kbhit()) {
        printf("Recado no. %04d enviado com sucesso!...\n", i);                     

        // monta msg com um numero e o nome da maquina em forma de string
        if (file == NULL || fgetsStatus == NULL) {
            snprintf(sendbuf, BUFFER_TAM, "Cliente %s envia recado no. %04d", hostname, i);
        } else {
            fgetsStatus = fgets(sendbuf, BUFFER_TAM, file);
            sendbuf[strcspn(sendbuf, "\n")] = '\0';
        }

        // Envia a mensagem, bufferizada
        iResult = send(ConnectSocket, sendbuf, strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("Oops! Falhou o envio com o erro: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            system("pause");
            return 1;
        }
        i++;
        Sleep(250);
    }
    /********************************************************************/
    /*Encerra a conexao*/
    fclose(file);
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Falhou encerrar a conexao com o erro: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    /*cleanup*/
    printf("Encerrando socket...");
    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
    return 0;
}
