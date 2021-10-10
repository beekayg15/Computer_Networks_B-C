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

    printf("\n\n\tYou are Ready to Play!!\n");

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
