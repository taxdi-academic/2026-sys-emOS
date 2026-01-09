#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char caesar(char c, int shift) {
    if (c >= 'a' && c <= 'z')
        return 'a' + (c - 'a' + shift + 26) % 26;
    if (c >= 'A' && c <= 'Z') // Support des majuscules
        return 'A' + (c - 'A' + shift + 26) % 26;
    return c;
}

void caesar_cipher(char *s, int shift) {
    for (int i = 0; s[i]; i++)
        s[i] = caesar(s[i], shift);
}

void xor_cipher(unsigned char *buf, int len) {
    char key[] = "Nirvana"; // Attention à la casse "Nirvana" vs "nirvana" (doit être identique à C3)
    int klen = strlen(key);
    for (int i = 0; i < len; i++)
        buf[i] ^= key[i % klen];
}

int main() {
    unsigned char buffer[128];

    // --- SERVEUR : Reçoit de C3 (VM2 Port 5002) ---
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5002);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&local, sizeof(local));
    listen(server, 1);

    printf("[C4] En attente de C3 sur le port 5002...\n");
    int client = accept(server, NULL, NULL);
    int len = recv(client, buffer, sizeof(buffer)-1, 0);

    // --- DECHIFFREMENT ---
    xor_cipher(buffer, len);
    buffer[len] = 0; // On peut remettre le null-terminator ici car le XOR est fini
    caesar_cipher((char*)buffer, -5);
    printf("[C4] Mot déchiffré : %s\n", buffer);

    // --- CLIENT : Retourne à C1 (Sur VM1 Port 5003) ---
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in c1 = {0};
    c1.sin_family = AF_INET;
    c1.sin_port = htons(5003);
    
    // IP de VM1 où C1 est exposé
    c1.sin_addr.s_addr = inet_addr("192.168.200.24"); 

    printf("[C4] Renvoi du résultat à C1 (VM1) sur 192.168.200.24:5003...\n");
    int retries = 0;
    while(connect(sock, (struct sockaddr*)&c1, sizeof(c1)) < 0) {
        retries++;
        printf("[C4] Tentative de connexion échouée (#%d), nouvelle tentative dans 2s...\n", retries);
        sleep(2);
        if (retries > 30) {
            fprintf(stderr, "[C4] ERREUR : Impossible de se connecter à C1 après 30 tentatives\n");
            break;
        }
    }
    
    if (retries <= 30) {
        send(sock, buffer, strlen((char*)buffer), 0);
        printf("[C4] Résultat envoyé à C1 avec succès\n");
    }

    close(sock);
    close(client);
    close(server);
    return 0;
}
