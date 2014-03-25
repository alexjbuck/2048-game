#include <iostream>
#include <fstream>
#include <ncurses.h>
#include <deque>
#include <random>
#include <algorithm>
#include <unistd.h>
#include <chrono>

typedef std::deque< std::deque<int> > Board;
#define WIN 		2048
#define LOSE 		1
#define PLAYING 	0
#define EXITING 	-1
#define KEY_QUIT 	113

class Game {
	public:
		std::default_random_engine xySeed;
		std::uniform_int_distribution<int> xyRand;
		std::default_random_engine nSeed;
		std::uniform_int_distribution<int> nRand;

		Board tiles;
		Board temp;
		int size;
		int xNew;
		int yNew;
		int score;
		int maxScore;
	    int playing; // still playing


	Game(int n) {
		unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
		unsigned seed2 = std::chrono::system_clock::now().time_since_epoch().count()*2;
		xySeed.seed(seed1);
		nSeed.seed(seed2);
		size = n;
		tiles.resize(n);
		temp.resize(n);
		for(int i=0;i<n;i++){
			tiles[i] = std::deque<int>(n);
			temp[i] = std::deque<int>(n);
		}
		xyRand = std::uniform_int_distribution<int>(0,n-1);
		nRand  = std::uniform_int_distribution<int>(1,100);
		score = 0;
		maxScore = readMaxScore();
		playing = PLAYING;
	}

	void init() {
		//addRand();
		tiles[0][3] = 2;
		tiles[3][3] = 2;
	}

	int readMaxScore() {
		std::ifstream scoreStream;
		scoreStream.open("score");
		if(scoreStream.is_open()) {
			std::string line;
			getline(scoreStream,line);
			maxScore = stoi(line);
		} else {
			maxScore = 0;
		}
		return maxScore;
	}

	void setMaxScore() {
		std::ofstream scoreStream;
		scoreStream.open("score",std::ios::trunc);
		if(scoreStream.is_open()) {
			scoreStream << score;
		}
	}

	void addRand() {
		if(checkWinLose()){
		//	return;
		}
		xNew = xyRand(xySeed);
		yNew = xyRand(xySeed);
		while (tiles[xNew][yNew]!=0) {
			xNew = xyRand(xySeed);
			yNew = xyRand(xySeed);
		}
		int num = nRand(nSeed);
		tiles[xNew][yNew] = (num<10)?4:2;
	}

	void transpose(Board tiles,Board* ret){
		for(int j=0;j<size;j++){
			(*ret)[j] = std::deque<int>(size);
		}
		for(int j=0;j<size;j++){
			for(int i=0;i<size;i++){
				(*ret)[j][i] = tiles[i][j];
			}
		}
		return;
	}

	bool UP() {
		bool success = FALSE;
		success |= up();
		success |= combine(KEY_UP,TRUE);
		print();
		sleep(1);
		success |= up();
		return success;
	}
	bool up() {
		bool success = FALSE;
		transpose(tiles,&temp);
		for(int j=0;j<size;j++) {
			int i = 0;
			int count = 0;
			while(i<size && count<size) {
				if(temp[j][i]==0){
					temp[j].erase(temp[j].begin()+i);
					temp[j].push_back(0);
					if(temp[j][i]!=0){
						success = TRUE;
					}
					count++;
				} else {
					i++;
				}
			}
		}
		transpose(temp,&tiles);
		return success;
	}

	bool DOWN() {
		bool success = FALSE;
		success |= down();
		success |= combine(KEY_DOWN,TRUE);
		print();
		sleep(1);
		success |= down();
		return success;
	}
	bool down() {
		bool success = FALSE;
		transpose(tiles,&temp);
		for(int j=0;j<size;j++) {
			int i = size-1;
			int count = 0;
			while(i>=0 && count<size){
				if(temp[j][i]==0){
					temp[j].erase(temp[j].begin()+i);
					temp[j].push_front(0);
					if(temp[j][i]!=0){
						success = TRUE;
					}
					count++;
				} else {
					i--;
				}

			}
		}
		transpose(temp,&tiles);
		return success;
	}

	bool LEFT() {
		bool success = FALSE;
		success |= left();
		success |= combine(KEY_LEFT,TRUE);
		print();
		sleep(1);
		success |= left();
		return success;
	}
	bool left() {
		bool success = FALSE;
		for(int i=0;i<size;i++) {
			int j = 0;
			int count = 0;
			while(j<size && count<size){
				if(tiles[i][j]==0){
					tiles[i].erase(tiles[i].begin()+j);
					tiles[i].push_back(0);
					if(tiles[i][j]!=0){
						success = TRUE;
					}
					count++;
				} else {
					j++;
				}
			}
		}
		return success;
	}

	bool RIGHT() {
		bool success = FALSE;
		success |= right();
		success |= combine(KEY_RIGHT,TRUE);
		print();
		sleep(1);
		success |= right();
		return success;
	}
	bool right() {
		bool success = FALSE;
		for(int i=0;i<size;i++) {
			int j = size-1;
			int count = 0;
			while(j>=0 && count<size){
				if(tiles[i][j]==0){
					tiles[i].erase(tiles[i].begin()+j);
					tiles[i].push_front(0);
					if(tiles[i][j]!=0){
						success = TRUE;
					}
					count++;
				} else {
					j--;
				}
			}
		}
		return success;
	}

	bool combine(int n,bool act) {
		bool success = FALSE;
		switch(n){
			case KEY_UP: // Up
				for(int i=0;i<size-1;i++){
					for(int j=0;j<size;j++){
						if(tiles[i][j]!=0 && tiles[i][j]==tiles[i+1][j]){
							success = TRUE;
							if(act){
								tiles[i][j] *= 2;
								tiles[i+1][j] = 0;
								score += tiles[i][j];
							}
						}
					}
				}
				break;
			case KEY_DOWN: // Down
				for(int i=size-1;i>=1;i--){
					for(int j=size-1;j>=0;j--){
						if(tiles[i][j]!=0 && tiles[i][j]==tiles[i-1][j]){
							success = TRUE;
							if(act){
								tiles[i][j] *= 2;
								tiles[i-1][j] = 0;
								score += tiles[i][j];
							}
						}
					}
				}
				break;
			case KEY_LEFT: // Left
				for(int i=0;i<size;i++){
					for(int j=0;j<size-1;j++){
						if(tiles[i][j]!=0 && tiles[i][j]==tiles[i][j+1]){
							success = TRUE;
							if(act){
								tiles[i][j] *= 2;
								tiles[i][j+1] = 0;
								score += tiles[i][j];
							}
						}
					}
				}
				break;
			case KEY_RIGHT: // Right
				for(int i=size-1;i>=0;i--){
					for(int j=size-1;j>=1;j--){
						if(tiles[i][j]!=0 && tiles[i][j]==tiles[i][j-1]){
							success = TRUE;
							if(act){
								tiles[i][j] *= 2;
								tiles[i][j-1] = 0;
								score += tiles[i][j];
							}
						}
					}
				}
				break;
		}
		return success;
	}

	int checkWinLose() {
		if(playing==EXITING) {return EXITING;}
		int win = (int)checkWin();
		int lose = (int)checkLose();
		if(win!=0){
			return WIN;
		} else { // I didn't win, so return if I lost or not
			return lose;
		}
	}

	bool checkWin() {
		bool win = FALSE;
		for(int i=0;i<size;i++) {
			for(int j=0;j<size;j++) {
				if(tiles[i][j]==2048) {
					win = TRUE;
					return win;
				}
			}
		}
		return win;
	}

	bool checkLose() {
		// TRUE means win or loss
		// FALSE means didn't win and didn't lose
		bool lose = TRUE;
		// comb is TRUE if any direction could create a combination (didn't lose)
		bool comb = combine(KEY_UP,FALSE) | combine(KEY_DOWN,FALSE) | combine(KEY_LEFT,FALSE) | combine(KEY_RIGHT,FALSE);

		for(int i=0;i<size;i++) {
			for(int j=0;j<size;j++) {
				if(tiles[i][j]==0) {
					lose = FALSE;
					return lose;
				}
			}
		}
		lose &= !comb; // only lose if all tiles are filed AND there are no combinations available
		return lose;
	}

	void update() {
		addRand();
		print();
	}

	void print() {
		clear();
		printw("2048 by Alexander Buck\n");
		printw("SCORE: %10.1d\tMAX SCORE: %10.1d\n\n",score,maxScore);
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				if(tiles[i][j]!=0){
					printw(" [%4.1d ] ",tiles[i][j]);
				} else {
					printw("  %4.1d   ",tiles[i][j]);
				}
			}
			printw("\n\n");
		}
		refresh();
	}
};


int main(int argc, char* argv[]) {
	int ch;
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    int N;
    if(argc>=2){
    	N = (int)atoi(argv[1]);
    } else {
    	N = 4;
    }
    Game g = Game(N);
    g.init();
    g.print();
    bool success = FALSE;
    while (g.playing==PLAYING) {
        if ((ch = getch()) == ERR) {
        	// Do nothing until key is pressed
        }
        else {
            switch(ch){
            	case KEY_UP:
            		success = g.UP();
            		break;
        		case KEY_DOWN:
        			success = g.DOWN();
        			break;
        		case KEY_LEFT:
        			success = g.LEFT();
        			break;
        		case KEY_RIGHT:
        			success = g.RIGHT();
        			break;
        		case KEY_QUIT:
        			g.playing = EXITING;
        			break;
        		default:
        			success = FALSE;
        			break;
            }
            if(success) {
            	g.update();
            } else {
            	printw("That is not a valid move\n");
            }
            if((g.playing=g.checkWinLose()) != PLAYING ){ // Not still playing

            	g.print();
            	switch(g.playing) {
            		case WIN:
            			printw("You Won! Press 'q' to exit\n");
            			break;
            		case LOSE:
            			printw("You Lost! Press 'q' to exit\n");
            			break;
            		case EXITING:
            			printw("You Quit! Press 'q' again to exit\n");
            			break;
            	}
            	if(g.score>g.maxScore) {
            		printw("New Max Score!! %10.1d\n",g.score);
            		g.setMaxScore();
            	}
            }
        }
    }
    for(;;){
    	if((ch=getch())==KEY_QUIT){
    		endwin();
    		return 0; // q
    	}
    }   
}

