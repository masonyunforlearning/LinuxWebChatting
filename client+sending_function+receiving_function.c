#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#define BUF_SIZE 10240
/**********�������� ����************/
char protocol[7] = "_##_";
char buf[BUF_SIZE] = "";
char ROOM[9] = "_##_ROOM";
    char MAKE[13] = "_##_MAKE_##_";
    char ENTER[13] = "_##_ENTE_##_";
    char EXIT[14] = "_##_EXIT_##_\n";
    char ROIN[14] = "_##_ROIN_##_\n";
    
char SEND[21] = "_##_CHAT_##_SEND_##_";
char DMSG[21] = "_##_CHAT_##_DMSG_##_";
char INFO[9] = "_##_INFO";
    char MIST[14] = "_##_MIST_##_\n";
    char RIST[14] = "_##_RIST_##_\n";
    char NAME[13] = "_##_NAME_##";
/*********************************/
int intheroom = 0; //���� ���� �濡 �ִ��� ������ Ȯ��
int roomnumber = 10; // �ִ� �� ����(������ �����ϴ� �� ������ ���� �ٲ� �� ����)
char myname[BUF_SIZE];
void error_handling(char* message);
int chatting(int sock_id);
int received(int sock_id);
void seecommand();

int main(int argc, char* argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    char* token;
    // char buf[BUF_SIZE] = "_##_INFO_##_NAME_##_";
    char buf[BUF_SIZE];
    int str_len;
    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else
    {
        printf("what is your name : ");
        fgets(myname, BUF_SIZE, stdin);
        token = strtok(myname, "\n");
        sprintf(buf, "%s%s%s%s\n", INFO, NAME, myname, protocol);
        write(sock, buf, strlen(buf));
        while (1) {
            int get = read(sock, message, BUF_SIZE);
            printf("%s\n", message);
            if (get != 0) break;
        }
    }

    pid_t pid = fork();
    while (1) {

        if (pid != 0)
        {
            //�޼��� ������ ������
            printf("if you want manual, type  /help or /?\n");
            if (chatting(sock) == 1) {
                kill(pid, SIGINT);
                break;
            }
            else
                error_handling("unknown error");
            
        }

        if (pid == 0)
			received(sock);

    }
    close(sock);

    return 0;
}
int chatting(int sock_id)
{
    char message[BUF_SIZE];
    char buf[BUF_SIZE] = "";
    char *token;
    while (1)
    {
         fgets(message, BUF_SIZE, stdin);
         
         if (message[0] == '/' )//��ɾ� ��Ʈ(dm����)
         {
             //��𼭵� ��밡��
             if (!strcmp(message + 1, "help\n") || !strcmp(message + 1, "?\n")) //��ɾ� ��� �� Ŭ���̾�Ʈ������
             {
                 seecommand();
             }
             else if (!strcmp(message + 1, "quit\n")) //Ŭ���̾�Ʈ ����
             {
                 return 1;
             }
             else if (!strcmp(message + 1, "myname\n")) //���̸� ǥ��
             {
                 printf("Your name : %s\n", myname);
             }
             else
             {
                 token = strtok(message, " ");
                 if (!strcmp(token, "/dm")) //DM
                 {
                     strcpy(buf, DMSG);
                     token = strtok(NULL, " ");
                     strcat(buf, token); //DM ���
                     if (token[0] == '\n')
                     {
                         printf("Does not fit the format of /dm\n");
                     }
                     else
		             {
                         token = strtok(NULL, "\n");
                         if (token == NULL || token[0] == '\n')
                         {
                             printf("Does not fit the format of /dm\n");
                         }
                         else
                         {
                             strcat(buf, protocol);
                             strcat(buf, token);//�޼���
                             strcat(buf, "_##_\n");
                             write(sock_id, buf, strlen(buf));
                         }
                     }
                 }
                 else if (!strcmp(token, "/exit\n"))
                 {
                     if (intheroom)
                     {
                         sprintf(buf, "%s%s", ROOM, EXIT);
                         write(sock_id, buf, strlen(buf));
						 intheroom--;
                     }
                     else
                         printf("you can use this command only in the room\n");
                     
                 }
                 else if (!strcmp(token, "/info\n"))
                 {
                     if (intheroom)
                     {
                         sprintf(buf, "%s%s", ROOM, ROIN);
                         write(sock_id, buf, strlen(buf));
                     }
                     else
                         printf("you can use this command only in the room\n");
                     
                 }
                 else if (!strcmp(token, "/mkroom"))
                 {
                     token = strtok(NULL, "\n");
                     if (intheroom)
                     {
                         printf("you can use this command only in the lobby\n");
                     }                         
                     else if(token == NULL || token[0] == '\n')
                     {
                         printf("Does not fit the format of /mkroom\n");
                     }
                     else
                     {
                         sprintf(buf, "%s%s%s%s\n", ROOM, MAKE,token,protocol);
                         write(sock_id, buf, strlen(buf));
                     }
                 }
                 else if (!strcmp(token, "/enter"))
                 {
                     if (intheroom)
		             {   
			            printf("you can use this command only in the lobby\n");
		             }
		             else
                     {
                         token = strtok(NULL, "\n");
						 if (atoi(token) > roomnumber)
						 {
							 printf("ROOMNUMBER is too big. Please try again.\n");
							 continue;
						 }
						 else
						 {
							 if (token == NULL || token[0] == '\n')
							 {
								 printf("Does not fit the format of /enter\n");
							 }
							 else
							 {
								 sprintf(buf, "%s%s%s%s\n", ROOM, ENTER, token, protocol);
								 write(sock_id, buf, strlen(buf));
								 intheroom++;
							 }
						 }

                     }
                     
                 }
                 else if (!strcmp(token, "/mlist\n"))
                 {
                     sprintf(buf, "%s%s", INFO, MIST);
                     write(sock_id, buf, strlen(buf));
                 }
                 else if (!strcmp(token, "/rlist\n"))
                 {
                     sprintf(buf, "%s%s", INFO, RIST);
                     write(sock_id, buf, strlen(buf));
                 }
                 else //�׿� �߸��� ��ɹ�
                     printf("Invalid command or Does not fit the format. Enter /help or /?\n\n");
             }
                 
         }
         else//ä����Ʈ
         {
             if (intheroom)//���� �濡 �����ִٸ�
             {
                 if (message[0] != '\n')
                 {
                     token = strtok(message, "\n");
                     sprintf(buf, "%s%s%s\n", SEND, token, protocol);
                     write(sock_id,buf,strlen(buf));
                 }
             }
             else //�ƴҰ��(�κ� �������)
             {
                 printf("You can only chat in the room.\n");
             }
             
         }
         
    }
   
    return 5;
}

int received(int sock_id)
{
	int str_len;
	char message[BUF_SIZE];
	char **msgArr;

	str_len = read(sock_id, message, BUF_SIZE - 1);

	message[str_len] = 0;
	printf("%s\n", message);
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void seecommand() {

    printf("/help /? : Shows command\n");
    printf("/quit : Close chatting program\n");
    printf("/mlist : Shows people on the server\n");
    printf("/rlist : Shows a list of rooms.\n");
    printf("/myname  : Shows your name\n");
    printf("/mkroom <ROOM_NAME> : Make a room named ROOM_NAME\n");
    printf("/enter <ROOM_NUMBER> : Enter the room numbered ROOM_NUMBER\n");
    printf("/exit : Leave the room\n");
    printf("/info : Shows the information of the current room.\n");
    printf("/dm <Target> <message> : Send a message to the Target.\n");
    
}
