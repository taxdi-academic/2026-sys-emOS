#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 128

void xor_cipher(char data, int data_len, const charkey) {
    int key_len = strlen(key);
    for (int i = 0; i < data_len; i++) data[i] = data[i] ^ key[i % key_len];
}

int main() {
    char buffer[MAX];
    const char key = "Nirvana";
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5001);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr)&local, sizeof(local));
    listen(server, 5);

    printf("[C3] En attente de données de VM1 (C2) sur le port 5001...\n");
    fflush(stdout);

    while (1) { // Début de la boucle infinie
        int client = accept(server, NULL, NULL);
        int len = recv(client, buffer, MAX, 0);
        if (len > 0) {
            printf("[C3] Données reçues (longueur: %d)\n", len);
        fflush(stdout);

            xor_cipher(buffer, len, key);
            printf("[C3] XOR appliqué avec la clé '%s'\n", key);
        fflush(stdout);

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in c4 = {0};
            c4.sin_family = AF_INET;
            c4.sin_port = htons(5002);

            struct hostent host = gethostbyname("c4_decipher");
            if (host) memcpy(&c4.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
            else c4.sin_addr.s_addr = inet_addr("127.0.0.1");

            printf("[C3] Transmission à C4...\n");
        fflush(stdout);

            if (connect(sock, (struct sockaddr)&c4, sizeof(c4)) >= 0) {
                send(sock, buffer, len, 0);
                close(sock);
            }
        }
        close(client);
    }
    return 0;
}
