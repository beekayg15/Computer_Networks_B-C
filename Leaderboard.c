#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
struct user{
	int userid;
	char name[100];
	int score;
};
struct reg_details{
	int userid;
	char name[100];
	char password[100];
};
char name[100] ;

void getnamebyuserid(int user_id)
{
	printf("\nFunction Called");
	FILE* file = fopen("Registered_users.bin","rb"); 
	if(!file)
	{
		printf("\n Unable to open : Registered_users.bin");
	}
	struct reg_details u;
	int i=0;
	while(fread(&u, sizeof(u), 1, file))
	{
		printf("\n %d %d %d\n",u.userid,user_id,i++);
		if(u.userid == user_id)
		{
			printf("FName: %s",name);
			strcpy(name,u.name);
			break;
		}
	}
	fclose(file);
}
int checkuseralreadyexist(int user_id)
{
	char fileName[100] ;
	FILE* file = fopen("leaderboard.bin","rb"); 
	if(!file)
	{
		printf("\n Unable to open : leaderboard.bin");
		return -1;
	}
	struct user u;
	while(fread(&u, sizeof(u), 1, file))
	{
		if(u.userid == user_id)
		{
			return 1;
		}
	}
	return 0;
	fclose(file);
}
void updateleaderboard(int user_id,int score)
{
	int flag = checkuseralreadyexist(user_id);
	if(flag != 0)
	{
		struct user u;
		FILE * fPtr;
		fPtr  = fopen("leaderboard.bin", "r+b");
		if (fPtr == NULL)
		{
			printf("Unable to open file.\n");
		}
		
		while (fread(&u, sizeof(u), 1, fPtr))
		{
			printf("lb: %d,user: %d",u.userid,user_id);
			if(u.userid==user_id && u.score>score)
			{
				u.score = score;
				fseek(fPtr,-1*sizeof(u),SEEK_CUR);
				fwrite(&u, sizeof(u), 1, fPtr);
			}
			    
		}
		fclose(fPtr);
		printf("\nSuccessfully replaced  line with .");

	}
	else
	{
		FILE* file = fopen( "leaderboard.bin", "ab+"); 
		if(!file)
		{
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
void sorting()
{
	FILE * fptr;
	fptr  = fopen("leaderboard.bin", "r+b");
	if (fptr == NULL)
	{
		printf("Unable to open");
	}
	int uid[100];
	char names[100][100];
	int scores[100];
	struct user u;
	int count=0;
	struct user arr[100];
	while(fread(&u,sizeof(struct user),1,fptr))
	{
		struct user temp;
		temp.userid=u.userid;
		strcpy(temp.name,u.name);
		temp.score=u.score;
		arr[count++]=temp;
	}
	for(int i=0;i<count-1;i++)
	{
		for(int j=0;j<count-i-1;j++)
		{
			if (arr[j].score > arr[j+1].score)
			{
				struct user t;
				t=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=t;
			}
		}
	}
	for(int i=0;i<count;i++)
	{
		printf("%d -> %s -> %d\n",arr[i].userid,arr[i].name,arr[i].score);
	}
}
void printlb()
{
	printf("\nLeaderboard\n");
	FILE* file = fopen("leaderboard.bin", "rb"); 
	if(!file)
	{
		printf("\n Unable to open : leaderboard.bin" );
	}
	struct user u;
	int i=0;
	while(fread(&u, sizeof(u), 1, file))
	{

		printf("%d %s %d\n",u.userid,u.name,u.score);
	}
	fclose(file);

}
void updatemultiplayerleaderboard(int user1,int user2,int user1score,int user2score)
{
	printf("updatemu");
	char fileName[100] = "multiplayer-leaderboard.txt";
	FILE* file = fopen(fileName, "a"); 
	if(!file)
	{
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
	
	if(user1score==user2score){
		fprintf(file,"Match drawn between %s and %s in %d turns\n",winner_name,loser_name,winner_score);
	}
	else{
		fprintf(file,"%s(%d) won against %s \n",winner_name,winner_score,loser_name);
	}
	fclose(file);


}
void printmultiplayerleaderboard()
{
	char fileName[100] = "multiplayer-leaderboard.txt";
	FILE* file = fopen(fileName, "r"); 
	if(!file)
	{
		printf("\n Unable to open : %s ", fileName);
	}
	char line[500];
	while (fgets(line, sizeof(line), file)) 
	{
		printf("\n%s\n",line);
	}
	fclose(file);

}
int main()
{
	FILE* fPtr  = fopen("leaderboard.bin", "ab");
	updateleaderboard(2,1);
	updatemultiplayerleaderboard(1,2,5,5);
	printlb();
	printmultiplayerleaderboard();

}
