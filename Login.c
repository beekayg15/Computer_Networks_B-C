#include<stdio.h>
#include<stdlib.h>
#include<string.h>


struct reg_details{
	int user_id;
	char name[100];
	char password[100];
};

int unique_user(char user_name[])
{

	FILE* file = fopen("Registered_users.bin","rb");
	if(!file)
	{
		printf("\n Unable to open : Registered_users.bin");
	}
	struct reg_details u;
	int i=0;
	while(fread(&u, sizeof(u), 1, file))
	{
		//printf("\n %d %d %d\n",u.user_id,user_id,i++);
		if(strcmp(u.name,user_name)==0)
		{
			return 0;
		}
	}

	fclose(file);
	return 1;
}
int get_name_by_id(char user_name[])
{

	FILE* file = fopen("Registered_users.bin","rb");
	if(!file)
	{
		printf("\n Unable to open : Registered_users.bin");
	}
	struct reg_details u;
	int i=0;
	while(fread(&u, sizeof(u), 1, file))
	{
		//printf("\n %d %d %d\n",u.user_id,user_id,i++);
		if(strcmp(u.name,user_name)==0)
		{
			return u.user_id;
		}
	}

	fclose(file);
	return -1;
}
void register_user(){
	printf("\n\t\t\t\t Registration");
	struct reg_details reg,temp;

	int i;
        printf("\nEnter user name : ");

        for(i=0;(reg.name[i]=getchar())!='\n';i++);
        reg.name[i] = '\0';

        if(unique_user(reg.name)==0)
        {
        	printf("\nUser name already taken\n");
        	getchar();
        	system("clear");
        	register_user();
        	return;
        }

        printf("\nEnter password : ");

        for(i=0;(reg.password[i]=getchar())!='\n';i++);
	reg.password[i] = '\0';

	FILE* fp = fopen("Registered_users.bin","rb");
	if(fp!=NULL)
	{
		//fclose(fp);
		fp = fopen("Registered_users.bin","ab+");
		fseek(fp,-1*sizeof(reg),SEEK_END);
		fread(&temp,sizeof(temp),1,fp);
	}


	else
	{
		//fclose(fp);
		fp = fopen("Registered_users.bin","wb");
		temp.user_id = 0;
		for(i=0; i<7;i++)
		{
			temp.user_id = temp.user_id*10 + rand()%10;
		}
	}


	reg.user_id = temp.user_id+1;
	printf("\nRegistration succesfull, your user_id = %d\n",reg.user_id);
	getchar();

	fwrite(&reg,sizeof(reg),1,fp);

        fclose(fp);
}

void login()
{

	system("clear");
	printf("\n\t\t\t\tLogin");

	struct reg_details user1;
	char user_name[100];
	char password[100];
	int i;
	printf("\nEnter user_name : ");
	for(i=0;(user_name[i]=getchar())!='\n';i++);
	user_name[i]='\0';
	int user_id = get_name_by_id(user_name);
	printf("\nEnter password : ");

	for(i=0;(password[i]=getchar())!='\n';i++);
	password[i]='\0';

	FILE* fp = fopen("Registered_users.bin","rb");
	if(fp==NULL)
	{

		printf("\nOops!! You have not registered, please register to continue\n");
		getchar();
		system("clear");
		register_user();
		login();
		return;
	}

	while(fread(&user1, sizeof(user1), 1, fp))
	{

		if(user1.user_id == user_id)
		{

			if(strcmp(user1.password,password)==0)
			{
				printf("\nLogin successful!! ");
				//Function to redirect user1 to home page
				fclose(fp);
				return;

			}
			else
			{
				printf("\nIncorrect password, please try again\n");
				getchar();
				system("clear");
				login();
				return;
			}
		}

	}
	printf("Oops!! the user id you entered is not found! Please try again[l] or proceed to registration[r]\n");
	char c = getchar();
	getchar();
	if(c=='r')
	{

		system("clear");
		register_user();
		login();

	}
	else if(c=='l')
	{

		login();
	}
	fclose(fp);
}
void print_existing_users(){
	FILE* fp = fopen("Registered_users.bin","rb");
	if(fp==NULL)return;
	struct reg_details reg;
	while(fread(&reg,sizeof(reg),1,fp)){
		printf("\nUser id : %d\nName : %s\nPassword : %s\n",reg.user_id,reg.name,reg.password);
	}

}
int main()
{
	char c;
	printf("\nPress any key to continue \nIf you are a new user, press 'r' to register\n");
	c = getchar();

	if(c=='r')
	{
		getchar(); // To capture '\n' when we hit the 'return' key
		system("clear");
		register_user();

	}
	login();
	//print_existing_users();
}
