#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

char caesar(char c, int shift) {
    if (c >= 'a' && c <= 'z') return 'a' + (c - 'a' + shift + 26) % 26;
    if (c >= 'A' && c <= 'Z') return 'A' + (c - 'A' + shift + 26) % 26;
    return c;
}

void caesar_cipher(char *s, int shift) {
    for (int i = 0; s[i]; i++) s[i] = caesar(s[i], shift);
}

int main() {
    char buffer[128];
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(5000);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&local, sizeof(local));
    listen(server, 5);

    printf("[C2] Waiting for C1 on port 5000...\n");
    fflush(stdout);

    while (1) { // Start of infinite loop
        int client = accept(server, NULL, NULL);
        int len = recv(client, buffer, sizeof(buffer)-1, 0);
        if (len > 0) {
            buffer[len] = 0;
            printf("[C2] Received from C1: %s\n", buffer);
            fflush(stdout);

            caesar_cipher(buffer, 5);
            printf("[C2] After Caesar (+5): %s\n", buffer);
            fflush(stdout);

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in c3 = {0};
            c3.sin_family = AF_INET;
            c3.sin_port = htons(5001);
            c3.sin_addr.s_addr = inet_addr("192.168.200.20"); 

            printf("[C2] Connecting to C3 (VM2)...\n");
            fflush(stdout);

            if (connect(sock, (struct sockaddr*)&c3, sizeof(c3)) >= 0) {
                send(sock, buffer, strlen(buffer), 0);
                printf("[C2] Data sent to C3.\n");
                fflush(stdout);
		close(sock);
            } else {
                perror("C3 connection error");
            }
        }
        close(client); // Close the client but NOT the server
    }
    return 0;
}
