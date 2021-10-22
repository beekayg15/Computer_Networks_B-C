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

struct user{
	int userid;
	char name[100];
	int score;
} splayer;

struct reg_details{
	int userid;
	char name[100];
	char password[100];
} player1, player2;

char name[100];

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

    player1.userid = 1;
    player2.userid = 2;
    strcpy(player1.name,"Aravind");
    strcpy(player2.name,"Hrithik");

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

    bzero(buffer, 1024);

    recv(sockfd, buffer, 1024, 0);
        
    if(strncmp("endgame", buffer, 7) == 0) {
        printf("The client has ended the game!!\n");
        close(sockfd);
        exit(0);
    }

    if(buffer[0] == 'l') {
        printf("The Player wants to view the Learderboard!!\n");
        
        bzero(buffer, 1024);
        recv(sockfd, buffer, 1024, 0);

        if(strncmp("sLeader", buffer, 7) == 0) {
            printf("The player chose to view Single Player Leaderboard\n");
            transferSLB(sockfd);
        } else if(strncmp("mLeader", buffer, 7) == 0) {
            printf("The player chose to view Multiplayer Player Leaderboard\n");
            transferMLB(sockfd);
        } else {
            printf("The player returned back to Home Screen\n");
            initiateGame(sockfd);
        }

        initiateMultiGame(sockfd);
        return;
    }

    if(buffer[0] == 'm') {
        printf("The Player has entered Multiplayer Mode!!\n");
        initiateMultiGame(sockfd);
        return;
    }

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
        
        printf("Turn Number : %d, %s",number_of_turns, buffer);

        number_of_turns++;

        int bulls = numberOfBulls(buffer, number);
        int cows = numberOfCows(buffer, number);
        
        printf("Number of Bulls: %d\n", bulls);
        printf("Number of Cows: %d\n", cows);

        if(bulls == 4) {
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

    playMultiGame(sockfd, client_socket);

    return;
}

void playMultiGame(int sockfd1, int sockfd2) {
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
		printf("\nSuccessfully replaced  line with .");

	} else {
		FILE* file = fopen( "leaderboard.bin", "ab+"); 
		if(!file) {
			printf("\n Unable to open : leaderboard.bin");

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

void printsingleplayerleaderboard(int sockfd) {
    printf("\nSP Leaderboard\n");
    
	struct user u;

    char line[1024];
    bzero(line,1024);

	FILE * fptr;
	fptr  = fopen("leaderboard.bin", "r+b");
	if (fptr == NULL) {
		printf("Unable to open : leaderboard.bin");
        strcpy(line, "1111");
        send(sockfd, line, 1024, 0);
        return;
	}

	int uid[100];
	char names[100][100];
	int scores[100];
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
	for(int i=0;i<count;i++) {
		//printf("%d -> %s -> %d\n",arr[i].userid,arr[i].name,arr[i].score);

        char *id;
        char *sc;
        char *result;
        sprintf(id,"%d", arr[i].userid);
        sprintf(sc,"%d", arr[i].score);

        strcpy(result, id);
        strcat(result, " -> ");
        strcat(result, arr[i].name);
        strcat(result, " -> ");
        strcat(result, sc);
        strcat(result, "\0");

        strcpy(line, result);

        send(sockfd, line, 1024, 0);
        bzero(line,1024);

	}
    strcpy(line, "1111");
    send(sockfd, line, 1024, 0);

    fclose(fptr);
}

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

void printmultiplayerleaderboard(int sockfd) {
    printf("\nMP Leaderboard\n");

	char line[1024];
    bzero(line,1024);

	char fileName[100] = "multiplayer-leaderboard.txt";
	FILE* file = fopen(fileName, "r"); 
	if(!file) {
		printf("\n Unable to open : %s ", fileName);
        strcpy(line, "1111");
        send(sockfd, line, 1024, 0);
        return;
	}

	while (fgets(line, sizeof(line), file)) {
        send(sockfd, line, 1024, 0);
		//printf("\n%s\n",line);
        bzero(line,1024);
	}
    strcpy(line, "1111");
    send(sockfd, line, 1024, 0);

	fclose(file);

}

void transferSLB(int sockfd) {
    printsingleplayerleaderboard(sockfd);
    initiateGame(sockfd);

}

void transferMLB(int sockfd) {
    printmultiplayerleaderboard(sockfd);
    initiateGame(sockfd);

}
