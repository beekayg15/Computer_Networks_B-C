#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>

#define PORT 3232

void startPlayer(int sockfd);
int getch(void);

int main() {
    socklen_t addr_len;
    int client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(PF_INET, SOCK_STREAM, 0);

    if(client_socket < 0) {
        printf("Error While Creating Socket!!\n");
        exit(0);

    } else {
        printf("Socket Creation Successful!!\n");

    }

    addr_len = sizeof(server_address);
    bzero(&server_address, addr_len);

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, (struct sockaddr*)&server_address, addr_len) != 0){
        printf("Connection to Server Failed!!\n");
        exit(0);

    } else {
        printf("Connection to Server Successful!!\n");

    }

    printf("Press any Key to Continue......");
    getch();

    startPlayer(client_socket);

    return 0;

}

void startPlayer(int sockfd) {
    system("clear");

    char buffer[1024];
    char code[5];
    bzero(code, 5);
    bzero(buffer, 1024);
    int count = 0;
    int turn = 0;

    printf("\n\n\tYou are Ready to Play!!\n");

    printf("\n\n\t\tEnter your secret four digit code : ");
    while(count < 4) {
        char ch = getch();
        int flag = 1;
        for(int j=0; j<count; j++) {
            if(ch == code[j]) {
                flag = 0;
            }
        }
        if(flag == 1 && ch >= '1' && ch <= '9') {
            code[count++] = ch;
            printf("*");
        }
    }
    code[4] = '\0';
    send(sockfd, code, 4, 0);

    system("clear");
    printf("\n\n\t\t Waiting for Player 1 to Enter his secret Code...\n");

    recv(sockfd, buffer, 1024, 0);
    printf("\n\n\t\tYou are set to Guess the opponents Secret Code. Are you ready!!!");

    printf("\n\n\t\tPress any Key to Conitnue......");
    getch();

    while(1>0) {
        system("clear");
        int Found = 0;

        printf("\n\n\t\tTurn : %d\n\n\t\tWaiting for Player 1 to make a Guess!!...", ++turn);
        bzero(buffer, 1024);

        recv(sockfd, buffer, 1024, 0);
        printf("\n\n\t\t%s", buffer);

        char bull = buffer[20];
        if(bull == '4') {
            Found = 1;
            printf("\n\n\tPlayer 1 Has found Your Secret Code");
        }

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();
    
        system("clear");

        printf("\n\n\t\tTurn : %d", turn);

        if(Found == 1) {
            printf("\n\n\t\tBeware!! This is your Last turn to find the Code!");
        }

        int n = 0;

        bzero(buffer, 1024);
        
        printf("\n\n\n\t\tGo on!! Make a Guess : ");
        while((buffer[n++] = getchar()) != '\n');

        send(sockfd, buffer, 1024, 0);

        bzero(buffer, 1024);
        recv(sockfd, buffer, 1024, 0);

        printf("\n\n\t\t%s", buffer);

        if(Found == 1 && buffer[20] == '4') {
            printf("\n\n\t\tLuckily, You have managed to draw the Game!!!");
            printf("\n\n\t\tPress any Key to Conitnue......\n");
            getch();
            close(sockfd);
            exit(0);
        }

        if(Found == 1 && buffer[20] != '4') {
            printf("\n\n\t\tAlas, You have Lost the Game to Player 1!!!");
            printf("\n\n\t\tPress any Key to Conitnue......\n");
            getch();
            close(sockfd);
            exit(0);
        }

        if(Found == 0 && buffer[20] == '4') {
            printf("\n\n\t\tCongrats, You have Won the Game!!!");
            printf("\n\n\t\tPress any Key to Conitnue......\n");
            getch();
            close(sockfd);
            exit(0);
        }

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

    }

    exit(0);
    return;

}

int getch(void) {
    struct termios oldattr, newattr;
    int ch;

    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );

    return ch;

}
