#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>

#define PORT 1515

void openGame(int sockfd);
int getch(void);
void instructions(int sockfd);
void startGame(int sockfd);

int main(){
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

    openGame(client_socket);

    return 0;

}

void openGame(int sockfd) {
    system("clear");

    printf("\n\n\n\tBULLS AND COWS");
    printf("\n\n\t\t1.Start Game");
    printf("\n\n\t\t2.Instructions");
    printf("\n\n\t\t3.Quit");
    printf("\n\n\tPress the corresponding number to continue......");

    char choice = getch();
    char buffer[1024];

    switch(choice) {
        case '1': strcpy(buffer, "Continue");
                  send(sockfd, buffer, 1024, 0);
                  startGame(sockfd); break;
        case '2': instructions(sockfd); break;
        case '3': strcpy(buffer,"endgame");
        		  send(sockfd,buffer,1024,0);
                  close(sockfd);
        	      system("clear");
        	      exit(0);
        	   
        default: openGame(sockfd);

    }

    return;

}

void startGame(int sockfd) {
    system("clear");
    
    char buffer[1024];
    int n = 0;

    bzero(buffer, 1024);
    
    printf("\n\n\n\t\tGo on!! Make a Guess : ");
    while((buffer[n++] = getchar()) != '\n');

    send(sockfd, buffer, 1024, 0);

    if(strncmp("exit", buffer, 4) == 0) {
        openGame(sockfd);

    }

    bzero(buffer, 1024);
    recv(sockfd, buffer, 1024, 0);

    printf("\n\n\t\t%s", buffer);

    if(buffer[0] == 'C') {
        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        openGame(sockfd);

    } else {
        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        startGame(sockfd);

    }

    return;

}

void instructions(int sockfd) {
    system("clear");

    printf("\n\tInstructions");
    printf("\n\n'Bulls and Cows' is an old Code-Breaking mind game in which the player is engaged to figure out a 4 digit code by the trail and error method. The number of cows represent the digits that were guessed and present in the actual code but not in the exact position. Example:");
    printf("\n\n\t\t3564  (Guessed Number)");
    printf("\n\t\t/  \\ ");
    printf("\n\t\t5986  (Actual Code)");
    printf("\n\nThus the number of cows is 2. The number of bulls represent the digits that were guessed and present in the actual code and also the exact same position. Example:");
    printf("\n\n\t\t4365  (Guessed Number)");
    printf("\n\t\t |");
    printf("\n\t\t9312  (Actual Code)");
    printf("\n\nTherefor the number of bulls is 1. Thus the code can be found by analyzing the number of bulls and cows");
    printf("\n\nHave Fun!!!!");

    printf("\n\nPress any Key to continue......");
    getch();

    openGame(sockfd);

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
