#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <vector>

// TODO
//   1. Board::move_by(int delta_row, delta_col)
//   2. move with keyboard
//   3. pick and show path of movement (somehow)


enum tiletype {
	tiletype_void ,
	tiletype_walkable ,
	tiletype_solid ,
};

const char * TILECHARACTER[] = {
	[tiletype_void] = " " ,
	[tiletype_walkable] = "." ,
	[tiletype_solid] = "#" ,
};

const char * CHAR_PLAYER = "@";


size_t const DUNGEON_SIZE_ROWS = 0x4;
size_t const DUNGEON_SIZE_COLS = 0x8;


struct Position {
	int row = 0;
	int col = 0;
};


struct Board {
	enum tiletype tile[DUNGEON_SIZE_ROWS][DUNGEON_SIZE_COLS] = { 0 };
	Position player;
	void wprint_board(WINDOW * w);
	void wprint_position(WINDOW * w);
	bool is_walkable(  int const r ,int const c );
	bool is_inside( int const r , int const c );
	void move_player( WINDOW * w , int const r , int const c );
	void move_player_by( WINDOW * w , int const delta_r , int const delta_c );
	void wprint_tile( WINDOW * w , int const r , int const c );
	void wprint_player(WINDOW * w);
};

void
Board::wprint_tile( WINDOW * w , int const r , int const c ) {
	mvwprintw(w , r , c , "%s" , TILECHARACTER[tile[r][c]]);
}


void
Board::wprint_player(WINDOW * w) {
	mvwprintw(w , player.row , player.col , "%s" , CHAR_PLAYER );
}


void
Board::wprint_board(WINDOW * w){
	wmove(w , 0 , 0);
	for( size_t r = 0; r < DUNGEON_SIZE_ROWS; ++r ) {
		for( size_t c = 0; c < DUNGEON_SIZE_COLS; ++c ) {
			wprintw( w , "%s" , TILECHARACTER[tile[r][c]] );
		}
		wprintw(w , "\n");
	}
	mvwprintw(w , player.row, player.col , "%s" , CHAR_PLAYER );
}


void
Board::wprint_position(WINDOW * w) {
	wprintw(w , "[%d ; %d]" , player.row , player.col);
}

bool
Board::is_inside( int const r , int const c ) {
	if( (r < 0) ) {
		return false;
	}
	if( (c < 0) ) {
		return false;
	}
	if( (r >= DUNGEON_SIZE_ROWS) ) {
		return false;
	}
	if( (c >= DUNGEON_SIZE_COLS) ) {
		return false;
	}
	return true;
}

bool
Board::is_walkable( int const r , int const c ) {
	if( !(is_inside(r,c)) ) {
		return false;
	}
	return (tile[r][c] == tiletype_walkable);
}


void
Board::move_player( WINDOW * w , int const r , int const c ) {
	wprint_tile( w , player.row , player.col );
	if( is_walkable(r , c) ) {
		player.row = r;
		player.col = c;
	}
	wprint_player(w);
	move(0,0);
}


void
Board::move_player_by( WINDOW * w , int const delta_r , int const delta_c ) {
	move_player( w
			, player.row + delta_r
			, player.col + delta_c
			);
}



int main(int argc, char * argv[])
{
	/* ncurses_init {   */
	/* boilerplate from http://tldp.org/HOWTO/NCURSES-Programming-HOWTO */
	int ch;

	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

	WINDOW * win_dungeon = newwin( DUNGEON_SIZE_ROWS+2 , DUNGEON_SIZE_COLS+2 , 2 , 2 );
	WINDOW * win_text  = newwin( 12 , 60 , 4 + DUNGEON_SIZE_ROWS , 4 );
	printw("hemlo\n");
	refresh();
	wprintw(win_text , "q quit\nwasd/hjkl move\n" );
	wrefresh(win_text);
	/* } ncurses_init */

	Board b;
	for( size_t r = 0 ; r < 3; ++r ) {
		for( size_t c = 0 ; c < 4; ++c ) {
			b.tile[r][c] = tiletype_walkable;
		}
	}
	b.tile[0][0] = tiletype_walkable;
	b.tile[2][4] = tiletype_walkable;
	b.tile[2][6] = tiletype_solid;
	b.tile[1][2] = tiletype_solid;
	b.wprint_board(win_dungeon);
	move(0,0);
	wrefresh(win_dungeon);

	
	while( (ch = getch()) ) {
		int move_r = 0;
		int move_c = 0;
		switch(ch) {
			case KEY_UP:
			case 'k':
			case 'w':
				--(move_r);
				break;
			case KEY_DOWN:
			case 'j':
			case 's':
				++(move_r);
				break;
			case KEY_LEFT:
			case 'a':
			case 'h':
				--(move_c);
				break;
			case KEY_RIGHT:
			case 'd':
			case 'l':
				++(move_c);
				break;
			case 'q':
				goto jump_end;
		}
		b.move_player_by( win_dungeon , move_r , move_c );
		wrefresh(win_dungeon);
	}


jump_end:

	endwin();
}
