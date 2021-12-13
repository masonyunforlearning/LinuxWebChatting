#include <stdio.h>                          
#include<locale.h>
#include <ncursesw/ncurses.h>                                                                                                    
                                                                                                                        
#define WIDTH 60                                                                                                        
#define HEIGHT 20                                                                                                       
                                                                                                                        
int startx = 0;                                                                                                         
int starty = 0;                                                                                                         
                                                                                                                        
char *choices[] = {                                                                                                     
                        "1. 수다 떠실분 들어오세요",                                                                                     
                        "2. 쪽문에서 치맥하실 분 구함",                                                                                     
                        "3. 우리 소통해요 채팅방",                                                                                     
                        "4. 시스템프로그래밍 회의방",                                                                                     
                        "프로그램 종료",                                                                                         
                  };                                                                                                    
int n_choices = sizeof(choices) / sizeof(char *);                                                                       
void print_menu(WINDOW *menu_win, int highlight);                                                                       
                                                                                                                        
int main()                                                                                                              
{      setlocale(LC_ALL, "ko_KR.utf8");
	WINDOW *menu_win;                                                                                               
        int highlight = 1;                                                                                              
        int choice = 0;                                                                                                 
        int c;                                                                                                          
                                                                                                                        
        initscr();

	start_color();

	init_pair(1,COLOR_BLACK,COLOR_YELLOW);

	bkgd(COLOR_PAIR(1));

        clear();                                                                                                        
        noecho();                                                                                                       
        cbreak();       /* Line buffering disabled. pass on everything */                                               
        startx = (80 - WIDTH) / 2;                                                                                      
        starty = (24 - HEIGHT) / 2;                                                                                     
                                                                                                                        
        menu_win = newwin(HEIGHT, WIDTH, starty, startx);                                                               
        keypad(menu_win, TRUE);                                                                                         
        mvprintw(0, 0, "                             < 채팅방 입장>\n방향키를 입력하여 방을 선택할 수 있고, Enter키를 눌러 방에 입장 할 수 있습니다.");                             
        refresh();                                                                                                      
        print_menu(menu_win, highlight);                                                                                
        while(1)                                                                                                        
        {       c = wgetch(menu_win);                                                                                   
                switch(c)                                                                                               
                {       case KEY_UP:                                                                                    
                                if(highlight == 1)                                                                      
                                        highlight = n_choices;                                                          
                                else                                                                                    
                                        --highlight;                                                                    
                                break;                                                                                  
                        case KEY_DOWN:                                                                                  
                                if(highlight == n_choices)                                                              
                                        highlight = 1;                                                                  
                                else                                                                                    
                                        ++highlight;                                                                    
                                break;                                                                                  
                        case 10:                                                                                        
                                choice = highlight;                                                                     
                                break;                                                                                  
                        default:                                                                                        
                                mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c); 
                                refresh();                                                                              
                                break;                                                                                  
                }                                                                                                       
                print_menu(menu_win, highlight);                                                                        
                if(choice != 0) /* User did a choice come out of the infinite loop */                                   
                        break;                                                                                          
        }                                                                                                               
        mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);                    
        clrtoeol();                                                                                                     
        refresh();                                                                                                      
        endwin();                                                                                                       
        return 0;                                                                                                       
}                                                                                                                       
                                                                                                                        
                                                                                                                        
void print_menu(WINDOW *menu_win, int highlight)                                                                        
{                                                                                                                       
        int x, y, i;                                                                                                    
                                                                                                                        
        x = 2;                                                                                                          
        y = 2;              

	init_pair(2,COLOR_BLACK,COLOR_WHITE);
	wbkgd(menu_win,COLOR_PAIR(2));	
        box(menu_win, 0, 0);
	wborder(menu_win, '|', '|', '-', '-', '+', '+', '+', '+');

        for(i = 0; i < n_choices; ++i)                                                                                  
        {       if(highlight == i + 1) /* High light the present choice */                                              
                {       wattron(menu_win, A_REVERSE);                                                                   
                        mvwprintw(menu_win, y, x, "%s", choices[i]);                                                    
                        wattroff(menu_win, A_REVERSE);                                                                  
                }                                                                                                       
                else                                                                                                    
                        mvwprintw(menu_win, y, x, "%s", choices[i]);                                                    
                ++y;                                                                                                    
        }                                                                                                               
        wrefresh(menu_win);                                                                                             
} 
