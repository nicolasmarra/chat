#include "lib.h"
#define CLIENTS_MAX 7

int validation_send(char *message, char *username, char *messageE) {
    int size = strlen(message);
    char aux[6];
    memset(aux, 0, 6);
    if (size > 5) {

        for (int i = 0; i < 5; i++) {
            aux[i] = message[i];
        }

        aux[5] = '\0';

        if (strstr(message, "-u")) {
            int i = 0;

            int a = 0;
            for (i = 5; i < size; i++) {

                if (message[i] == '-') {
                    break;
                }
                messageE[a] = message[i];
                a++;
            }
            messageE[a] = '\0';
            if (message[i + 1] == 'u') {
                int c = 0;
                for (int j = i + 2; j < size; j++) {
                    username[c] = message[j];
                    c++;
                }
                username[c] = '\0';

            } else {
                return 0;
            }
        } else {
            return 0;
        }

    } else {
        return 0;
    }

    return 1;
}

int main(int argc, char **argv) {

    int sockfd, socka;                // descripteur de socket
    int sock_client, clients_sock[7]; // descripteur de socket client
    char buf[2000]; // espace necessaire pour stocker le message recu
    char username[7][50];
    // taille d'une structure sockaddr_in utile pour la fonction recvfrom
    socklen_t fromlen = sizeof(struct sockaddr_in);

    struct sockaddr_in my_addr; // structure d'adresse qui contiendra les param
                                // reseaux du recepteur
    struct sockaddr_in client;  // structure d'adresse qui contiendra les param
                                // reseaux de l'expediteur

    fd_set readfs, readfs2;

    // verification du nombre d'arguments sur la ligne de commande
    if (argc != 2) {
        raler(0, "Usage: %s port_local\n", argv[0]);
    }

    // initialisiation à 0 de la socket de tous les clients
    for (int i = 0; i < CLIENTS_MAX; i++) {
        clients_sock[i] = 0;
    }

    // creation de la socket
    CHK(sockfd = socket(AF_INET, SOCK_STREAM, 0));

    // initialisation de la structure d'adresse du recepteur (pg local)

    // famille d'adresse
    my_addr.sin_family = AF_INET;

    // recuperation du port du recepteur
    int port = atoi(argv[1]);
    my_addr.sin_port = htons((port));

    // adresse IPv4 du recepteur
    my_addr.sin_addr.s_addr = INADDR_ANY;
    // target = "127.0.0.1"
    // inet_aton("127.0.0.1", &(my_addr.sin_addr));

    // association de la socket et des param reseaux du recepteur
    if (bind(sockfd, (const struct sockaddr *)&my_addr, fromlen) != 0) {
        raler(1, "erreur lors de l'appel a bind -> ");
    }

    printf("On écoute dans le port %d\n", port);

    // nombre de connexions pouvant être mises en attente

    if (listen(sockfd, 5) == -1) {
        raler(1, "erreur lors de l'attente de connexion ->");
    }

    printf("On attend les connexions...\nNb utilisateurs maximum = %d\n",
           CLIENTS_MAX);

    int sock;
    while (1) {

        /* Surveiller l'entrée standard (en lecture) */

        // On remet à zero

        FD_ZERO(&readfs);

        FD_SET(0, &readfs);
        FD_SET(sockfd, &readfs);

        socka = sockfd;

        for (int i = 0; i < CLIENTS_MAX; i++) {
            // socket du client
            sock = clients_sock[i];

            // si la socket du client est valide (supérieure à 0)
            // alors on l'ajoute à la liste de socket
            if (sock > 0)
                FD_SET(sock, &readfs);

            // on choisit le nombre de descripteur le plus élevé

            if (sock > socka)
                socka = sock;
        }

        readfs2 = readfs;
        // on attend une activité sur l'un des sockets
        // timeout est null, alors on attend indéfiniment

        int activite = select(socka + 1, &readfs2, 0, 0, 0);

        if ((activite < 0) && (errno != EINTR)) {
            raler(1, "erreur select -> ");
        }

        // s'il se passe quelque chose, alors il y a une connexion entrante
        if (FD_ISSET(sockfd, &readfs2)) {

            // reception de la chaine de caracteres
            // accepter une demande de connexion entrante

            if ((sock_client = accept(sockfd, (void *)&client, &fromlen)) < 0) {
                raler(1, "erreur lors de l'appel à la fonction accept -> ");
            }

            printf("Nouvelle connexion, descripteur de socket : %d, adresse ip "
                   ": %s, numéro de port: %d\n",
                   sock_client, inet_ntoa(client.sin_addr),
                   ntohs(client.sin_port));

            CHK(send(sock_client, buf, 2000, 0));

            // ajouter la nouvelle socket au  tableau de sockets

            for (int i = 0; i < CLIENTS_MAX; i++) {
                // si la socket est vide
                if (clients_sock[i] == 0) {
                    clients_sock[i] = sock_client;
                    CHK(recv(sock_client, username[i], 50, 0));
                    printf("user n°%d - connexion: %s vient de se connecter\n",
                           i + 1, username[i]);
                    strcat(username[i], ": ");
                    username[i][49] = '\0';
                    break;
                }
            }
        }

        // sinon c'est une opération sur une autre socket

        for (int i = 0; i < CLIENTS_MAX; i++) {
            sock = clients_sock[i];

            if (FD_ISSET(sock, &readfs2)) {
                // vérifier si c'est toujours disponible et lire le message
                int res;
                bzero(buf, 2000);
                if ((res = read(sock, buf, 2000)) == 0) {
                    // un client s'est déconnecté, vérifier ses informations
                    getpeername(sock, (void *)&client, &fromlen);

                    printf("Client déconnecté, adresse ip "
                           ": %s, numéro de port: %d\n",
                           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

                    CHK(close(sock));
                    clients_sock[i] = 0;
                }

                // sinon, on envoie à tous les clients le message arrivé
                else {

                    char user_p[50];
                    char message_p[2000];

                    //Afficher les clients en ligne
                    if (strcmp(buf, "/online\n") == 0) {
                        bzero(buf, 2000);

                        CHK(send(clients_sock[i], "Clients en ligne: \n",
                                 strlen("Clients en ligne: \n"), 0));
                        char _username[7][50];
                        for (int j = 0; j < CLIENTS_MAX; j++) {
                            strcpy(_username[j], username[j]);
                            int size = strlen(username[j]);
                            _username[j][size - 2] = '\n';
                            _username[j][size - 1] = '\0';

                            if (clients_sock[j] != 0) {

                                CHK(send(clients_sock[i], _username[j],
                                         strlen(_username[j]), 0));
                            }
                        }

                    } 

                    // Envoyer des messages privés
                    
                    else if (validation_send(buf, user_p, message_p) == 1) {
                        char _username[7][50];
                        int c = 0;
                        int v = 0;
                        for (int j = 0; j < CLIENTS_MAX; j++) {
                            strcpy(_username[j], username[j]);
                            int size = strlen(username[j]);
                            _username[j][size - 2] = '\n';
                            _username[j][size - 1] = '\0';

                            if (strcmp(_username[j], user_p) == 0) {
                                c = j;
                                v = 1;
                                break;
                            }
                        }

                        char aux[2050];
                        char *message = strcat(aux, username[i]);

                        message = strcat(aux, message_p);

                        if (clients_sock[c] != 0 && v == 1)
                            CHK(send(clients_sock[c], message, strlen(message),
                                     0));
                        else
                            CHK(send(
                                clients_sock[i], "L'utilisateur n'existe pas\n",
                                strlen("L'utilisateur n'existe pas\n"), 0));
                        bzero(message, 2000);
                        bzero(aux, 2000);
                        bzero(message_p, 2000);

                    }
                    
                    //Envoyer des messages à tous les clients
                    //Sauf à celui qui veut envoyer (expéditeur)
                    else {

                        char aux[2050];
                        char *message = strcat(aux, username[i]);

                        message = strcat(aux, buf);

                        for (int j = 0; j < CLIENTS_MAX; j++) {
                            if (clients_sock[j] != 0 &&
                                clients_sock[j] != clients_sock[i])
                                CHK(send(clients_sock[j], message,
                                         strlen(message), 0));
                        }
                        bzero(message, strlen(message));
                    }
                }
            }
        }
    }

    // fermeture de la socket
    CHK(close(sockfd));

    for (int i = 0; i < CLIENTS_MAX; i++) {
        if (clients_sock[i] != 0)
            CHK(close(clients_sock[i]));
    }

    return 0;
}
