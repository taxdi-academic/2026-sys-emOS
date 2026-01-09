#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 128

// Fonction de chiffrement XOR
void xor_cipher(char *data, int data_len, const char *key) {
    int key_len = strlen(key);
    for (int i = 0; i < data_len; i++) {
        data[i] = data[i] ^ key[i % key_len];
    }
}

int main() {
    char buffer[MAX];
    const char *key = "Nirvana";

    // --- PHASE SERVEUR (Écoute sur VM2 port 5001) ---
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5001); // Port défini dans ton tableau pour C3
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&local, sizeof(local));
    listen(server, 1);

    printf("[C3] En attente de données de VM1 (C2) sur le port 5001...\n");
    int client = accept(server, NULL, NULL);
    
    // On récupère la longueur réelle reçue
    int len = recv(client, buffer, MAX, 0);
    if (len > 0) {
        printf("[C3] Données reçues (longueur: %d)\n", len);

        // Application du XOR
        xor_cipher(buffer, len, key);
        printf("[C3] XOR appliqué avec la clé '%s'\n", key);

        // --- PHASE CLIENT (Envoie à C4 sur la même VM) ---
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in c4 = {0};
        c4.sin_family = AF_INET;
        c4.sin_port = htons(5002);

        // Comme C4 est sur la même VM, on peut utiliser le DNS Docker
        struct hostent *host = gethostbyname("c4_decipher");
        if (host) 
            memcpy(&c4.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        else 
            c4.sin_addr.s_addr = inet_addr("127.0.0.1");

        printf("[C3] Transmission à C4...\n");
        while(connect(sock, (struct sockaddr*)&c4, sizeof(c4)) < 0) {
            sleep(1);
        }

        // On envoie 'len' octets (très important pour le binaire)
        send(sock, buffer, len, 0);
        close(sock);
    }

    close(client);
    close(server);
    return 0;
}
