#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> // Ajouté pour gethostbyname

#define MAX 128

// Changement : retourne char* et non char
char* random_word() {
    static char word[MAX];
    FILE *f = fopen("dico.txt", "r"); // Ajout de l'espace et de l'étoile
    if (!f) {
        perror("Erreur dico.txt");
        exit(1);
    }

    int lines = 0;
    while (fgets(word, MAX, f)) lines++;

    srand(time(NULL));
    int target = (lines > 0) ? rand() % lines : 0;

    rewind(f);
    for (int i = 0; i <= target; i++)
        fgets(word, MAX, f);

    word[strcspn(word, "\r\n")] = 0; // Nettoie \n et \r
    fclose(f);
    return word;
}

int main() {
    char *word = random_word();
    printf("[C1] Mot choisi : %s\n", word);

    // --- ENVOI VERS C2 ---
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in c2 = {0};
    c2.sin_family = AF_INET;
    c2.sin_port = htons(5000);

    // Résolution du nom "c2_caesar" (Docker DNS)
    struct hostent *host = gethostbyname("c2_caesar");
    if (host) {
        memcpy(&c2.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    } else {
        // Si DNS échoue, se rabattre sur le nom direct (Docker interne)
        // Sinon sur localhost puisqu'ils sont sur la même compose
        c2.sin_addr.s_addr = inet_addr("c2_caesar");
        if (c2.sin_addr.s_addr == (in_addr_t)-1) {
            // Dernière tentative : localhost
            c2.sin_addr.s_addr = inet_addr("127.0.0.1");
        }
    }

    printf("[C1] Tentative de connexion à C2...\n");
    // Attendre un peu que C2 soit prêt si nécessaire
    while(connect(sock, (struct sockaddr*)&c2, sizeof(c2)) < 0) {
        sleep(1);
    }

    send(sock, word, strlen(word), 0);
    printf("[C1] Mot envoyé à C2 : %s\n", word);
    close(sock);

    // Attendre que le mot transite complètement dans la chaîne
    printf("[C1] En attente de 5 secondes avant d'écouter le retour...\n");
    sleep(5);

    // --- ATTENTE DU RETOUR FINAL ---
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Évite l'erreur "Address already in use"

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5003);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&local, sizeof(local));
    listen(server, 1);

    printf("[C1] En attente du retour final sur le port 5003...\n");
    int client = accept(server, NULL, NULL);
    char buffer[MAX] = {0};
    recv(client, buffer, MAX, 0);

    printf("[C1] Succès ! Mot final reçu : %s\n", buffer);

    close(client);
    close(server);
    return 0;
}
