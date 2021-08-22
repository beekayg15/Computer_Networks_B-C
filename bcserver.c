#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 1515

void initiateGame(int sockfd);
int numberOfBulls(char guess[], char num[]);
int numberOfCows(char guess[], char num[]);
int valid(char guess[]);

int main() {
    socklen_t addr_len;
    struct sockaddr_in server_address;
    int server_socket;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);

    if(server_socket < 0) {
        printf("Error While Creating Socket!!\n");
        exit(0);

    } else {
        printf("Socket Creation Successful!!\n");

    }

    addr_len = sizeof(server_address);
    bzero(&server_address, addr_len);

    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    if(bind(server_socket, (struct sockaddr*)&server_address, addr_len) != 0) {
        printf("Binding Failed!!\n");
        exit(0);

    } else {
        printf("Binding Successful!!\n");

    }

    if(listen(server_socket, 2) != 0) {
        printf("Unable to Listen to Clients!!\n");
        exit(0);

    } else {
        printf("Waiting for Clients!!\n");

    }

    int client_socket;
    struct sockaddr_in client_address;

    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &addr_len);

    if(client_socket < 0) {
        printf("Error in Establishing Connection with the Client!!\n");
        exit(0);

    } else {
        printf("Connection with Client Established Successfully!!\n");

    }

    initiateGame(client_socket);

    return 0;

}

void initiateGame(int sockfd) {
    int number_of_turns = 0;
    char buffer[1024];

    char number[4] = "    ";
    int count = 0;

    while(count < 4) {
        int flag = 0;
        int r = rand() % 9 + 49;
        
        for(int index=0; index<count; index++) {
            if((char)r == number[index]) {
                flag = 1;

            }

        }

        if(flag == 0) {
            number[count] = (char)r;
            count++;

        }

    }

    printf("%c%c%c%c\n", number[0], number[1], number[2], number[3]);

    while(1) {
        bzero(buffer, 1024);

        recv(sockfd, buffer, 1024, 0);
        

        if(strncmp("exit", buffer, 4) == 0) {
            printf("The client has left the current game!!\n");
            initiateGame(sockfd);
        }
        
        if(strncmp("endgame", buffer, 7) == 0) {
            printf("The client has ended the game!!\n");
            close(sockfd);
            exit(0);
        }
        
        printf("Turn Number : %d, %s",number_of_turns, buffer);

        number_of_turns++;

        int bulls = numberOfBulls(buffer, number);
        int cows = numberOfCows(buffer, number);
        
        printf("Number of Bulls: %d\n", bulls);
        printf("Number of Cows: %d\n", cows);

        if(bulls == 4) {
            bzero(buffer, 1024);

            char temp[] = "Congrats!! Number of Turns Taken: _\n";

            for(int z=0; temp[z]!='\n'; z++) {
                buffer[z] = temp[z];

            }

            char s[4]; 
            sprintf(s,"%d", number_of_turns);
            buffer[34] = s[0];

            for(int z=1; z<5 && s[z-1]!='\0'; z++) {
                if(s[z] == '\0') {
                    buffer[z+34] = '\n';

                } else {
                    buffer[z+34] = s[z];

                }

            }

            send(sockfd, buffer, 1024, 0);

            initiateGame(sockfd);

        }

        bzero(buffer, 1024);

        char temp[] = "Bulls: _; Cows: _; In turn: _\n\0";

        for(int z=0; temp[z]!='\0'; z++) {
            buffer[z] = temp[z];

        }

        buffer[7] = (char)(bulls + 48);
        buffer[16] = (char)(cows + 48);

        char s[4]; 
        sprintf(s,"%d", number_of_turns);
        buffer[28] = s[0];

        for(int z=1; z<5 && s[z-1]!='\0'; z++) {
            if(s[z] == '\0') {
                buffer[z+28] = '\n';

            } else {
                buffer[z+28] = s[z];

            }

        }

        send(sockfd, buffer, 1024, 0);

    }

    return;

}

int numberOfBulls(char guess[], char num[]) {
    int bulls = 0;

    for(int index=0; index<4; index++) {
        if(guess[index] == num[index]) {
            bulls++;

        }

    }

    return bulls;

}

int numberOfCows(char guess[], char num[]) {
    int cows = 0;

    for(int index1=0; index1<4; index1++) {
        for(int index2=0; index2<4; index2++) {
            if((guess[index1] == num[index2]) && (index1 != index2)) {
                cows++;

            }

        }

    }

    return cows;

}
