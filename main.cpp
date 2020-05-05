#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <vector>
#include <assert.h>

// TODO
//  ✓1. Board::move_by(int delta_row, delta_col)
//  ✓2. move with keyboard
//  ✓3. change tiletype in-game
//   4. save and load the state of the board
//   5. not compile-time-only setting of boardsize
//   6. pick and show path of movement (somehow)

// FIX: the cursor in board doesn't display correctly.

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


int const DEFAULT_DUNGEON_SIZE_ROWS = 0x8;
static_assert(DEFAULT_DUNGEON_SIZE_ROWS > 0);
int const DEFAULT_DUNGEON_SIZE_COLS = 0xc;
static_assert(DEFAULT_DUNGEON_SIZE_COLS > 0);


struct Position {
	int row = 0;
	int col = 0;
};

struct Entity {
	const char * display;
	const char * description;
};


struct Board {
	std::vector< std::vector<enum tiletype> > tile;
	//std::vector<Entity> vector_entities;
	int dungeon_size_rows;
	int dungeon_size_cols;
	Position player;
	Position position_cursor;
	void wprint_board( WINDOW * w );
	void wprint_position( WINDOW * w );
	void wprint_position_cursor( WINDOW * w );
	bool is_walkable( int const r ,int const c );
	bool is_inside( int const r , int const c );
	void move_player( WINDOW * w , int const r , int const c );
	void move_cursor( WINDOW * w , int const r , int const c );
	void move_player_by( WINDOW * w , int const delta_r , int const delta_c );
	void move_cursor_by( WINDOW * w , int const delta_r , int const delta_c );
	void move_global_cursor( WINDOW * w ) ;
	void wprint_tile( WINDOW * w , int const r , int const c );
	void wprint_player( WINDOW * w );

	void change_tiletype_at(
			WINDOW * w
			,int const r
			,int const c
			,enum tiletype tt );
	void change_tiletype_at_cursor(
			WINDOW * w
			,enum tiletype tt );
	void change_tiletype_relative_to_player(
			WINDOW * w
			,int const delta_r
			,int const delta_c
			,enum tiletype tt );
	Board( int const rows , int const cols );
	Board( ) : Board(
		 DEFAULT_DUNGEON_SIZE_ROWS
		, DEFAULT_DUNGEON_SIZE_COLS ) {}
};


Board::Board( int const rows , int const cols ){
	dungeon_size_rows = rows;
	dungeon_size_rows = cols;
	assert(rows > 1);
	assert(cols > 1);
	tile.resize(rows);
	for( auto &each_row : tile  ) {
		each_row.resize(cols);
		for( auto &each_cell : each_row ) {
			each_cell = tiletype_walkable;
		}
	}
	dungeon_size_rows = rows;
	dungeon_size_cols = cols;
}


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
	for( int r = 0; r < dungeon_size_rows; ++r ) {
		for( int c = 0; c < dungeon_size_cols; ++c ) {
			wprintw( w , "%s" , TILECHARACTER[tile[r][c]] );
		}
		wprintw(w , "\n");
	}
	mvwprintw(w , player.row, player.col , "%s" , CHAR_PLAYER );
}


void
Board::wprint_position(WINDOW * w) {
	wprintw(w , "[%d ; %d]\n" , player.row , player.col);
}
void
Board::wprint_position_cursor(WINDOW * w) {
	wprintw(w , "[%d ; %d]\n" , position_cursor.row , position_cursor.col);
}


bool
Board::is_inside( int const r , int const c ) {
	if( (r < 0) ) {
		return false;
	}
	if( (c < 0) ) {
		return false;
	}
	if( (r >= dungeon_size_rows) ) {
		return false;
	}
	if( (c >= dungeon_size_cols) ) {
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
Board::move_player(
		WINDOW * w
		, int const r
		, int const c )
{
	wprint_tile( w , player.row , player.col );
	if( is_walkable(r , c) ) {
		player.row = r;
		player.col = c;
	}
	wprint_player(w);
	move(0,0);
}
void
Board::move_cursor(
		WINDOW * w
		, int const r
		, int const c )
{
	if(is_inside(r,c)) {
		position_cursor.row = r;
		position_cursor.col = c;
	}
	wmove(w,r,c);
}


void
Board::move_player_by(
		 WINDOW * w
		,int const delta_r
		,int const delta_c )
{
	move_player( w
			, player.row + delta_r
			, player.col + delta_c
			);
}
void
Board::move_cursor_by(
		 WINDOW * w
		,int const delta_r
		,int const delta_c )
{
	move_cursor(
			 w
			,position_cursor.row + delta_r
			,position_cursor.col + delta_c
			);
}


void
Board::move_global_cursor( WINDOW * w ) {
	int wbeg_row;
	int wbeg_col;
	getbegyx(w ,wbeg_row , wbeg_col);
	move(
			wbeg_row + position_cursor.row
			,wbeg_col + position_cursor.col
			);
}


void
Board::change_tiletype_at(
		 WINDOW * w
		,int const r
		,int const c
		,enum tiletype tt)
{
	if( is_inside(r,c) ) {
		tile[r][c] = tt;
		mvwprintw( w , r , c , "%s" , TILECHARACTER[tile[r][c]]);
		move(0,0);
	}
}


void
Board::change_tiletype_at_cursor(
		 WINDOW * w
		,enum tiletype tt )
{
	assert( is_inside( position_cursor.row, position_cursor.col ));
	tile[position_cursor.row][position_cursor.col] = tt;
	wprintw( w , "%s" , TILECHARACTER[tt] );
	wmove( w , position_cursor.row , position_cursor.col ); // reset cursor back where it was
}


void
Board::change_tiletype_relative_to_player(
		 WINDOW * w
		,int const delta_r
		,int const delta_c
		,enum tiletype tt)
{
	change_tiletype_at(
			 w
			,player.row + delta_r
			,player.col + delta_c
			,tt );
}


int main()//int argc, char * argv[])
{
	/* ncurses_init {   */
	/* boilerplate from http://tldp.org/HOWTO/NCURSES-Programming-HOWTO */

	int const dungeon_size_rows = DEFAULT_DUNGEON_SIZE_ROWS;
	int const dungeon_size_cols = DEFAULT_DUNGEON_SIZE_COLS;
	int const screen_size_rows = dungeon_size_rows;
	int const screen_size_cols = dungeon_size_cols;

	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

	WINDOW * win_dungeon = newwin( screen_size_rows+2 , screen_size_cols+2 , 2 , 16 );
	WINDOW * win_text  = newwin( 12 , 60 , 4 + screen_size_rows , 12 );
	printw("hemlo\n");
	refresh();
	wprintw(win_text , "q quit\nwasd|hjkl|arrows move\nW  make tile walkable\nV make tile void\nS make tile solid\nT stop changing tiles\nC switch to moving cursor\nP switch to moving player\n" );
	wrefresh(win_text);
	/* } ncurses_init */

	Board b = Board();
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

	enum tiletype set_tiletype = tiletype_walkable;
	bool flag_change_tile = false;
	bool flag_move_cursor = false;
	bool flag_set_tiletype_at_cursor = false;
	int ch;
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
			case 'T':
				flag_change_tile = false;
				break;
			case 'W':
				set_tiletype = tiletype_walkable;
				flag_change_tile = true;
				break;
			case 'V':
				set_tiletype = tiletype_void;
				flag_change_tile = true;
				break;
			case 'S':
				set_tiletype = tiletype_solid;
				flag_change_tile = true;
				break;
			case 'C':
				flag_move_cursor = true;
				break;
			case 'P':
				flag_move_cursor = false;
				break;
			case 't':
				flag_set_tiletype_at_cursor = true;
				break;
			case 'q':
				goto jump_end;
		}
		if( flag_set_tiletype_at_cursor ) {
			b.change_tiletype_at_cursor( win_dungeon , set_tiletype );
			flag_set_tiletype_at_cursor = false;
		}
		if( (!flag_move_cursor) && flag_change_tile && (move_r || move_c) ) {
			b.change_tiletype_relative_to_player( win_dungeon , move_r , move_c , set_tiletype );
		}
		if( flag_move_cursor ) {
			b.move_cursor_by( win_dungeon , move_r , move_c );
			b.move_global_cursor( win_dungeon );
		} else {
			b.move_player_by( win_dungeon , move_r , move_c );
		}
		wrefresh(win_dungeon);
		werase(win_text);
		b.wprint_position(win_text);
		b.wprint_position_cursor(win_text);
		wrefresh(win_text);
	}


jump_end:

	endwin();
}
