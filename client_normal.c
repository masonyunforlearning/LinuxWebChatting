#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#define BUF_SIZE 10240
void error_handling(char *message);

int main(int argc, char * argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len;
    if(argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else{   
        while(1){
            int get = read(sock, message, BUF_SIZE);
            printf("%s\n", message);
            if(get != 0) break;
        }
    }
    
    pid_t pid = fork();
    while(1){
        if(pid != 0)
        {
            strcpy(message, "");
            fgets(message, BUF_SIZE, stdin);
            
            if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")){
                kill(pid,SIGINT);
                break;
            }
            write(sock, message, strlen(message));
        }
        
        if(pid == 0){
            str_len = read(sock, message, BUF_SIZE - 1);
            message[str_len] = 0;
            printf("%s", message);
        }

    }
    close(sock);

    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
