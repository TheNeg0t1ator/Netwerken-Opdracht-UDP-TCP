#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define PORT 12345 // het poortnummer waarop de UDP-server luistert
#define BUF_SIZE 1024 // de maximale grootte van de buffer

int main() {
    struct sockaddr_in server_addr, client_addr;
    int sockfd, recvlen, highest_num = 0;
    socklen_t addrlen = sizeof(client_addr);
    char buf[BUF_SIZE];
    struct timeval tv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Waiting for message...\n");

        memset(buf, 0, BUF_SIZE);

        recvlen = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen);

        if (recvlen > 0) {
            

            if (strcmp(buf, "GO") == 0) {
                

                highest_num = 0;
                tv.tv_sec = 1;
                tv.tv_usec = 0;

                while (1) {
                    sendto(sockfd, &highest_num, sizeof(int), 0, (struct sockaddr *)&client_addr, addrlen);
                    
                    if (select(sockfd + 1, &tv, NULL, NULL, NULL) <= 0) {
                        // Er is geen inkomende data meer ontvangen binnen de timeout van 1 seconde.
                        // Stuur het hoogste ontvangen getal 3 keer terug en stop met wachten.
                        for (int i = 0; i < 3; i++) {
                            sendto(sockfd, &highest_num, sizeof(int), 0, (struct sockaddr *)&client_addr, addrlen);
                            
                        }
                        break;
                    }

                    // Er is inkomende data ontvangen binnen de timeout van 1 seconde.
                    // Ontvang het inkomende getal en update het hoogste ontvangen getal indien nodig.
                    int num;
                    recvfrom(sockfd, &num, sizeof(int), 0, (struct sockaddr *)&client_addr, &addrlen);
                    

                    if (num > highest_num) {
                        highest_num = num;
                    }

                    if (strcmp(buf, "OK") == 0) {
                        
                        break;
                    }
                }
            }
        }
    }

    close(sockfd);
    return 0;
}