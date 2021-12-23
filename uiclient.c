#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <ncursesw/ncurses.h>
#include<locale.h>

#define WIDTH 60                                                                                                        
#define HEIGHT 20
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

//UI
void edit_name();
int startx = (80 - WIDTH) / 2;
int starty = (24 - HEIGHT) / 2;
int chatline = 0;

//

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ko_KR.utf8");    //ncurses �ѱ� ����� ����
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    bkgd(COLOR_PAIR(1));

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
    else // �̸� �Է�
    {
        WINDOW* edit_name_win;
        edit_name_win = newwin(HEIGHT, WIDTH, starty, startx);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
        wbkgd(edit_name_win, COLOR_PAIR(2));
        box(edit_name_win, 0, 0);
        wborder(edit_name_win, '|', '|', '-', '-', '+', '+', '+', '+');
        refresh();
        mvwprintw(edit_name_win, 1, 10, "LinuxWebChatting�� ���� ���� ȯ���մϴ�.");
        mvwprintw(edit_name_win, 2, 11, "ä�ÿ��� ����Ͻ� �̸��� �������ּ���.");
        mvwprintw(edit_name_win, 6, 15, "����� �̸��� �����ΰ���?:");
        echo();
        mvwgetstr(edit_name_win, 8, 15, myname);
        token = strtok(myname, "\n");
        sprintf(buf, "%s%s%s%s\n", INFO, NAME, myname, protocol);
        write(sock, buf, strlen(buf));
        while (1) {
            int get = read(sock, message, BUF_SIZE);
            printw("%s\n", message);
            if (get != 0) break;
        }
        noecho();
        refresh();
        clear();
        delwin(edit_name_win);
    }

    pid_t pid = fork();
    
    if (pid != 0) {
        endwin();
    }

    while (1) {

        if (pid != 0)
        {
            //�޼��� ������ ������
            printw("�Ŵ����� ������ /help Ȥ�� /?�� �Է��ϼ���.");
            if (chatting(sock) == 1) {
                kill(pid, SIGINT);
                break;
            }
            else
                error_handling("unknown error");
        }

        if (pid == 0)
            received(sock);
        refresh();
    }
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
        move(30, 0);
        getstr(message);

        if (message[0] == '/')//��ɾ� ��Ʈ(dm����)
        {
            //��𼭵� ��밡��
            if (!strcmp(message + 1, "help") || !strcmp(message + 1, "?")) //��ɾ� ��� �� Ŭ���̾�Ʈ������
            {
                seecommand();
            }
            else if (!strcmp(message + 1, "quit")) //Ŭ���̾�Ʈ ����
            {
                endwin();
                return 1;
            }
            else if (!strcmp(message + 1, "myname")) //���̸� ǥ��
            {
                printw("���� �̸� : %s\n", myname);
            }
            else
            {
                token = strtok(message, " ");
                if (!strcmp(token, "/dm")) //DM
                {
                    strcpy(buf, DMSG);
                    token = strtok(NULL, " ");
                    strcat(buf, token); //DM ���
                    if (token[0] == '\0')
                    {
                        printw("/dm ���Ŀ� ���� �ʽ��ϴ�. �ٽ� �õ����ּ��� (/dm <���> <�޼���>)\n");
                    }
                    else
                    {
                        token = strtok(NULL, "\0");
                        if (token == NULL || token[0] == '\0')
                        {
                            printw("/dm ���Ŀ� ���� �ʽ��ϴ�. �ٽ� �õ����ּ��� (/dm <���> <�޼���>)\n");
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
                else if (!strcmp(token, "/exit"))
                {
                    if (intheroom)
                    {
                        sprintf(buf, "%s%s", ROOM, EXIT);
                        write(sock_id, buf, strlen(buf));
                        intheroom--;
                    }
                    else
                        printw("�� ��ɾ�� ä�ù� �ȿ����� ��� �����մϴ�.\n");

                }
                else if (!strcmp(token, "/info"))
                {
                    if (intheroom)
                    {
                        sprintf(buf, "%s%s", ROOM, ROIN);
                        write(sock_id, buf, strlen(buf));
                    }
                    else
                        printw("�� ��ɾ�� ä�ù� �ȿ����� ��� �����մϴ�.\n");

                }
                else if (!strcmp(token, "/mkroom"))
                {
                    token = strtok(NULL, "\0");
                    if (intheroom)
                    {
                        printw("/mkroom ���Ŀ� ���� �ʽ��ϴ�. �ٽ� �õ��� �ּ��� (/mkroom <���̸�>)\n");
                    }
                    else if (token == NULL || token[0] == '\0')
                    {
                        printw("/mkroom ���Ŀ� ���� �ʽ��ϴ�. �ٽ� �õ��� �ּ��� (/mkroom <���̸�>)\n");
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
                        printw("�� ��ɾ�� �κ� �ȿ����� ��� �����մϴ�.\n");
                    }
                    else
                    {
                        token = strtok(NULL, "\0");
                        if (atoi(token) > roomnumber)
                        {
                            printw("ä�ù��� �ʹ� �����ϴ�. ���߿� �ٽ� �õ����ּ���.\n");
                            continue;
                        }
                        else
                        {
                            if (token == NULL || token[0] == '\0')
                            {
                                printw("/enter ���Ŀ� ���� �ʽ��ϴ�. �ٽ� �õ����ּ��� (/enter <���ȣ>)\n");
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
                else //�׿� �߸��� ��ɹ�
                    printw("��ȿ�������� ��ɾ��̰ų� ���Ŀ� ���� �ʽ��ϴ�. �Ŵ����� ������ /help Ȥ�� /?�� �Է��ϼ���.\n\n");
            }

        }
        else//ä����Ʈ
        {
            if (intheroom)//���� �濡 �����ִٸ�
            {
                if (message[0] != '\0')
                {
                    token = strtok(message, "\0");
                    sprintf(buf, "%s%s%s\n", SEND, token, protocol);
                    write(sock_id, buf, strlen(buf));
                }
            }
            else //�ƴҰ��(�κ� �������)
            {
                printw("ä���� ä�ù� �ȿ����� �����մϴ�.\n");
            }

        }

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
    printw("%s", message);
    refresh();
}

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void seecommand() {

    printw("/help /? : ��ɾ� ����\n");
    printw("/quit : ���α׷� ����\n");
    printw("/mlist : ������ ���� ����\n");
    printw("/rlist : �� ��� ����\n");
    printw("/myname  : �� �̸� Ȯ���ϱ�\n");
    printw("/mkroom <ROOM_NAME> : �Է��� �̸����� ä�ù��� ����\n");
    printw("/enter <ROOM_NUMBER> : �Է��� ��ȣ�� ä�ù����� ����\n");
    printw("/exit : ä�ù� ������\n");
    printw("/info : ���� ä�ù��� ���� ����\n");
    printw("/dm <Target> <message> : �Է��� ��󿡰� �ӼӸ��� ����.\n");
}