#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 128

int main() {
    char word[MAX];
    int compteur = 0;

    while (1) {
        compteur++;
        printf("\n==========================================\n");
        printf("   PARTIE N° %d\n", compteur);
        printf("==========================================\n");
        printf("[C1] Entrez le mot a envoyer (ou 'fin' pour quitter) : ");
	fflush(stdout);
        
        if (fgets(word, MAX, stdin) == NULL) break;
        word[strcspn(word, "\n")] = 0;

        // Condition d'arrêt
        if (strcmp(word, "fin") == 0) {
            compteur--; // On ne compte pas la partie "fin"
            printf("\n[C1] Arret du programme demandé.\n");
	    fflush(stdout);
            printf("[C1] Nombre total de parties realisees : %d\n", compteur);
	    fflush(stdout);
            break;
        }

        if (strlen(word) == 0) {
            printf("[C1] Erreur : Le mot ne peut pas etre vide.\n");
	    fflush(stdout);
            compteur--;
            continue;
        }

        // --- ENVOI VERS C2 ---
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in c2 = {0};
        c2.sin_family = AF_INET;
        c2.sin_port = htons(5000);

        struct hostent *host = gethostbyname("c2_caesar");
        if (host) {
            memcpy(&c2.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        } else {
            c2.sin_addr.s_addr = inet_addr("127.0.0.1");
        }

        if (connect(sock, (struct sockaddr*)&c2, sizeof(c2)) < 0) {
            perror("[C1] Erreur connexion C2");
	    fflush(stdout);
            close(sock);
            continue;
        }

        send(sock, word, strlen(word), 0);
        printf("[C1] Mot envoye à C2 : %s\n", word);
	fflush(stdout);
        close(sock);

        // --- ATTENTE DU RETOUR FINAL ---
        int server = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in local = {0};
        local.sin_family = AF_INET;
        local.sin_port = htons(5003);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(server, (struct sockaddr*)&local, sizeof(local)) < 0) {
            perror("[C1] Erreur bind 5003");
	    fflush(stdout);
            close(server);
            continue;
        }
        
        listen(server, 1);
        printf("[C1] En attente du retour de la chaine...\n");
	fflush(stdout);

        int client = accept(server, NULL, NULL);
        char buffer[MAX] = {0};
        recv(client, buffer, MAX, 0);

        printf("[C1] >>> SUCCES ! Mot final recu : %s\n", buffer);
	fflush(stdout);

        close(client);
        close(server);
    }

    return 0;
}
