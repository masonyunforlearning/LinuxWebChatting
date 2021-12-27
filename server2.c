#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 10240

typedef struct type_room {
    int is_made;
    int cur_member;
    char roomName[256];
    int current_member[100];
    int max_member;
    char password[256];
} room;

typedef struct type_socketInfo {
    int socket_number;
    int curr_roomNumber;
    char nickname[30];
} socketinfo;

void room_info(room* servers, socketinfo* infos, int socketid);
void init_room(room* server_rooms, socketinfo* infos, char* room_name, int id,char* password, char* max_member);
void set_nickname(socketinfo* infos, char* nickname, int id);
void room_exit(room* servers, socketinfo* infos, int socketid);
void error_handling(char* buf);
int main(int argc, char* argv[]) {
    room Server_room[10];

    for (int i = 0; i < 10; i++)
    {
        Server_room[i].cur_member = Server_room[i].is_made = Server_room[i].max_member = 0;
        Server_room[i].roomName[0] = '\0';
        for (int k = 0; k < 100; k++)
            Server_room[i].current_member[k] = 0;
    }
    socketinfo Room_member_info[100];
    for (int i = 0; i < 100; i++)
    {
        Room_member_info[i].curr_roomNumber = Room_member_info[i].socket_number = -1;
    }
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;

    int count_cli = 0;
    int counting = 0;
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE];

    if (argc != 2) {
        printf("Usage: %s<port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    FD_ZERO(&reads);

    FD_SET(serv_sock, &reads);

    fd_max = serv_sock;

    while (1) {
        cpy_reads = reads;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if (fd_num == 0)
            continue;
        for (i = 3; i < fd_max + 1; i++) {
            if (FD_ISSET(i, &cpy_reads)) {
                if (i == serv_sock) {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d\n", clnt_sock);
                    count_cli++;
                    counting++;
                    // sprintf(buf, "Server : Welcome~\nServer : The number of clients is %d now.",counting);
                    // write(clnt_sock, buf, sizeof(buf));
                    strcpy(buf, "");
                    sprintf(buf, "Welcome\n");
                    write(clnt_sock, buf, sizeof(buf));

                    // for(int k = 4; k < 4+count_cli; k++)
                    // {
                    //     if(k != clnt_sock){
                    //         sprintf(buf,"client %d has joined this chatting room\nHello~\n",clnt_sock);
                    //         write(k, buf,sizeof(buf));
                    //     }
                    // }
                }
                else {
                    str_len = read(i, buf, BUF_SIZE);
                    buf[str_len] = 0;
                    char message[BUF_SIZE];
                    strcpy(message, buf);
                    printf("inserted message : %s\n", message);
                    if (str_len == 0) {
                        for (int k = 4; k < 4 + count_cli; k++)
                        {
                            if (k != i) {
                                sprintf(buf, "User %s has left this chatting room\n", Room_member_info[i].nickname);
                                write(k, buf, sizeof(buf));
                            }
                        }
                        room_exit(Server_room, Room_member_info, i);
                        Room_member_info[i].socket_number = -1;
                        Room_member_info[i].curr_roomNumber = -1;

                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                        //count_cli--;
                        counting--;
                    }
                    else {

                        char* ptr;
                        char* ptr2;//password
                        char* ptr3;//max member
                        ptr = strtok(message, "_##_");
                        if (!strcmp(ptr, "ROOM"))
                        {
                            printf("ROOM\n");
                            ptr = strtok(NULL, "_##_");
                            if (!strcmp(ptr, "MAKE"))
                            {
                                printf("MAKE\n");
                                ptr = strtok(NULL, "_##_");
                                ptr2 = strtok(NULL, "_##_");
                                ptr3 = strtok(NULL, "_##_");
                                printf("making_room : %s\n", ptr);
                                printf("password : %s\n",ptr2);
                                printf("max_member : %s\n",ptr3);
                                
                                 
                                init_room(Server_room, Room_member_info, ptr, i,ptr2,ptr3);
                            }
                            if (!strcmp(ptr, "ENTE"))
                            {
                                printf("ENTE\n");
                                printf("Matched\n");
                                ptr = strtok(NULL, "_##_");
                                ptr2 = strtok(NULL, "_##_"); //password
                                printf("Inserted Password : %s, Original Password : %s \n", ptr2, Server_room[atoi(ptr)].password);
                                if(!strcmp(ptr2,Server_room[atoi(ptr)].password)) //password correct
                                {
                                    printf("Entering room %d \n", atoi(ptr));
                                    Server_room[atoi(ptr)].cur_member++;
                                    printf("%d ?\n", Server_room[atoi(ptr)].cur_member);
                                    for (int k = 0; k < Server_room[atoi(ptr)].max_member; k++)
                                    {
                                        if (Server_room[atoi(ptr)].current_member[k] == 0 && Server_room[atoi(ptr)].is_made != 0)
                                        {
                                            printf("Room Enter to %d %dth %d\n", atoi(ptr), k, i);
                                            Server_room[atoi(ptr)].current_member[k] = i;
                                            break;
                                        }
                                    }
                                    Room_member_info[i].curr_roomNumber = atoi(ptr);
                                    printf("%s : %d\n", Room_member_info[i].nickname, atoi(ptr));
                                }
                                else //password fault
                                {
                                    printf("unmatched password\n");
                                }
                            }
                            if (!strcmp(ptr, "EXIT"))
                            {

                                printf("Exiting_room : %d\n", i);
                                room_exit(Server_room, Room_member_info, i);
                            }
                            if (!strcmp(ptr, "ROIN"))
                            {
                                printf("Sending Room info to %d\n", i);
                                room_info(Server_room, Room_member_info, i);
                            }
                        }

                        if (!strcmp(ptr, "INFO"))
                        {
                            ptr = strtok(NULL, "_##_");
                            if (!strcmp(ptr, "MIST"))
                            {
                                char user[BUF_SIZE] = "";
                                char sending[BUF_SIZE] = "";
                                for (int k = 0; k < 100; k++)
                                {
                                    if (Room_member_info[k].socket_number != -1)
                                    {
                                        sprintf(user, "%s (Room %d, %s)\n", Room_member_info[k].nickname, Room_member_info[k].curr_roomNumber, Server_room[Room_member_info[k].curr_roomNumber].roomName);
                                        strcat(sending, user);
                                    }
                                }
                                write(i, sending, BUF_SIZE);

                            }
                            if (!strcmp(ptr, "RIST"))
                            {
                                printf("Showing room\n");
                                sprintf(buf, "Current available room is : \n");

                                for (int k = 0; k < 10; k++)
                                {
                                    if (Server_room[k].is_made == 0) {


                                    }
                                    else {
                                        char temp_buf[1024];
                                        printf("%d room : %s (%d/%d)\n", k, Server_room[k].roomName, Server_room[k].cur_member, Server_room[k].max_member);
                                        sprintf(temp_buf, "%d room : %s (%d/%d)\n", k, Server_room[k].roomName, Server_room[k].cur_member, Server_room[k].max_member);
                                        strcat(buf, temp_buf);
                                    }
                                }

                                write(i, buf, sizeof(buf));
                            }
                            if (!strcmp(ptr, "NAME"))
                            {
                                ptr = strtok(NULL, "_##_");
                                printf("Setting %d's nickname\n", i);
                                set_nickname(Room_member_info, ptr, i);
                            }
                        }
                        if (!strcmp(ptr, "CHAT"))
                        {
                            ptr = strtok(NULL, "_##_");
                            if (!strcmp(ptr, "SEND"))
                            {
                                ptr = strtok(NULL, "_##_");
                                int current_room = Room_member_info[i].curr_roomNumber;
                                for (int k = 0; k < 100; k++)
                                {
                                    if (Server_room[current_room].current_member[k] && Server_room[current_room].current_member[k] != i)
                                    {
                                        printf("Sending to %d in room %d \n", k, current_room);
                                        sprintf(buf2, "%s : %s\n", Room_member_info[i].nickname, ptr);
                                        write(Server_room[current_room].current_member[k], buf2, BUF_SIZE);
                                    }
                                }
                            }
                            if (!strcmp(ptr, "DMSG"))
                            {
                                printf("dmsg provoked\n");
                                char unfound[] = "Can't find user\n";
                                char found[BUF_SIZE];
                                int who_send_to = i;
                                ptr = strtok(NULL, "_##_");
                                for (int k = 0; k < 100; k++)
                                {
                                    if (!strcmp(Room_member_info[k].nickname, ptr) && Room_member_info[k].socket_number != -1)
                                    {
                                        printf("??????? %d \n", k);
                                        who_send_to = k;
                                    }
                                }
                                if (who_send_to == i)
                                {
                                    printf("?????????\n");
                                    sprintf(buf2, "%s : ?????????\n", Room_member_info[i].nickname);
                                    write(i, buf2, sizeof(unfound));
                                }
                                else
                                {

                                    ptr = strtok(NULL, "_##_");

                                    strcpy(found, ptr);
                                    printf("??????? : %s\n", found);
                                    sprintf(buf2, "%s : %s\n", Room_member_info[i].nickname, ptr);
                                    write(who_send_to, buf2, BUF_SIZE);
                                }
                            }
                        }

                        // for(int k = 4; k < 4+count_cli; k++)
                        // {
                        //     if(k != i){
                        //         sprintf(buf2,"client %d : ",i);
                        //         int temp = strlen(buf2);
                        //         strncat(buf2,buf,str_len);
                        //         write(k,buf2,str_len+temp);
                        //     }
                        // }
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char* buf) {
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

void room_info(room* servers, socketinfo* infos, int socketid)
{
    char temp_buf[BUF_SIZE];
    char temp_buf2[100];
    char* test;
    int room_number = infos[socketid].curr_roomNumber;
    printf("%d's are in %d room\n ", socketid, room_number);
    sprintf(temp_buf, "************* ROOM INFO *************\n\n\tRoom Number : %d\n\tRoom Name: %s\n\tRoom member:\n", room_number, servers[room_number].roomName);
    for (int i = 0; i < 100; i++)
    {
        if (servers[room_number].current_member[i] != 0)
        {
            printf("%d checked : %s\n", i, infos[servers[room_number].current_member[i]].nickname);
            sprintf(temp_buf2, "\t%d: %s\n", i, infos[servers[room_number].current_member[i]].nickname);
            printf("%s\n", temp_buf2);
            test = strcat(temp_buf, temp_buf2);
        }
    }
    printf("Write to %d\n", socketid);
    printf("%s", temp_buf);
    write(socketid, test, BUF_SIZE);
}

void room_exit(room* servers, socketinfo* infos, int socketid)
{
    for (int i = 0; i < 100; i++) {
        if (servers[infos[socketid].curr_roomNumber].current_member[i] == socketid)
        {
            servers[infos[socketid].curr_roomNumber].current_member[i] = 0;
            servers[infos[socketid].curr_roomNumber].cur_member--;
            if (servers[infos[socketid].curr_roomNumber].cur_member == 0)
            {
                servers[infos[socketid].curr_roomNumber].is_made = 0;
            }
            break;
        }
    }
    infos[socketid].curr_roomNumber = -1;
}

void set_nickname(socketinfo* infos, char* nickname, int id)
{
    strcpy(infos[id].nickname, nickname);
    infos[id].socket_number = id;
}

void init_room(room* server_rooms, socketinfo* infos, char* room_name, int id, char* password, char* max_member)
{
    int max;
    if(max_member == NULL)
        max = 100;
    else
        max = atoi(max_member);
    
    printf("Setting max member to %d\n", max);

    for (int i = 0; i < 10; i++)
    {
        if (server_rooms[i].is_made == 0)
        {
            server_rooms[i].is_made = 1;
            for (int k = 0; k < max; k++)
            {
                server_rooms[i].current_member[k] = 0;
            }
            server_rooms[i].max_member = max;
            strcpy(server_rooms[i].roomName, room_name);
            strcpy(server_rooms[i].password, password);
            printf("Final setting : %d's room is %s %s %d\n", i, server_rooms[i].roomName, server_rooms[i].password, server_rooms[i].max_member);
            break;
        }
    }
}