
#include "message.h"

#include <sys/types.h> 
#include <unistd.h>

#include <sys/stat.h>
#include<stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

int fifo_me;
requete_client_serveur_t* requete;

void creation_tube_nomme() {
    mkfifo(FIFO_SERVEUR,0666);
}


void recevoir_requete(){
    requete=malloc(sizeof(requete_client_serveur_t)); //on alloue la mémoire à la requete
    fifo_me=open(FIFO_SERVEUR,O_RDONLY); //on ouvre le fifo du serveur
    read(fifo_me,requete,sizeof(requete_client_serveur_t)); //on lis la requete du fifo et on la met dans requete
}


int calculResultat(char* result)
{
    char** bc = malloc(sizeof(char*)); //on alloue la mémoire pour pouvoir executer bc plus tard
    bc[0] = "bc";
    bc[1] = NULL;

    int pipeBc[2]; //création des pipe
    pipe(pipeBc);
    int pipeRes[2];
    pipe(pipeRes);

    pid_t pid=fork();
    if(pid== 0){
        close(pipeRes[0]); //fermeture des entrées lecture/écritures non nécessaires
        close(pipeBc[1]);
        
        dup2(pipeBc[0],0); //redirection de l'entrée standard dans l'entrée lecture du pipe
        dup2(pipeRes[1],1); //redirection de la sortie standard dans l'entrée écriture du pipe
        close(pipeBc[0]); 
        close(pipeRes[1]);

        int res = execvp(bc[0],bc); //execution commande bc
        printf("%i",res);
        fflush(stdout);
    }
    else{
        pid_t pid2=fork();
        if(pid2==0){
            close(pipeBc[0]); //fermeture des entrées lecture/écritures non nécessaires
            close(pipeRes[1]);
            close(pipeRes[0]);

            dup2(pipeBc[1],1); //redirection de la sortie standard dans l'entrée écriture du pipe
            close(pipeBc[1]);

            char** echoArgs=malloc(sizeof(char*)*3); //on alloue la mémoire pour pouvoir effectuer la commande echo
            echoArgs[0]="echo";
            echoArgs[1]=requete->expression;
            echoArgs[2]=NULL;

            int res2 = execvp(echoArgs[0],echoArgs); //execution echo
            printf("%i", res2);
        }
        else{
            close(pipeBc[0]); //fermeture des entrées lecture/écritures non nécessaires
            close(pipeRes[1]);
            close(pipeBc[1]);
            
            dup2(pipeRes[0],0); //redirection de l'entrée standard dans l'entrée lecture du pipe
            close(pipeRes[0]);

            waitpid(pid2,NULL,0); //attente de fin de la commande echo
            fflush(stdout);
            
            int taille=read(0,result,256); //lecutre du résultat placé dans result
            result[taille-1]='\0';

            printf("%s", result);
            return taille;
        }


    }
}

void envoyer_resultat(){
    char* pathname=malloc(sizeof(char)*80); 
    sprintf(pathname,PATH_FORMAT,requete->client_pid);
    int fdClient=open(pathname,O_WRONLY); //on ouvre le fifo du client concerné
    char* resultat=malloc(sizeof(char)*80);
    int res=calculResultat(resultat);
    write(fdClient,resultat,res); //on écrit le resultat de la requete dans le fifo
    close(fdClient);
}

void terminer(){

}

int main(int argc, char** argv){
    
    creation_tube_nomme();
    
    while(1){
    recevoir_requete();
    envoyer_resultat();
    }

    terminer();
    
}
