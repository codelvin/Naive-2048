#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <curses.h>
using namespace std;

#define N 4
#define WIDTH 4

//game condition
#define FAIL 0
#define WIN 1
#define NORMAL 2
#define QUIT 3
#define TARGET 128

class Game2048{
    public:
        Game2048(): status(NORMAL){
        	restart();
        };

        void draw_screen(){
            //draw the interface
            clear();
            for(int i=0;i<2*N+1;i++){
                for(int j=0;j<N*(WIDTH+1)+1;j++){
                    //draw that plus
                    if(i%2==0 && j%(WIDTH+1)==0)
                        draw(i, j, '+');
                    else if(i%2==0 && j%(WIDTH+1)!=0)
                        draw(i, j, '-');
                    else if(i%2==1  && j%(WIDTH+1)==0)
                        draw(i, j, '|');
                }
            }
			for(int i=0;i<4;i++){
                for(int j=0;j<4;++j){
                    draw_num(i,j,data[i][j]);
                }
            }
            if (status==NORMAL)
            	mvprintw(2*N+3, 0,  "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");
            else if(status==WIN)
            	mvprintw(2*N+3, 0,  "You won, choose R to restart a game");
            else if(status == FAIL)
            	mvprintw(2*N+3, 0, "You lose, choose R to restart or Q to quit");
        }
        
        void process_input(){
            char ch = getch();
            bool update = false;
            if(ch>='a'&&ch<='z')
                ch -= 32;

            
            switch(ch){
            	case 'Q':status=QUIT;break;
            	case 'R':restart();break;
            	case 'A':update = move_left();break;
            	case 'S':rotate();rotate();rotate();update=move_left();rotate();break;
            	case 'D':rotate();rotate();update=move_left();rotate();rotate();break;
            	case 'W':rotate();update=move_left();rotate();rotate();rotate();break;
            	default: break;
            }
            
            // put a random number in vacant place
            if(update){
            	rand_new();
            	if(is_over())
            		status = FAIL;
            }
        }
 
        int get_status(){
            return status;
        }



    private:
		int data[N][N]={0};
		int status;

		bool is_over(){
        	for (int i = 0; i < N; ++i) {
            	for (int j = 0; j < N; ++j) {
                	// 有空位或者相邻有一样的都可以继续
                	if ((j + 1 < N) && (data[i][j] * data[i][j+1] == 0 || data[i][j] == data[i][j+1])) return false;
                	if ((i + 1 < N) && (data[i][j] * data[i+1][j] == 0 || data[i][j] == data[i+1][j])) return false;
            	}
        	}
        	return true;
        }

        void draw(int row, int col, char c){
            move(row, col);
            addch(c);
        }

        void draw_num(int row_grid, int col_grid, int num){
            //grid starts from 0, so 0-3
			int tmp_col = col_grid*(WIDTH+1)+4;
            for(int j = tmp_col;j>tmp_col-4;j--)
                draw(row_grid*2+1, j, ' ');
            while(num){
                draw(row_grid*2+1, tmp_col--, char(num%10+'0'));
                num/=10;
            }
        }

        void restart(){
            for (int i = 0; i < N; ++i) {
            	for (int j = 0; j < N; ++j) {
                	data[i][j] = 0;
            	}
        	}
        	rand_new();
        	rand_new();
        	status = NORMAL;
        }

        void rotate(){
        	// clockwise
        	int tmp[N][N]={0};
        	for (int i = 0; i < N; ++i) {
        		for (int j = 0; j < N; ++j) {
                	tmp[i][j] = data[j][N - 1 - i];
            	}
        	}
	        for (int i = 0; i < N; ++i) {
	            for (int j = 0; j < N; ++j) {
	                data[i][j] = tmp[i][j];
	            }
	        }
        }

        bool move_left(){
        	int tmp[N][N];

        	for(int i=0;i<N;i++){
        		int last = 0;
        		int cur = 0;
        		for(int j=0;j<N;j++){
        			tmp[i][j] = data[i][j];
        			if(data[i][j]==0)
        				continue;

        			if(last==0)
        				last = data[i][j];
        			else{
        				if(last==data[i][j]){
        					data[i][cur] = last*2;
        					last = 0;
        					if(data[i][cur]==TARGET)
        						status = WIN;
        				}
        				else{
        					data[i][cur] = last;
        					last = data[i][j];
        				}
        				cur++;
        			}
        			data[i][j]=0;
        		}
        		if(last!=0)
        			data[i][cur] = last;
        	}

        	for(int i=0;i<N;i++)
        		for(int j=0;j<N;j++){
        			if (tmp[i][j]!=data[i][j])
        				return true;
        		}
        	return false;
        }

        bool rand_new(){
        	vector<int> zero_pos;
        	for(int i=0;i<N;i++){
        		for(int j=0;j<N;j++){
        			if (data[i][j]==0)
        				zero_pos.push_back(i*N+j);
        		}
        	}
        	if(zero_pos.empty())
        		return false;
        	int pos = zero_pos[rand()%zero_pos.size()];
        	data[pos/N][pos%N] = rand()%10==1?4:2;
        	return true;

        }
};

void initialize() {
    initscr();// ncurses初始化
    cbreak();// 按键不需要输入回车直接交互
    noecho();// 按键不显示
    curs_set(0);// 隐藏光标
    srand(time(NULL));// 随机数
}

int main(){
    initialize();

    Game2048 game;
    do{
    	game.draw_screen();
        game.process_input();
        //DO STH
    }while(game.get_status()!=QUIT);
    endwin();
}
