#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char caesar(char c, int shift) {
    if (c >= 'a' && c <= 'z')
        return 'a' + (c - 'a' + shift + 26) % 26;
    if (c >= 'A' && c <= 'Z') // Ajout pour les majuscules
        return 'A' + (c - 'A' + shift + 26) % 26;
    return c;
}

void caesar_cipher(char *s, int shift) {
    for (int i = 0; s[i]; i++)
        s[i] = caesar(s[i], shift);
}

int main() {
    char buffer[128];

    // --- PHASE SERVEUR (Reçoit de C1) ---
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5000);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&local, sizeof(local));
    listen(server, 1);

    printf("[C2] En attente de C1 sur le port 5000...\n");
    int client = accept(server, NULL, NULL);
    int len = recv(client, buffer, sizeof(buffer)-1, 0);
    if (len > 0) {
        buffer[len] = 0;
        printf("[C2] Reçu de C1 : %s\n", buffer);
        
        caesar_cipher(buffer, 5);
        printf("[C2] Après César (+5) : %s\n", buffer);

        // --- PHASE CLIENT (Envoie à C3 sur VM2) ---
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in c3 = {0};
        c3.sin_family = AF_INET;
        c3.sin_port = htons(5001);
        
        // ATTENTION : Ici, mettre l'IP de la VM2 !
        c3.sin_addr.s_addr = inet_addr("192.168.200.20"); 

        printf("[C2] Connexion à C3 (VM2)... \n");
        while(connect(sock, (struct sockaddr*)&c3, sizeof(c3)) < 0) {
            perror("Erreur connexion C3");
            sleep(2);
        }
        
        send(sock, buffer, strlen(buffer), 0);
        printf("[C2] Données envoyées à C3.\n");
        close(sock);
    }

    close(client);
    close(server);
    return 0;
}
