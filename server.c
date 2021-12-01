#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

typedef struct type_room {
    int is_made;
    int cur_member;
    char roomName[256];
    int current_member[100];
    int max_member;
} room;

typedef struct type_socketInfo {
    int socket_number;
    int curr_roomNumber;
} socketinfo ;

void error_handling(char * buf);
int main(int argc, char * argv[]){
    room Server_room[10];
    
    for(int i = 0; i < 10; i++)
    {
        Server_room[i].cur_member = Server_room[i].is_made = Server_room[i].max_member = 0;
        Server_room[i].roomName[0] = '\0';
    }
    socketinfo Room_member_info[100];
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    int count_cli = 0;
    int counting=0;
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE*10];
    if(argc!=2){
        printf("Usage: %s<port>\n",argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    printf("%d\n", serv_sock);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;
    while(1){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if(fd_num == 0)
            continue;
        for(i = 0; i < fd_max+1; i++){
            if(FD_ISSET(i, &cpy_reads)){
                if(i == serv_sock){
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d\n", clnt_sock);
                    count_cli++;
                    counting++;
                    sprintf(buf, "Server : Welcome~\nServer : The number of clients is %d now.",counting);
                    write(clnt_sock, buf, sizeof(buf));
                    sprintf(buf, "Current available room is : \n");
                    write(clnt_sock, buf, sizeof(buf));
                    for(int i = 0; i < 10; i++)
                    {
                        if(Server_room[i].is_made == 0){

                        }else{
                            sprintf(buf, "%d room : %s (%d/%d)",i,Server_room[i].roomName,Server_room[i].cur_member,Server_room[i].max_member);
                            write(clnt_sock, buf, sizeof(buf));
                        }
                    }
                    sprintf(buf, "Enter the roomname");
                    write(clnt_sock, buf, sizeof(buf));
                
                    for(int k = 4; k < 4+count_cli; k++)
                    {
                        if(k != clnt_sock){
                            sprintf(buf,"client %d has joined this chatting room\nHello~\n",clnt_sock);
                            write(k, buf,sizeof(buf));
                        }
                    }
                }else {
                    str_len = read(i, buf, BUF_SIZE);
                    if(str_len == 0){
                        for(int k = 4; k < 4+count_cli; k++)
                        {
                            if(k != i){
                               sprintf(buf,"client %d has left this chatting room\n",i);
                                write(k, buf,sizeof(buf));
                            }
                        }
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                        //count_cli--;
                        counting--;
                    }else{
                       

                        for(int k = 4; k < 4+count_cli; k++)
                        {
                            if(k != i){
                                sprintf(buf2,"client %d : ",i);
                                int temp = strlen(buf2);
                                strncat(buf2,buf,str_len);
                                write(k,buf2,str_len+temp);
                            }
                        }
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char * buf){
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}