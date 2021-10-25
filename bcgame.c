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
void startMultiGame(int sockfd);
void leaderbd(int sockfd);
void retrieveSLB(int sockfd);
void retrieveMLB(int sockfd);
void login(int sockfd);
void loginScreen(int sockfd);
void register_user(int sockfd);

/*
Main Function
Parameters: None
Description: Driver Function to Establish Socket Connection
*/
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

    loginScreen(client_socket);

    return 0;

}

/*
Login Screen Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: The Function Displays the Login Page from which the User can choose to Login or Sign Up
*/
void loginScreen(int sockfd) {
    system("clear");

    printf("\n\n\n\tBULLS AND COWS - LOGIN");
    
    char c;
	printf("\n\n\t\tPress any key to continue..... \n\n\t\tIf you are a new user, press 'r' to register.....");
	c = getch();

    char buffer[1024];
    bzero(buffer, 1024);

    if(c == 'r') {
        strcpy(buffer, "1");
        send(sockfd, buffer, 1024, 0);
        register_user(sockfd);
    } 

    
    strcpy(buffer, "2");
    send(sockfd, buffer, 1024, 0);
    login(sockfd);

}

/*
Login Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: To Login into the Account of an already Registered User
*/
void login(int sockfd) {
    system("clear");
	printf("\n\n\tBULLS AND COWS - LOG IN");

    char buffer[1024];
    bzero(buffer, 1024);

    char pname[100], ppassword[100];
    bzero(pname, 100);
    bzero(ppassword, 100);

    printf("\n\n\t\tEnter user name : ");

    int i;
    for(i=0;(pname[i]=getchar())!='\n';i++);
	pname[i] = '\0';
    send(sockfd, pname, 100, 0);

    printf("\n\n\t\tEnter password : ");

    for(i=0;(ppassword[i]=getchar())!='\n';i++);
	ppassword[i] = '\0';
    send(sockfd, ppassword, 100, 0);

    recv(sockfd, buffer, 1024, 0);

    if(buffer[0] == '1') {
        printf("\n\n\t\tYour Authentication was Successful!!");
        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        openGame(sockfd);
    }

    printf("\n\n\t\tYour Authentication Failed!!");
    printf("\n\n\t\tPress any Key to Try Again or 'r' to Under Go Registration...");
    char x = getch();

    bzero(buffer, 1024);
    if(x == 'r') {
        strcpy(buffer, "1");
        send(sockfd, buffer, 1024, 0);
        register_user(sockfd);

    } else {
        strcpy(buffer, "2");
        send(sockfd, buffer, 1024, 0);
        login(sockfd);
    }

}

/*
Register Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: To Register an User and create a Profile for the User
*/
void register_user(int sockfd) {
    system("clear");
    printf("\n\n\tBULLS AND COWS - REGISTRATION");

    char buffer[1024];
    bzero(buffer, 1024);

    char pname[100], ppassword[100];
    bzero(pname, 100);
    bzero(ppassword, 100);

    printf("\n\n\t\tEnter user name : ");

    int i;
    for(i=0;(pname[i]=getchar())!='\n';i++);
	pname[i] = '\0';

    send(sockfd, pname, 100, 0);

    recv(sockfd, buffer, 1024, 0);

    if(strncmp("Invalid", buffer, 7) == 0) {
        printf("\n\n\t\tUser name already taken");

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        register_user(sockfd);

    }

    printf("\n\n\t\tEnter password : ");

    for(i=0;(ppassword[i]=getchar())!='\n';i++);
	ppassword[i] = '\0';

    send(sockfd, ppassword, 100, 0);

    bzero(buffer, 1024);
    recv(sockfd, buffer, 1024, 0);

    printf("\n\n\t\tRegistration Successful!! Your user_id is %s\n", buffer);

    printf("\n\n\t\tPress any Key to Conitnue......");
    getch();

    login(sockfd);

}

/*
Open Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It displays the Home Screen to the User and provides Various Choices
*/
void openGame(int sockfd) {
    system("clear");

    printf("\n\n\n\tBULLS AND COWS");
    printf("\n\n\t\t1.Single Player");
    printf("\n\n\t\t2.Multi Player");
    printf("\n\n\t\t3.Instructions");
    printf("\n\n\t\t4.Leaderboard");
    printf("\n\n\t\t5.Log Out");
    printf("\n\n\t\t6.Quit");
    printf("\n\n\tPress the corresponding number to continue......");

    char choice = getch();
    char buffer[1024];

    switch(choice) {
        case '1': strcpy(buffer, "sContinue");
                  send(sockfd, buffer, 1024, 0);
                  startGame(sockfd); break;
        case '2': strcpy(buffer, "mContinue");
                  send(sockfd, buffer, 1024, 0);
                  startMultiGame(sockfd); break;
        case '3': instructions(sockfd); break;
        case '4': strcpy(buffer, "lContinue");
                  send(sockfd, buffer, 1024, 0);
                  leaderbd(sockfd);break;
        case '5': strcpy(buffer, "rContinue");
                  send(sockfd, buffer, 1024, 0);
                  loginScreen(sockfd); break;
        case '6': strcpy(buffer,"endgame");
        		  send(sockfd,buffer,1024,0);
                  close(sockfd);
        	      system("clear");
        	      exit(0);
        	   
        default: openGame(sockfd);

    }

    return;

}

/*
Start Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It interacts with the User to provide the Single Player Experience
*/
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

/*
Start Multiplayer Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It interacts with the User to provide the Single Player Experience
*/
void startMultiGame(int sockfd) {
    system("clear");

    printf("\n\n\t\tYou have entered Multiplayer Mode\n\n\n\t\tWaiting for Player 2 to establish connection with the server...\n");

    char buffer[1024];
    bzero(buffer, 1024);
    recv(sockfd, buffer, 1024, 0);

    if(buffer[0] == 'f') {
        printf("\n\n\t\tError while trying to connect with second player!!\n");

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        openGame(sockfd);

    } else {
        printf("\n\n\t\tConnection with second player has been Established!!\n");

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();
        system("clear");
        
    }

    system("clear");

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
    printf("\n\n\t\tWaiting for Player 2 to Enter his secret Code...\n");

    recv(sockfd, buffer, 1024, 0);
    printf("\n\n\t\tYou are set to Guess the opponents Secret Code. Are you ready!!!");

    printf("\n\n\t\tPress any Key to Conitnue......");
    getch();

    while(1>0) {
        system("clear");
        int Found = 0;

        printf("\n\n\t\tTurn : %d", ++turn);

        int n = 0;

        bzero(buffer, 1024);
        
        printf("\n\n\n\t\tGo on!! Make a Guess : ");
        while((buffer[n++] = getchar()) != '\n');

        send(sockfd, buffer, 1024, 0);

        bzero(buffer, 1024);
        recv(sockfd, buffer, 1024, 0);

        printf("\n\n\t\t%s", buffer);

        if(buffer[20] == '4') {
            Found = 1;
            printf("\n\n\t\tCongrats!! You have Discovered the Secret Code\n\t\tLet's Wait and See if Player 2 is able to Draw the Game");
        }

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();

        system("clear");

        printf("\n\n\t\tTurn : %d\n\n\t\tWaiting for Player 2 to make a Guess!!...\n", turn);
        bzero(buffer, 1024);

        recv(sockfd, buffer, 1024, 0);
        printf("\n\n\t\t%s", buffer);

        if(Found == 1 && buffer[20] == '4') {
            printf("\n\n\t\tUnfortunately, Player 2 managed to Draw the Game!!!");
            printf("\n\n\t\tPress any Key to Conitnue......");
            getch();
            openGame(sockfd);
        }

        if(Found == 0 && buffer[20] == '4') {
            printf("\n\n\t\tAlas, You have Lost the Game to Player 2!!!");
            printf("\n\n\t\tPress any Key to Conitnue......");
            getch();
            openGame(sockfd);
        }

        if(Found == 1 && buffer[20] != '4') {
            printf("\n\n\t\tCongrats, You have Won the Game!!!");
            printf("\n\n\t\tPress any Key to Conitnue......");
            getch();
            openGame(sockfd);
        }

        printf("\n\n\t\tPress any Key to Conitnue......");
        getch();
    }

    exit(0);
    return;

}

/*
Start Multiplayer Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It provides extensive details on how to play the game and its rules
*/
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

/*
Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It provides the user with the choices to view either single player or multiplayer leaderboard
*/
void leaderbd(int sockfd) {
    system("clear");

    char buffer[1024];
    bzero(buffer, 1024);

    printf("\n\tLeaderboard");
    printf("\n\n\t\t1.Single Player Leaderboard");
    printf("\n\n\t\t2.Multi Player Leaderboard");
    printf("\n\n\t\t3.Back");
    printf("\n\n\tPress the corresponding number to continue......");

    char choice = getch();

    switch(choice) {
        case '1': strcpy(buffer, "sLeader");
                  send(sockfd, buffer, 1024, 0);
                  retrieveSLB(sockfd); break;
        case '2': strcpy(buffer, "mLeader");
                  send(sockfd, buffer, 1024, 0);
                  retrieveMLB(sockfd); break;
        case '3': strcpy(buffer, "back");
                  send(sockfd, buffer, 1024, 0);
                  openGame(sockfd); break;
        	   
        default: leaderbd(sockfd);

    }

}

/*
Retrieve Single Player Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It retrieves the single player leaderboard from the server and displays it
*/
void retrieveSLB(int sockfd) {
    system("clear");
    printf("\n\n\t\tSingle Player Leaderboard\n");

    char line[1024];
    int count = 0;

    while(1>0) {
        char sc[1024];
        char id[1024];

        bzero(id, 1024);
        recv(sockfd, id, sizeof(line), 0);

        if(strncmp("0000", id, 4) == 0) {
            break;
        }

        bzero(line, 1024);
        recv(sockfd, line, sizeof(line), 0);

        bzero(sc, 1024);
        recv(sockfd, sc, sizeof(line), 0);

        count++;
        printf("\n\t\t%s\t%s\t%s\n",id,line,sc);
    }

    if(count == 0) {
        printf("\n\t\tUnable to open the Leaderboard");
    }

    printf("\n\n\tPress any Key to continue......");
    getch();
    
    openGame(sockfd);

}

/*
Retrieve Multi Player Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Server
Description: It retrieves the multi player leaderboard from the server and displays it
*/
void retrieveMLB(int sockfd) {
    system("clear");
    printf("\n\n\tMulti-Player Leaderboard\n");

    char line[1024];
    int count = 0;

    while(1>0) {
        bzero(line, 1024);
        recv(sockfd, line, sizeof(line), 0);

        if(strncmp("0000", line, 4) == 0) {
            break;
        }

        count++;
        printf("\n\t\t%s\n",line);
    }

    if(count == 0) {
        printf("\n\t\tUnable to open the Leaderboard");
    }

    printf("\n\nPress any Key to continue......");
    getch();
    
    openGame(sockfd);
    
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
