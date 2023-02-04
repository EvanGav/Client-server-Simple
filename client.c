#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#define PORT 12345
#define SERV "127.0.0.1" // adresse du serveur

struct hostent *server;
int sock;
struct sockaddr_in sockaddServer;
char mess[80];

void creer_socket(){
    server = gethostbyname(SERV);
    bzero(&sockaddServer, sizeof(sockaddServer));
    sockaddServer.sin_family=AF_INET;
    sockaddServer.sin_port=htons(PORT);
    sockaddServer.sin_addr.s_addr=*(long*)server->h_addr;
    bzero(&(sockaddServer.sin_zero),8);
    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}

int main(){
    creer_socket();
    connect(sock,(struct sockaddr*) &sockaddServer,sizeof(sockaddServer));
    while(strncmp(mess,"fin",3)!=0){
        printf("client : écris un message \n");
        fflush(stdout);
        int nb=read(0,mess,80);
        write(sock,mess,80);
        nb=read(sock,mess,80);
        mess[nb]='\0';
        printf("le serveur me dit : %s",mess);
        fflush(stdout);
    }
    close(sock);
}
