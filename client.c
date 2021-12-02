#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#define BUF_SIZE 1024
void error_handling(char* message);
int chatting(int sock_id);
void seecommand();

int main(int argc, char* argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
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
    else {
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
            //메세지 서버에 보내기
            printf("if you want manual, type  /help or /?\n");
            if (chatting(sock) == 1) {
                kill(pid, SIGINT);
                break;
            }
            
        }

        if (pid == 0) {
            //메세지 서버에서 받기
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
    char mr[BUF_SIZE] = "makeroom";
    char clearbuf[] = "";
    while (1)
    {
         fgets(message, BUF_SIZE, stdin);
         
         if (message[0] == '/' )
         {
                
             if (!strcmp(message + 1, "help\n") || !strcmp(message + 1, "?\n"))
             {
                 seecommand();
             }
             else if (!strcmp(message + 1, "q\n") || !strcmp(message + 1, "Q\n"))
             {
                 return 1;
             }
             else if (!strcmp(message + 1, "makeroom\n"))
             {
                 printf("i want room\n");
                 write(sock_id, message, strlen(message));
             }
             else
                 printf("if you want manual, type  /help or /?\n");
         }
         else
            write(sock_id, message, strlen(message));
    }
   
    return 0;
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void seecommand() {

    printf("/help /? : see command\n/q  /Q : quit client program\n/makeroom : make a room\n");
}
