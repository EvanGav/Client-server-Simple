#include "message.h"
#include <sys/types.h> 
#include <unistd.h>

#include <sys/stat.h>
#include<stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


pid_t pid ;
char pathname[80];
int fifo_serveur_fd;
int fifo_me_fd;

void creation_tube_nomme() {
    pid = getpid();
    sprintf(pathname,PATH_FORMAT,pid);
    mkfifo(pathname, 0666);
}

void envoyer_expression(){
    requete_client_serveur_t* requete = malloc(sizeof(requete_client_serveur_t)); //on alloue la mémoire à la requete
    requete->client_pid = pid;
    printf("entrez opération \n");
    fflush(stdout);
    int taille = read(0,requete->expression,BUFFER_SIZE); //on lit l'entrée standard pour pouvoir récuperer l'entrée utiliateur sus forme de string
    requete->expression[taille-1] = '\0';
    fifo_serveur_fd = open(FIFO_SERVEUR,O_WRONLY); //ouverture du fifo serveur
    write(fifo_serveur_fd,requete,sizeof(requete_client_serveur_t)); //écriture de la requete dans le fifo
}

void recevoir_resultat(){
    fifo_me_fd = open(pathname,O_RDONLY); //ouverture fifo du client
    char res[256];
    int taille = read(fifo_me_fd,res,256); //lecture du résultat
    res[taille] = '\0';
    printf("%s\n",res); //affichage du résultat
    fflush(stdout);
};
    
void terminer()
{
    remove( pathname );
}

int main(int argc, char** argv){

    creation_tube_nomme();
    envoyer_expression();
    recevoir_resultat();
    terminer();   
    return 0;
}
