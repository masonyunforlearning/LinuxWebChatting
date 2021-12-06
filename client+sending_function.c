#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#define BUF_SIZE 1024
/**********�������� ����************/
char protocol[7] = "_##_";
char buf[BUF_SIZE] = "";
char ROOM[9] = "_##_ROOM";
    char MAKE[11] = "_##_MAKE\n";
    char ENTER[9] = "_##_ENTE";
    char EXIT[11] = "_##_EXIT\n";
    char ROIN[11] = "_##_ROIN\n";
    char LIST[11] = "_##_LIST\n";

char SEND[21] = "_##_CHAT_##_SEND_##_";
char DMSG[21] = "_##_CHAT_##_DMSG_##_";
char INFO[9] = "_##_INFO";
    char MIST[11] = "_##_MIST\n";
    char RIST[11] = "_##_RIST\n";
    char NAME[9] = "_##_NAME";
/*********************************/
int intheroom = 0;
char myname[20];
void error_handling(char* message);
int chatting(int sock_id);
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
    strcat(buf, myname);
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

        if (pid == 0) {
            //�޼��� �������� �ޱ�
            str_len = read(sock, message, BUF_SIZE - 1);
            message[str_len] = 0;
            printf("%s", message);
        }

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
                 printf("Your name : %s", myname);
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
                         token = strtok(NULL, "\0");
                         if (token == NULL || token[0] == '\n')
                         {
                             printf("Does not fit the format of /dm\n");
                         }
                         else
                         {
                             strcat(buf, protocol);
                             strcat(buf, token);//�޼���
                             write(sock_id, buf, strlen(buf));
                         }
                     }
                 }
                 else if (!strcmp(token, "/exit\n"))
                 {
                     if (intheroom)
                     {
                         strcpy(buf, ROOM);
                         strcat(buf, EXIT);
                         write(sock_id, buf, strlen(buf));
                     }
                     else
                         printf("you can use this command only in the room\n");
                     
                 }
                 else if (!strcmp(token, "/info\n"))
                 {
                     if (intheroom)
                     {
                         strcpy(buf, ROOM);
                         strcat(buf, ROIN);
                         printf("message : %s", buf);
                         write(sock_id, buf, strlen(buf));
                     }
                     else
                         printf("you can use this command only in the room\n");
                     
                 }
                 else if (!strcmp(token, "/mkroom\n"))
                 {
                     token = strtok(NULL, " ");
                     if (intheroom)
                         printf("you can use this command only in the lobby\n");
                     else
                     {
                         strcpy(buf, ROOM);
                         strcat(buf, MAKE);
                         strcat(buf,token);
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

                         strcpy(buf, ROOM);
                         strcat(buf, ENTER);
                         token = strtok(NULL, " ");
                         strcat(buf, protocol);
                         if (token[0] == '\n')
                         {
                             printf("Does not fit the format of /enter\n");
                         }
                         else
                         {
                             strcat(buf, token);
                             write(sock_id, buf, strlen(buf));
                         }

                     }
                     
                 }
                 else if (!strcmp(token, "/mlist\n"))
                 {
                         strcpy(buf, INFO);
                         strcat(buf, MIST);
                         write(sock_id, buf, strlen(buf));
                 }
                 else if (!strcmp(token, "/rlist\n"))
                 {
                     strcpy(buf, INFO);
                     strcat(buf, RIST);
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
                     strcpy(buf, SEND);
                     strcat(buf, message);
                     write(sock_id,buf,strlen(buf));
                 }
             }
             else //�ƴҰ��(�κ� �������)
             {
                 printf("lobby\n");
             }
             
         }
         
    }
   
    return 5;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void seecommand() {

    printf("/help /? : see command\n/q  /Q : quit client program\n/makeroom : make a room\n");
}
