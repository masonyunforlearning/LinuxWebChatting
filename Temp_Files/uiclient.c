//전역변수버전

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <pthread.h>

#define WIDTH 75                                                                                                        
#define HEIGHT 20
#define BUF_SIZE 10240
/**********프로토콜 모음************/
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
int intheroom = 0; //현재 내가 방에 있는지 없는지 확인
int roomnumber = 10; // 최대 방 개수(서버가 제한하는 방 개수에 따라 바뀔 수 있음)
char myname[BUF_SIZE];
void error_handling(char* message);
int chatting(int sock_id);
int received(int sock_id);
void seecommand();

//UI
void edit_name();
int startx = (80 - WIDTH) / 2;
int starty = (24 - HEIGHT) / 2;
int chatline = 0;
WINDOW* log_win;
WINDOW* input_win;
//

void* p_writing_function(void* sock) {
    //printf("writing function success\n");
    int sock_id = *(int*)sock;
    //printf("Writing socket : %d\n", sock_id);
    while (1)
    {
        wprintw(log_win, "매뉴얼을 보려면 /help 혹은 /?를 입력하세요.\n");
        wrefresh(log_win);
        if (chatting(sock_id) == 1) {
            return;
            break;
        }
        else
            error_handling("unknown error");
        refresh();
    }
}

void* p_reading_function(void* sock) {
    //printf("reading function success\n");
    int sock_id = *(int*)sock;
    //printf("reading socket : %d\n", sock_id);
    while (1)
    {
        received(sock_id);
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ko_KR.utf8");    //ncurses 한글 출력을 위함
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    bkgd(COLOR_PAIR(1));
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_CYAN, COLOR_WHITE);

    int status;
    pthread_t writing;
    pthread_t reading;


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
    else // 이름 입력
    {
        WINDOW* edit_name_win;
        edit_name_win = newwin(HEIGHT, WIDTH, starty, startx);
        wbkgd(edit_name_win, COLOR_PAIR(2));
        box(edit_name_win, 0, 0);
        wborder(edit_name_win, '|', '|', '-', '-', '+', '+', '+', '+');
        refresh();
        mvwprintw(edit_name_win, 2, (WIDTH / 2) - 21, "LinuxWebChatting에 오신 것을 환영합니다.");
        mvwprintw(edit_name_win, 3, (WIDTH / 2) - 20, "채팅에서 사용하실 이름을 설정해주세요.");
        mvwprintw(edit_name_win, 9, (WIDTH / 2) - 14, "사용자의 이름을 입력해주세요.");
        echo();
        mvwgetstr(edit_name_win, 10, (WIDTH / 2), myname);
        token = strtok(myname, "\n");
        sprintf(buf, "%s%s%s%s\n", INFO, NAME, myname, protocol);
        write(sock, buf, strlen(buf));
        while (1) {
            int get = read(sock, message, BUF_SIZE);
            printw("%s\n", message);
            if (get != 0) break;
        }
        noecho();
        clear();
        delwin(edit_name_win);
        refresh();
    }

    input_win = newwin(3, WIDTH, HEIGHT, 2);
    log_win = newwin(HEIGHT - 2, WIDTH, 1, 2);
    wbkgd(input_win, COLOR_PAIR(2));
    wbkgd(log_win, COLOR_PAIR(2));
    scrollok(input_win, TRUE);
    scrollok(log_win, TRUE);

    //printf("Success=====================\n");

    pthread_create(&writing, NULL, p_writing_function, (void*)&sock);
    pthread_create(&reading, NULL, p_reading_function, (void*)&sock);

    //printf("Success2====================\n");


    pthread_join(writing, (void**)&status); //6
    pthread_join(reading, (void**)&status);
    close(sock);

    return 0;
}
int chatting(int sock_id)
{
    char message[BUF_SIZE];
    char buf[BUF_SIZE] = "";
    char* token;

    while (1)
    {
        echo();
        wgetstr(input_win, message);
        wclear(input_win);
        wrefresh(input_win);

        if (message[0] == '/')//명령어 파트(dm포함)
        {
            //어디서든 사용가능
            if (!strcmp(message + 1, "help") || !strcmp(message + 1, "?")) //명령어 목록 및 클라이언트정보등
            {
                seecommand();
            }
            else if (!strcmp(message + 1, "quit")) //클라이언트 종료
            {
                endwin();
                return 1;
            }
            else if (!strcmp(message + 1, "myname")) //내이름 표시
            {
                wprintw(log_win, "나의 이름 : %s\n\n", myname);
            }
            else
            {
                token = strtok(message, " ");
                if (!strcmp(token, "/dm")) //DM
                {
                    strcpy(buf, DMSG);
                    token = strtok(NULL, " ");
                    strcat(buf, token); //DM 대상
                    if (token[0] == '\0')
                    {
                        wprintw(log_win, "/dm 형식에 맞지 않습니다. 다시 시도해주세요 (/dm <대상> <메세지>)\n\n");
                    }
                    else
                    {
                        token = strtok(NULL, "\0");
                        if (token == NULL || token[0] == '\0')
                        {
                            wprintw(log_win, "/dm 형식에 맞지 않습니다. 다시 시도해주세요 (/dm <대상> <메세지>)\n\n");
                        }
                        else
                        {
                            strcat(buf, protocol);
                            strcat(buf, token);//메세지
                            strcat(buf, "_##_\n");
                            write(sock_id, buf, strlen(buf));
                        }
                    }
                }
                else if (!strcmp(token, "/exit"))
                {
                    if (intheroom)
                    {
                        sprintf(buf, "%s%s", ROOM, EXIT);
                        write(sock_id, buf, strlen(buf));
                        intheroom--;
                    }
                    else
                        wprintw(log_win, "이 명령어는 채팅방 안에서만 사용 가능합니다.\n\n");

                }
                else if (!strcmp(token, "/info"))
                {
                    if (intheroom)
                    {
                        sprintf(buf, "%s%s", ROOM, ROIN);
                        write(sock_id, buf, strlen(buf));
                    }
                    else
                        wprintw(log_win, "이 명령어는 채팅방 안에서만 사용 가능합니다.\n\n");

                }
                else if (!strcmp(token, "/mkroom"))
                {
                    token = strtok(NULL, "\0");
                    if (intheroom)
                    {
                        wprintw(log_win, "/mkroom 형식에 맞지 않습니다. 다시 시도해 주세요 (/mkroom <방이름>)\n\n");
                    }
                    else if (token == NULL || token[0] == '\0')
                    {
                        wprintw(log_win, "/mkroom 형식에 맞지 않습니다. 다시 시도해 주세요 (/mkroom <방이름>)\n\n");
                    }
                    else
                    {
                        sprintf(buf, "%s%s%s%s\n", ROOM, MAKE, token, protocol);
                        write(sock_id, buf, strlen(buf));
                    }
                }
                else if (!strcmp(token, "/enter"))
                {
                    if (intheroom)
                    {
                        wprintw(log_win, "이 명령어는 로비 안에서만 사용 가능합니다.\n\n");
                    }
                    else
                    {
                        token = strtok(NULL, "\0");
                        if (atoi(token) > roomnumber)
                        {
                            wprintw(log_win, "채팅방이 너무 많습니다. 나중에 다시 시도해주세요.\n\n");
                            continue;
                        }
                        else
                        {
                            if (token == NULL || token[0] == '\0')
                            {
                                wprintw(log_win, "/enter 형식에 맞지 않습니다. 다시 시도해주세요 (/enter <방번호>)\n\n");
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
                else if (!strcmp(token, "/mlist"))
                {
                    sprintf(buf, "%s%s", INFO, MIST);
                    write(sock_id, buf, strlen(buf));
                }
                else if (!strcmp(token, "/rlist"))
                {
                    sprintf(buf, "%s%s", INFO, RIST);
                    write(sock_id, buf, strlen(buf));
                }
                else //그외 잘못된 명령문
                    wprintw(log_win, "유효하지않은 명령어이거나 형식에 맞지 않습니다. 매뉴얼을 보려면 /help 혹은 /?를 입력하세요.\n\n");
            }

        }
        else//채팅파트
        {
            if (intheroom)//현재 방에 속해있다면
            {
                if (message[0] != '\0')
                {
                    token = strtok(message, "\0");

                    wattron(log_win, COLOR_PAIR(3));
                    wprintw(log_win, "%s\n", token);
                    wattroff(log_win, COLOR_PAIR(3));

                    sprintf(buf, "%s%s%s\n", SEND, token, protocol);

                    write(sock_id, buf, strlen(buf));
                }
            }
            else //아닐경우(로비에 있을경우)
            {
                wprintw(log_win, "채팅은 채팅방 안에서만 가능합니다.\n\n");
            }

        }
        wrefresh(log_win);
    }
    refresh();
    return 5;
}

int received(int sock_id)
{
    int str_len;
    char message[BUF_SIZE];
    char** msgArr;

    str_len = read(sock_id, message, BUF_SIZE - 1);

    message[str_len] = 0;
    wprintw(log_win, "%s", message);
    wrefresh(log_win);
    refresh();
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void seecommand() {

    wprintw(log_win, "/help /? : 명령어 보기\n");
    wprintw(log_win, "/quit : 프로그램 종료\n");
    wprintw(log_win, "/mlist : 접속자 정보 보기\n");
    wprintw(log_win, "/rlist : 방 목록 보기\n");
    wprintw(log_win, "/myname  : 내 이름 확인하기\n");
    wprintw(log_win, "/mkroom <ROOM_NAME> : 입력한 이름으로 채팅방을 생성\n");
    wprintw(log_win, "/enter <ROOM_NUMBER> : 입력한 번호의 채팅방으로 입장\n");
    wprintw(log_win, "/exit : 채팅방 나가기\n");
    wprintw(log_win, "/info : 현재 채팅방의 정보 보기\n");
    wprintw(log_win, "/dm <Target> <message> : 입력한 대상에게 귓속말을 보냄.\n\n");
}

