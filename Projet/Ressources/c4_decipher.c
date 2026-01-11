#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX 128

/* ----------- CESAR ----------- */

char caesar(char c, int shift) {
    if (c >= 'a' && c <= 'z')
        return 'a' + (c - 'a' + shift + 26) % 26;
    if (c >= 'A' && c <= 'Z')
        return 'A' + (c - 'A' + shift + 26) % 26;
    return c;
}

void caesar_cipher(char *s, int shift) {
    for (int i = 0; s[i]; i++)
        s[i] = caesar(s[i], shift);
}

/* ----------- XOR ----------- */

void xor_cipher(unsigned char *buf, int len) {
    const char key[] = "Nirvana";
    int klen = strlen(key);
    for (int i = 0; i < len; i++)
        buf[i] ^= key[i % klen];
}

/* ----------- MAIN ----------- */

int main() {
    unsigned char buffer[MAX];

    /* ===== SERVEUR : reçoit depuis C3 ===== */

    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("[C4] socket server");
        return 1;
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5002);
    local.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr*)&local, sizeof(local)) < 0) {
        perror("[C4] bind");
        return 1;
    }

    listen(server, 1);

    printf("[C4] En attente de C3 sur le port 5002...\n");
    int client = accept(server, NULL, NULL);
    if (client < 0) {
        perror("[C4] accept");
        return 1;
    }

    int len = recv(client, buffer, MAX, 0);
    if (len <= 0) {
        perror("[C4] recv");
        close(client);
        close(server);
        return 1;
    }

    /* ===== DECHIFFREMENT ===== */

    xor_cipher(buffer, len);
    buffer[len] = 0;              // terminé APRÈS XOR
    caesar_cipher((char*)buffer, -5);

    printf("[C4] Mot déchiffré : %s\n", buffer);

    close(client);
    close(server);

    /* ===== CLIENT : renvoi vers C1 ===== */

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[C4] socket client");
        return 1;
    }

    struct sockaddr_in c1 = {0};
    c1.sin_family = AF_INET;
    c1.sin_port = htons(5003);

    if (inet_pton(AF_INET, "192.168.200.24", &c1.sin_addr) != 1) {
        perror("[C4] inet_pton");
        return 1;
    }

    printf("[C4] Connexion à C1 (192.168.200.24:5003)...\n");

    while (connect(sock, (struct sockaddr*)&c1, sizeof(c1)) < 0) {
        perror("[C4] connexion C1");
        sleep(2);
    }

    send(sock, buffer, strlen((char*)buffer), 0);
    printf("[C4] Résultat renvoyé à C1\n");

    close(sock);
    return 0;
}
