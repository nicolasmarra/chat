#include "lib.h"

int main(int argc, char **argv) {
    int sockfd;              // descripteur de socket
    struct sockaddr_in dest; // structure d'adresse qui contiendra les
                             // parametres reseaux du destinataire

    fd_set readfs, readfs2;

    // verification du nombre d'arguments sur la ligne de commande
    if (argc != 4) {
        raler(0, "Usage : %s @dest num_port username\n", argv[0]);
    }

    // creation de la socket

    CHK(sockfd = socket(AF_INET, SOCK_STREAM, 0));

    // initialisation de la structure d'adresse du destinataire :
    char buf[2000];
    char username[50];
    strcpy(username, argv[3]);
    username[49] = '\0';
    socklen_t fromlen = sizeof(struct sockaddr_in);

    // famille d'adresse
    dest.sin_family = AF_INET;

    // adresse IPv4 du destinataire
    dest.sin_addr.s_addr = inet_addr(argv[1]);

    // port du destinataire
    int des_port = atoi(argv[2]);
    dest.sin_port = htons(des_port);

    CHK(connect(sockfd, (void *)&dest, fromlen));

    CHK(send(sockfd, username, 50, 0));

    /* Surveiller l'entrée standard (en lecture) */
    FD_ZERO(&readfs);
    FD_SET(0, &readfs);
    FD_SET(sockfd, &readfs);

    while (1) {

        readfs2 = readfs;
        int activite = select(sockfd + 1, &readfs2, 0, 0, 0);

        if ((activite < 0) && (errno != EINTR)) {
            raler(1, "erreur select -> ");
        }

        if (FD_ISSET(0, &readfs2)) {
            // envoi de la chaine
            bzero(buf, 2000);
            fgets(buf, 2000, stdin);
            if (strcmp(buf, "/quit\n") == 0) {
                CHK(close(sockfd));
                return 0;
            }
            CHK(send(sockfd, buf, 2000, 0));
            memset(buf, 0, 2000);
        }

        if (FD_ISSET(sockfd, &readfs2)) {

            // reception de la chaine de caracteres
            memset(buf, 0, 2000);
            bzero(buf, 2000);
            CHK(recv(sockfd, buf, 2000, 0));

            // affichage de la chaine de caracteres recue
            fprintf(stdout, "%s\n", buf);
            fflush(stdout);
        }
    }
    // fermeture de la socket
    CHK(close(sockfd));

    return 0;
}
