#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define PORT 1515
#define PORT2 3232
#define MAX 1000000007

void initiateGame(int sockfd);
void initiateMultiGame(int sockfd);
void playMultiGame(int sockfd1, int sockfd2);
int numberOfBulls(char guess[], char num[]);
int numberOfCows(char guess[], char num[]);
int valid(char guess[]);
void getnamebyuserid(int user_id);
int checkuseralreadyexist(int user_id);
void updateleaderboard(int user_id,int score);
void printsingleplayerleaderboard(int sockfd);
void updatemultiplayerleaderboard(int user1,int user2,int user1score,int user2score);
void printmultiplayerleaderboard(int sockfd);
void transferSLB(int sockfd);
void transferMLB(int sockfd);
void login(int sockfd, int player);
void register_user(int sockfd, int player);
void loginOrRegister(int sockfd, int player);
int get_id_by_name(char user_name[]);
int unique_user(char user_name[]);

// Structure to store the Game details of the user
struct user{
	int userid;
	char name[100];
	int score;
} splayer;

// Structure to store the Registartion details of the user
struct reg_details{
	int userid;
	char name[100];
	char password[100];
} player1, player2;

char name[100];     //Stores the name of the registered user
int temp_sockfd;    //Stores the socket descriptor of the first client

/*
Main Function
Parameters: None
Description: Driver Function to Establish Socket Connection with the Clients
*/
int main() {
    FILE* fptr  = fopen("leaderboard.bin", "ab");
    fclose(fptr);

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

    loginOrRegister(client_socket, 1);

    return 0;

}

/*
Login Screen Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
- int player : Stores if it is the first client or the second
Description: The Function retrieves from the Login Page from which the User can choose to Login or Sign Up
*/
void loginOrRegister(int sockfd, int player) {
    printf("Asking Player %d to Register or Login...\n", player);

    char buffer[1024];
    bzero(buffer, 1024);

    recv(sockfd, buffer, 1024, 0);

    if(buffer[0] == '1') {  // If user has chose to Register
        register_user(sockfd, player);
    } 
    
    // Else login
    login(sockfd, player);

}

/*
Initiate Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: It first takes in the user's choice either to view leaderboard, multiplayer or single player
If the user chooses single player the function continues to provide the requested service 
*/
void initiateGame(int sockfd) {
    int number_of_turns = 0;
    char buffer[1024];

    bzero(buffer, 1024);

    recv(sockfd, buffer, 1024, 0);
        
    if(strncmp("endgame", buffer, 7) == 0) { //If user has chosen to exit the game
        printf("The client has ended the game!!\n");
        close(sockfd);
        exit(0);
    }

    if(buffer[0] == 'l') { //If user has chosen to view the leaderboard
        printf("The Player wants to view the Learderboard!!\n");
        
        bzero(buffer, 1024);
        recv(sockfd, buffer, 1024, 0);

        if(strncmp("sLeader", buffer, 7) == 0) { //If user has chosen to view the singleplayer leaderboard
            printf("The player chose to view Single Player Leaderboard\n");
            transferSLB(sockfd);
        } else if(strncmp("mLeader", buffer, 7) == 0) { //If user has chosen to view the multiplayer leaderboard
            printf("The player chose to view Multiplayer Player Leaderboard\n");
            transferMLB(sockfd);
        } else { //If user has chosen to move back
            printf("The player returned back to Home Screen\n");
            initiateGame(sockfd);
        }

        initiateMultiGame(sockfd);
        return;
    }

    if(buffer[0] == 'm') { //If user has chosen to play in Multiplayer Mode
        printf("The Player has entered Multiplayer Mode!!\n");
        initiateMultiGame(sockfd);
        return;
    }

    if(buffer[0] == 'r') {
        printf("The Player has Logged Out!!\n");
        loginOrRegister(sockfd, 1);
        return;
    }

    // Else if the user wants to play in Single Player Mode
    char number[4] = "    ";
    int count = 0;

    while(count < 4) { //Generate a 4-digit number with distinct digits
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

        // Recieves the User's guess
        recv(sockfd, buffer, 1024, 0);

        if(strncmp("exit", buffer, 4) == 0) { //If the user has chosen to quit the current game
            printf("The client has left the current game!!\n");
            initiateGame(sockfd);
        }
        
        printf("Turn Number : %d, %s",number_of_turns, buffer);

        number_of_turns++;

        //Calculate the number of bulls and cows for the guess
        int bulls = numberOfBulls(buffer, number);
        int cows = numberOfCows(buffer, number);
        
        printf("Number of Bulls: %d\n", bulls);
        printf("Number of Cows: %d\n", cows);

        if(bulls == 4) { // Player has found the secret number
            bzero(buffer, 1024);

            updateleaderboard(player1.userid, number_of_turns);

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

            initiateGame(sockfd); // Start new Game

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

/*
Initiate Multiplayer Game Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: Drives the Multiplayer game and establishes connection with the Second Client
*/
void initiateMultiGame(int sockfd) {
    printf("In Multiplayer Mode!!\n");

    char buffer[1024];
    bzero(buffer, 1024);
    strcpy(buffer, "f");

    socklen_t addr_len;
    struct sockaddr_in server_address;
    int server_socket;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);

    if(server_socket < 0) {
        printf("Error While Creating Second Socket!!\n");
        send(sockfd, buffer, 1024, 0);
        initiateGame(sockfd);

    } else {
        printf("Second Socket Creation Successful!!\n");

    }

    addr_len = sizeof(server_address);
    bzero(&server_address, addr_len);

    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT2);

    if(bind(server_socket, (struct sockaddr*)&server_address, addr_len) != 0) {
        printf("Second Binding Failed!!\n");
        send(sockfd, buffer, 1024, 0);
        initiateGame(sockfd);

    } else {
        printf("Second Binding Successful!!\n");

    }

    if(listen(server_socket, 2) != 0) {
        printf("Unable to Listen to Multiplayer Clients!!\n");
        send(sockfd, buffer, 1024, 0);
        initiateGame(sockfd);

    } else {
        printf("Waiting for Multiplayer Clients!!\n");

    }

    int client_socket;
    struct sockaddr_in client_address;

    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &addr_len);

    if(client_socket < 0) {
        printf("Error in Establishing Connection with the Multiplayer Client!!\n");
        send(sockfd, buffer, 1024, 0);
        initiateGame(sockfd);

    } else {
        printf("Connection with Client Established Multiplayer Successfully!!\n");
        buffer[0] = 's';
        send(sockfd, buffer, 1024, 0);

    }

    temp_sockfd = sockfd;

    loginOrRegister(client_socket, 2); // To Login/Register the Second Player

    return;
}

/*
Play Multiplayer Game Function
Parameters:
- int sockfd1 : Stores the Socket Descriptor to Communicate with the first Client
- int sockfd1 : Stores the Socket Descriptor to Communicate with the Second Cleint
Description: Drives the Multiplayer game and establishes connection with the Second Client
*/
void playMultiGame(int sockfd1, int sockfd2) {
    if(sockfd1 == 0) {
        sockfd1 = temp_sockfd;
    }

    int number_of_turns = 0;
    char buffer1[1024];
    char buffer2[1024];
    char code1[5];
    char code2[5];
    bzero(code1, 5);
    bzero(code2, 5);

    recv(sockfd1, code1, 5, 0);
    recv(sockfd2, code2, 5, 0);

    printf("Secret Code of Player 1 : %s\n", code1);
    printf("Secret Code of Player 2 : %s\n", code2);

    bzero(buffer1, 1024);
    bzero(buffer2, 1024);

    strcpy(buffer1, "continue");
    strcpy(buffer2, "continue");

    send(sockfd1, buffer1, 1024, 0);
    send(sockfd2, buffer2, 1024, 0);

    printf("\nPlayers are ready to mave their guesses!!!\n\n");

    while(1>0) {
        number_of_turns++;
        int Found = 0;

        //Player 1
        bzero(buffer1, 1024);

        recv(sockfd1, buffer1, 1024, 0);

        printf("\nPlayer 1 in Turn Number : %d, %s\n",number_of_turns, buffer1);

        int bulls1 = numberOfBulls(buffer1, code2);
        int cows1 = numberOfCows(buffer1, code2);
        if(bulls1 == 4) {
            Found = 1;
            printf("\n\nPlayer 1 has Found the Code\n");
        }
        
        printf("Number of Bulls: %d\n", bulls1);
        printf("Number of Cows: %d\n", cows1);

        bzero(buffer1, 1024);

        char temp[] = "Player 1 got Bulls: _; Cows: _; In turn: _\n\0";

        for(int z=0; temp[z]!='\0'; z++) {
            buffer1[z] = temp[z];

        }

        buffer1[20] = (char)(bulls1 + 48);
        buffer1[29] = (char)(cows1 + 48);

        char s[4]; 
        sprintf(s,"%d", number_of_turns);
        buffer1[41] = s[0];

        for(int z=1; z<5 && s[z-1]!='\0'; z++) {
            if(s[z] == '\0') {
                buffer1[z+41] = '\n';

            } else {
                buffer1[z+41] = s[z];

            }

        }

        send(sockfd1, buffer1, 1024, 0);
        send(sockfd2, buffer1, 1024, 0);

        //Player 2
        bzero(buffer2, 1024);

        recv(sockfd2, buffer2, 1024, 0);

        printf("\nPlayer 2 in Turn Number : %d, %s\n",number_of_turns, buffer2);

        int bulls2 = numberOfBulls(buffer2, code1);
        int cows2 = numberOfCows(buffer2, code1);
        
        printf("Number of Bulls: %d\n", bulls2);
        printf("Number of Cows: %d\n", cows2);

        bzero(buffer2, 1024);

        strcpy(temp, "Player 2 got Bulls: _; Cows: _; In turn: _\n\0");

        for(int z=0; temp[z]!='\0'; z++) {
            buffer2[z] = temp[z];

        }

        buffer2[20] = (char)(bulls2 + 48);
        buffer2[29] = (char)(cows2 + 48);

        sprintf(s,"%d", number_of_turns);
        buffer2[41] = s[0];

        for(int z=1; z<5 && s[z-1]!='\0'; z++) {
            if(s[z] == '\0') {
                buffer2[z+41] = '\n';

            } else {
                buffer2[z+41] = s[z];

            }

        }

        send(sockfd1, buffer2, 1024, 0);
        send(sockfd2, buffer2, 1024, 0);

        if(Found == 1 && bulls2 == 4) {
            printf("\nThe game has ended in a tie!!\n");
            updatemultiplayerleaderboard(player1.userid, player2.userid,number_of_turns,number_of_turns);
            close(sockfd2);
            initiateGame(sockfd1);

        }

        if(Found == 1 && bulls2 != 4) {
            printf("\nThe game has been won by player 1!!\n");
            updatemultiplayerleaderboard(player1.userid, player2.userid,number_of_turns,MAX);
            close(sockfd2);
            initiateGame(sockfd1);

        }

        if(Found == 0 && bulls2 == 4) {
            printf("\nThe game has been won by player 2!!\n");
            updatemultiplayerleaderboard(player1.userid, player2.userid,MAX,number_of_turns);
            close(sockfd2);
            initiateGame(sockfd1);

        }


    }

    exit(0);
}

/*
Number of Bulls Function
Parameters:
- char[] guess : Stores the 4-digit number guessed by the user
- char[] num : Stores the actual 4-digit number generated by the Server
Description: Calculates the number of bulls comparing the entered guess and the actual code
*/
int numberOfBulls(char guess[], char num[]) {
    int bulls = 0;

    for(int index=0; index<4; index++) {
        if(guess[index] == num[index]) {
            bulls++;

        }

    }

    return bulls;

}

/*
Number of Cows Function
Parameters:
- char[] guess : Stores the 4-digit number guessed by the user
- char[] num : Stores the actual 4-digit number generated by the Server
Description: Calculates the number of cows comparing the entered guess and the actual code
*/
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

/*
Get Name by User-ID Function
Parameters:
- int user_id : Stores the user_id of the player
Description: It reads the 'Registered_users.bin' file and finds the corresponding Name from the User-ID
*/
void getnamebyuserid(int user_id) {
	printf("\nFunction Called");
	
    FILE* file = fopen("Registered_users.bin","rb"); 
	
    if(!file) {
		printf("\n Unable to open : Registered_users.bin");
	}
	
    struct reg_details u;
	int i=0;
	
    while(fread(&u, sizeof(u), 1, file)) {
		printf("\n %d %d %d\n",u.userid,user_id,i++);
		if(u.userid == user_id) {
			printf("FName: %s",name);
			strcpy(name,u.name);
			break;

		}

	}
	fclose(file);

}

/*
Check User Already Exists Function
Parameters:
- int user_id : Stores the User_id of the player
Description: Checks if the User's score is already present in the 'leaderboard.bin' file
*/
int checkuseralreadyexist(int user_id) {
	char fileName[100] ;
	
    FILE* file = fopen("leaderboard.bin","rb"); 
	if(!file) {
		printf("\n Unable to open : leaderboard.bin");
		return -1;
	}
	
    struct user u;

	while(fread(&u, sizeof(u), 1, file)) {
		if(u.userid == user_id) {
			return 1;

		}

	}
	return 0;
	fclose(file);

}

/*
Update Leaderboard Function
Parameters:
- int user_id : Stores the User_id of the player
- int score : Stores the number of turns taken by the player to find the secret code
Description: Updates the new Score taken by the user in the current Game
*/
void updateleaderboard(int user_id,int score) {
	int flag = checkuseralreadyexist(user_id);

	if(flag != 0) {
		struct user u;
		
        FILE * fPtr;
		fPtr  = fopen("leaderboard.bin", "r+b");
		if (fPtr == NULL) {
			printf("Unable to open file.\n");
		}
		
		while (fread(&u, sizeof(u), 1, fPtr)) {
			printf("lb: %d,user: %d",u.userid,user_id);

			if(u.userid==user_id && u.score>score) {
				u.score = score;
				fseek(fPtr,-1*sizeof(u),SEEK_CUR);
				fwrite(&u, sizeof(u), 1, fPtr);

			}
			    
		}
		fclose(fPtr);
		printf("\nSuccessfully replaced line\n");

	} else {
		FILE* file = fopen( "leaderboard.bin", "ab+"); 
		if(!file) {
			printf("\n Unable to open : leaderboard.bin\n");

		}

		struct user newuser;		
		newuser.userid=user_id;
		
        getnamebyuserid(user_id);
		strcpy(newuser.name,name);
		newuser.score=score;
		
        fseek(file,0,SEEK_END);
		fwrite(&newuser, sizeof(newuser), 1, file);
		fclose(file);
		printf("New user added");

	}

}

/*
Print Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: Prints the Top Scores taken by the users and Sends them to the Client
*/
void printsingleplayerleaderboard(int sockfd) {
    printf("\nSP Leaderboard\n");
    
	struct user u;

    char line[1024];
    bzero(line,1024);

	FILE * fptr;
	fptr  = fopen("leaderboard.bin", "r+b");
	if (fptr == NULL) {
		printf("Unable to open : leaderboard.bin");
        strcpy(line, "0000");
        send(sockfd, line, 1024, 0);
        return;
	}

	int count=0;
	struct user arr[100];

	while(fread(&u,sizeof(struct user),1,fptr)) {
		struct user temp;
		temp.userid=u.userid;
		strcpy(temp.name,u.name);
		temp.score=u.score;
		arr[count++]=temp;
	}

	for(int i=0;i<count-1;i++) {
		for(int j=0;j<count-i-1;j++) {
			if (arr[j].score > arr[j+1].score) {
				struct user t;
				t=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=t;
			}
		}
	}

    printf("\n%d Data's Retrieved\n", count);

	for(int i=0;i<count;i++) {
		printf("%d -> %s -> %d\n",arr[i].userid,arr[i].name,arr[i].score);

        char sc[1024];
        char id[1024];

        bzero(id,1024);
        bzero(line,1024);
        bzero(sc,1024);

        strcpy(line, arr[i].name);
        sprintf(sc, "%d", arr[i].score);
        sprintf(id, "%d", arr[i].userid);
        
        
        send(sockfd, id, 1024, 0);
        send(sockfd, line, 1024, 0);
        send(sockfd, sc, 1024, 0);

	}
    bzero(line, 1024);
    strcpy(line, "0000");
    send(sockfd, line, 1024, 0);

    fclose(fptr);
}

/*
Update Multiplayer Leaderboard Function
Parameters:
- int user1 : Stores the User_id of the player 1
- int user1scorev : Stores the number of turns taken by the player 1 to find the secret code
- int user2 : Stores the User_id of the player 2
- int user2score : Stores the number of turns taken by the player 2 to find the secret code
Description: Updates the new Score taken by the users in the current Game
*/
void updatemultiplayerleaderboard(int user1,int user2,int user1score,int user2score) {
	printf("updatemu");
	
    char fileName[100] = "multiplayer-leaderboard.txt";
	FILE* file = fopen(fileName, "a"); 
	if(!file) {
		printf("\n Unable to open : %s ", fileName);
	}
	
	char line[100];

	int winner_id = user1score<user2score?user1:user2;
	int loser_id = user1score<user2score?user2:user1;
	int winner_score = user1score<user2score?user1score:user2score;

	char winner_name[100],loser_name[100];
	getnamebyuserid(winner_id);
	strcpy(winner_name,name);
	getnamebyuserid(loser_id);
	strcpy(loser_name,name);
	
	if(user1score==user2score) {
		fprintf(file,"Match drawn between %s and %s in %d turns\n",winner_name,loser_name,winner_score);
	} else {
		fprintf(file,"%s(%d) won against %s \n",winner_name,winner_score,loser_name);
	}
	fclose(file);

}

/*
Print Multiplayer Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: Prints the Scores taken in various Multiplayer mode games and Sends them to the Client
*/
void printmultiplayerleaderboard(int sockfd) {
    printf("\nMP Leaderboard\n");

	char line[1024];
    bzero(line,1024);

	char fileName[100] = "multiplayer-leaderboard.txt";
	FILE* file = fopen(fileName, "r"); 
	if(!file) {
		printf("\n Unable to open : %s ", fileName);
        strcpy(line, "0000");
        send(sockfd, line, 1024, 0);
        return;
	}

	while (fgets(line, sizeof(line), file)) {
        send(sockfd, line, 1024, 0);
		//printf("\n%s\n",line);
        bzero(line,1024);
	}
    strcpy(line, "0000");
    send(sockfd, line, 1024, 0);

	fclose(file);

}

/*
Print Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: Prints the Top Scores taken by the users and Sends them to the Client
*/
void transferSLB(int sockfd) {
    printsingleplayerleaderboard(sockfd);
    initiateGame(sockfd);

}

/*
Print Multiplayer Leaderboard Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
Description: Prints the Scores taken in various Multiplayer mode games and Sends them to the Client
*/
void transferMLB(int sockfd) {
    printmultiplayerleaderboard(sockfd);
    initiateGame(sockfd);

}

/*
Login Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
- int player : Stores if it is the first client or the second
Description: To Login into the Account of an already Registered User
*/
void login(int sockfd, int player) {

    char buffer[1024];
    bzero(buffer, 1024);

    char pname[100], ppassword[100];
    bzero(pname, 100);
    bzero(ppassword, 100);

    recv(sockfd, pname, 100, 0);
    recv(sockfd, ppassword, 100, 0);

	struct reg_details user1;
	int flag = 0;

	FILE* fp = fopen("Registered_users.bin","rb");
	if(fp == NULL) {
		printf("\n Unable to open : Registered_users.bin");

	} else {

        int user_id = get_id_by_name(pname);

        while(fread(&user1, sizeof(user1), 1, fp)) {
            if(user1.userid == user_id) {
                if(strcmp(user1.password,ppassword)==0) {

                    strcpy(buffer, "1");
                    send(sockfd, buffer, 1024, 0);
                    printf("\nLogin Authentication Successful!!\n");
                    fclose(fp);

                    flag = 1;

                    if(player == 1) {
                        player1.userid = user1.userid;
                        strcpy(player1.name, pname);
                        strcpy(player1.password, ppassword);

                        initiateGame(sockfd);

                    } else {
                        player2.userid = user1.userid;
                        strcpy(player2.name, pname);
                        strcpy(player2.password, ppassword);

                        playMultiGame(0, sockfd);
                    }
                    return;

                } else {
                    printf("\nIncorrect Password\n");
                    fclose(fp);
                    break;
                }
            }
        }
    }

    strcpy(buffer, "2");
    send(sockfd, buffer, 1024, 0);

    recv(sockfd, buffer, 1024, 0);

	if(buffer[0] == '1') {
		register_user(sockfd, player);

	}
	login(sockfd,player);

}

/*
Register Function
Parameters:
- int sockfd : Stores the Socket Descriptor to Communicate with the Client
- int player : Stores if it is the first client or the second
Description: To Register an User and create a Profile for the User
*/
void register_user(int sockfd, int player) {
    printf("\nRegistration\n");
	struct reg_details reg,temp;

    char buffer[1024];
    bzero(buffer, 1024);

    char pname[100],ppassword[100];
    bzero(pname, 100);
    bzero(ppassword, 100);

    recv(sockfd, pname, 100, 0);

    strcpy(reg.name,pname);

    if(unique_user(reg.name)==0) {
        printf("\nUser name already taken\n");

        strcpy(buffer, "Invalid");
        send(sockfd, buffer, 1024, 0);
        
        register_user(sockfd, player);
        return;
    }

    strcpy(buffer, "Valid");
    send(sockfd, buffer, 1024, 0);

    recv(sockfd, ppassword, 100, 0);

    strcpy(reg.password,ppassword);

	FILE* fp = fopen("Registered_users.bin","rb");
	if(fp!=NULL) {
		fp = fopen("Registered_users.bin","ab+");
		fseek(fp,-1*sizeof(reg),SEEK_END);
		fread(&temp,sizeof(temp),1,fp);

	} else {
		fp = fopen("Registered_users.bin","wb");
		temp.userid = 0;
		for(int i=0; i<7;i++) {
			temp.userid = temp.userid*10 + rand()%10;

		}
	}


	reg.userid = temp.userid+1;
	printf("\nRegistration succesfull, player's user_id = %d\n",reg.userid);

    bzero(buffer, 1024);
    sprintf(buffer, "%d", reg.userid);

    send(sockfd, buffer, 1024, 0);

    printf("\nID SENT\n");

	fwrite(&reg,sizeof(reg),1,fp);
    fclose(fp);

    login(sockfd, player);

}

/*
Unique User Function
Parameters:
- char[] user_name : Stores the Name of the User to check if the requested in avaiable
Description: To Register an User and create a Profile for the User
*/
int unique_user(char user_name[]) {

	FILE* file = fopen("Registered_users.bin","rb");
	if(!file) {
		printf("\n Unable to open : Registered_users.bin");
        return 1;
	}

	struct reg_details u;
	int i=0;

	while(fread(&u, sizeof(u), 1, file)) {
		if(strcmp(u.name,user_name)==0) {
			return 0;
		}
	}

	fclose(file);
	return 1;
}

/*
Get User-ID by Name Function
Parameters:
- char[] user_name : Stores the Name of the User to check if the requested in avaiable
Description: It reads the 'Registered_users.bin' file and finds the corresponding User-ID from the Name
*/
int get_id_by_name(char user_name[])
{

	FILE* file = fopen("Registered_users.bin","rb");
	if(!file)
	{
		printf("\n Unable to open : Registered_users.bin");
        exit(0);
	}
	struct reg_details u;
	int i=0;
	while(fread(&u, sizeof(u), 1, file))
	{
		//printf("\n %d %d %d\n",u.user_id,user_id,i++);
		if(strcmp(u.name,user_name)==0)
		{
			return u.userid;
		}
	}

	fclose(file);
	return -1;
}