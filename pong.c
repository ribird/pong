#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define DELAY 60000
#define BRIGHT_WHITE -1
#define WINNING_SCORE 10
#define ESC 27

typedef struct Paddle {

  int width;
  int length;
  int x_coord;
  int y_coord;
  int score;

} Paddle;

typedef struct Ball {

  int width;
  int length;
  int x_coord;
  int y_coord;
  int vertical_direction;
  int horizontal_direction;
  
} Ball;

static Ball ball;
static Paddle leftPaddle;
static Paddle rightPaddle;

void initialize_color_pairs();
void start_game();
void handle_resize(int ch);
void adjust_speed();
void reset();

void move_leftPaddle(int ch);
void move_leftPaddle_down();
void move_leftPaddle_up();
int leftPaddle_hit_top();
int leftPaddle_hit_bottom();

void move_rightPaddle();
void move_rightPaddle_down();
void move_rightPaddle_up();
int rightPaddle_hit_top();
int rightPaddle_hit_bottom();

void move_ball();
void move_ball_left();
void move_ball_right();
void move_ball_up();
void move_ball_down();
int ball_hit_leftPaddle();
int ball_hit_rightPaddle();
int ball_hit_bottom();
int ball_hit_top();
int ball_hit_above_leftPaddle();
int ball_hit_below_leftPaddle();
int ball_hit_above_rightPaddle();
int ball_hit_below_rightPaddle();

void create_objects();
void create_leftPaddle();
void create_rightPaddle();
void create_ball();

void render_game();
void render_background();
void render_new_ball(int y, int x);
void render_new_leftPaddle(int y, int x);
void render_new_rightPaddle(int y, int x);
void render_score();

void cover_old_ball();
void cover_old_leftPaddle();
void cover_old_rightPaddle();

int player_scored();
int computer_scored();
int winner();
int opening_screen();


int main(int argc, char *argv[])
{
  initscr(); // initialize the window
  noecho(); // Don't echo any key strokes
  raw(); // Don't wait to recieve key stokes
  curs_set(FALSE); // Don't display a cursor
  keypad(stdscr, TRUE); // Keypad enabled
  initialize_color_pairs(); 

  srand(time(NULL)); 

  if(opening_screen() != ESC){
    reset();
    start_game();
  }

  endwin(); // Restore normal terminal
  
  return 0;
}

void initialize_color_pairs()
{
  start_color();
  use_default_colors();
  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, BRIGHT_WHITE);
  init_pair(3, COLOR_GREEN, COLOR_GREEN);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);
}

void reset()
{
  create_objects();
  render_game();
}

void start_game()
{
  int ch = 0;
  
  while(ch != ESC){
      
      nodelay(stdscr, TRUE);

      ch = getch();
       
      handle_resize(ch);
     
      move_leftPaddle(ch);

      move_rightPaddle();

      move_ball();

      if(player_scored()){
	leftPaddle.score++;
	create_ball();
	render_new_ball(0, 0);
	render_score();
	ch = winner();
      }

      if(computer_scored()){
	rightPaddle.score++;
	create_ball();
	render_new_ball(0, 0);
	render_score();
	ch = winner();
      }
      
      adjust_speed();
      
      refresh();
    }
}

void handle_resize(int ch)
{
  if(ch == KEY_RESIZE){
    create_rightPaddle();
    leftPaddle.score = 0;
    render_game();
  }
}

void adjust_speed()
{
  usleep(DELAY - 1500*(leftPaddle.score + rightPaddle.score));
}

void move_leftPaddle(int ch)
{ 
  if(ch == KEY_UP){
         
    if(!leftPaddle_hit_top() && !ball_hit_above_leftPaddle())
      move_leftPaddle_up();

  }
  else if(ch == KEY_DOWN){
    
    if(!leftPaddle_hit_bottom() && !ball_hit_below_leftPaddle())
      move_leftPaddle_down();
      
  } 
}

void move_leftPaddle_up()
{
  render_new_leftPaddle(-1, 0); 
}

void move_leftPaddle_down()
{
  render_new_leftPaddle(1, 0);
}

int leftPaddle_hit_top()
{
  if(leftPaddle.y_coord == 0)
    return TRUE;
  
  return FALSE;
}

int leftPaddle_hit_bottom()
{
  if((leftPaddle.y_coord + leftPaddle.width - 1) == (LINES - 1))
    return TRUE;

  return FALSE;
}

void move_rightPaddle()
{
  int accuracy = rand() % 100;

  if(accuracy < 95 && ball.vertical_direction == RIGHT){
    
    if(ball.y_coord < rightPaddle.y_coord - 1  && !rightPaddle_hit_top() && !ball_hit_above_rightPaddle())
      move_rightPaddle_up();
    
    if(ball.y_coord > (rightPaddle.y_coord + rightPaddle.length - 1) && !rightPaddle_hit_bottom() && !ball_hit_below_rightPaddle())
      move_rightPaddle_down();
  
  }
}

void move_rightPaddle_up()
{
  render_new_rightPaddle(-1, 0); 
}

void move_rightPaddle_down()
{
  render_new_rightPaddle(1, 0);
}

int rightPaddle_hit_top()
{
  if(rightPaddle.y_coord == 0)
    return TRUE;
    
  return FALSE;
}

int rightPaddle_hit_bottom()
{
  if((rightPaddle.y_coord + rightPaddle.width - 1) == (LINES - 1))
    return TRUE;

  return FALSE;
}

void move_ball()
{
  if(ball.vertical_direction == LEFT){
	  
    if(!ball_hit_leftPaddle()){
      move_ball_left();
    }
    else{
      ball.vertical_direction = RIGHT;
    }
    
  }
  else if(ball.vertical_direction == RIGHT){

    if(!ball_hit_rightPaddle()){
      move_ball_right();
    }
    else{
      ball.vertical_direction = LEFT;
    }
    
  } 

  if(ball.horizontal_direction == UP){
      
    if(!ball_hit_top() && !ball_hit_below_leftPaddle() && !ball_hit_below_rightPaddle()){
      move_ball_up();
    }
    else{
      ball.horizontal_direction = DOWN;
    }
      
  }
  else if(ball.horizontal_direction == DOWN){
      
    if(!ball_hit_bottom() && !ball_hit_above_leftPaddle() && !ball_hit_above_rightPaddle()){     
      move_ball_down();
    }
    else{
      ball.horizontal_direction = UP;
    }
    
  }
}

void move_ball_up()
{
  render_new_ball(-1, 0);
}

void move_ball_down()
{
  render_new_ball(1, 0);
}

void move_ball_left()
{
  render_new_ball(0, -1);
}

void move_ball_right()
{
  render_new_ball(0, 1);
}

int ball_hit_top()
{
  if(ball.y_coord == 0)
    return TRUE;
  
  return FALSE;
}

int ball_hit_bottom()
{
  if(ball.y_coord == (LINES - 1))
    return TRUE;
  
  return FALSE;
}

int ball_hit_above_leftPaddle()
{
  int ballAboveLeftPaddle = ball.y_coord == leftPaddle.y_coord - 1;
  int ballBehindLeftPaddle = ball.x_coord < (leftPaddle.x_coord + leftPaddle.length);

  if(ballBehindLeftPaddle && ballAboveLeftPaddle)
    return TRUE;

  return FALSE;    
}

int ball_hit_below_leftPaddle()
{
  int ballBelowLeftPaddle = ball.y_coord == (leftPaddle.y_coord + leftPaddle.width);
  int ballBehindLeftPaddle = ball.x_coord < (leftPaddle.x_coord + leftPaddle.length);

  if(ballBehindLeftPaddle && ballBelowLeftPaddle)
    return TRUE;

  return FALSE;
}

int ball_hit_leftPaddle()
{
  int ballAboveLeftPaddle = ball.y_coord < leftPaddle.y_coord;
  int ballBelowLeftPaddle = ball.y_coord > (leftPaddle.y_coord + leftPaddle.width - 1);
  int ballInFrontOfLeftPaddle = ball.x_coord == (leftPaddle.x_coord + leftPaddle.length);

  if(ballInFrontOfLeftPaddle && (!ballAboveLeftPaddle && !ballBelowLeftPaddle))
    return TRUE;
  
  return FALSE;
}

int ball_hit_above_rightPaddle()
{
  int ballAboveRightPaddle = ball.y_coord == rightPaddle.y_coord - 1;
  int ballBehindRightPaddle = ball.x_coord > (rightPaddle.x_coord - rightPaddle.length - 1);

  if(ballBehindRightPaddle && ballAboveRightPaddle)
    return TRUE;

  return FALSE;    
}

int ball_hit_below_rightPaddle()
{
  int ballBelowRightPaddle = ball.y_coord == (rightPaddle.y_coord + leftPaddle.width);
  int ballBehindRightPaddle = ball.x_coord > (rightPaddle.x_coord - rightPaddle.length - 1);

  if(ballBehindRightPaddle && ballBelowRightPaddle)
    return TRUE;

  return FALSE;
}

int ball_hit_rightPaddle()
{
  int ballAboveRightPaddle = ball.y_coord < rightPaddle.y_coord;
  int ballBelowRightPaddle = ball.y_coord > (rightPaddle.y_coord + rightPaddle.width - 1);
  int ballInFrontOfRightPaddle = ball.x_coord == (rightPaddle.x_coord - rightPaddle.length - 1); 

  if(ballInFrontOfRightPaddle && (!ballAboveRightPaddle && !ballBelowRightPaddle))
    return TRUE;
  
  return FALSE;
}

void create_objects()
{
  create_leftPaddle();
  create_rightPaddle();
  create_ball();
}

void create_leftPaddle()
{
  leftPaddle.width = 5;
  leftPaddle.length = 2;
  leftPaddle.x_coord = 1;
  leftPaddle.y_coord = (LINES - leftPaddle.width) / 2;
  leftPaddle.score = 0;
}

void create_rightPaddle()
{
  rightPaddle.width = 5;
  rightPaddle.length = 2;
  rightPaddle.x_coord = COLS - 1;
  rightPaddle.y_coord = (LINES - rightPaddle.width) / 2;
  rightPaddle.score = 9;
}

void create_ball()
{
  ball.width = 1;
  ball.length = 1;
  ball.x_coord = COLS / 2;
  ball.y_coord = rand() % LINES;
  ball.vertical_direction = rand() % 2;
  ball.horizontal_direction = rand() % 2 + 2;
}


void render_game()
{
  render_background();
  render_new_ball(0, 0);
  render_new_leftPaddle(0, 0);
  render_new_rightPaddle(0, 0);
  render_score();
}

void render_background()
{
  int i;

  bkgd(COLOR_PAIR(1));
  
  for(i = 0; i < LINES; i++){
	attron(COLOR_PAIR(2));
	mvaddch(i, COLS / 2, ' ');
	attroff(COLOR_PAIR(2));
  }
}

void render_new_ball(int y, int x)
{
  cover_old_ball();
  
  ball.y_coord = ball.y_coord + y;
  ball.x_coord = ball.x_coord + x;
  
  attron(COLOR_PAIR(3));
  mvaddch(ball.y_coord, ball.x_coord, ' ');
  attroff(COLOR_PAIR(3));
}

void render_new_leftPaddle(int y, int x)
{
  cover_old_leftPaddle();
  
  leftPaddle.y_coord = leftPaddle.y_coord + y;
  leftPaddle.x_coord = leftPaddle.x_coord + x;
  
  int i, j;

  for(i = leftPaddle.y_coord; i < (leftPaddle.y_coord + leftPaddle.width); i++){
    for(j = leftPaddle.x_coord; j < (leftPaddle.x_coord + leftPaddle.length); j++){

      attron(COLOR_PAIR(2));
      mvaddch(i, j, ' ');
      attroff(COLOR_PAIR(2));
      
    }
  }
}

void render_new_rightPaddle(int y, int x)
{
  cover_old_rightPaddle();

  rightPaddle.y_coord = rightPaddle.y_coord + y;
  rightPaddle.x_coord = rightPaddle.x_coord + x;
  
  int i, j;

  for(i = rightPaddle.y_coord; i < (rightPaddle.y_coord + rightPaddle.width); i++){
    for(j =  (rightPaddle.x_coord - rightPaddle.length); j < rightPaddle.x_coord; j++){

      attron(COLOR_PAIR(2));
      mvaddch(i, j, ' ');
      attroff(COLOR_PAIR(2));
      
    }
  }
}

void render_score()
{
  attron(COLOR_PAIR(4));
  attron(A_BOLD);
  mvprintw(3, COLS / 4 , "%d", leftPaddle.score);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(4));

  attron(COLOR_PAIR(4));
  attron(A_BOLD);
  mvprintw(3, (COLS - (COLS / 4)), "%d", rightPaddle.score);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(4));
}

void cover_old_ball(int y, int x)
{
  if(ball.x_coord == COLS / 2){
    attron(COLOR_PAIR(2));
    mvaddch(ball.y_coord, ball.x_coord, ' ');
    attroff(COLOR_PAIR(2));
  }
  else if(ball.y_coord == 3 && ( ball.x_coord == (COLS / 4) || ball.x_coord == (COLS - (COLS / 4)))){
    render_score();
  }
  else{
    attron(COLOR_PAIR(1));
    mvaddch(ball.y_coord, ball.x_coord, ' ');
    attroff(COLOR_PAIR(1));
  } 
}

void cover_old_leftPaddle()
{
  int i, j;

  for(i = leftPaddle.y_coord; i < (leftPaddle.y_coord + leftPaddle.width); i++){
    for(j = leftPaddle.x_coord; j < (leftPaddle.x_coord + leftPaddle.length); j++){

      attron(COLOR_PAIR(1));
      mvaddch(i, j, ' ');
      attroff(COLOR_PAIR(1));
      
    }
  }
}

void cover_old_rightPaddle()
{
  int i, j;

  for(i = rightPaddle.y_coord; i < (rightPaddle.y_coord + rightPaddle.width); i++){
    for(j =  (rightPaddle.x_coord - rightPaddle.length); j < rightPaddle.x_coord; j++){

      attron(COLOR_PAIR(1));
      mvaddch(i, j, ' ');
      attroff(COLOR_PAIR(1));
      
    }
  }
}

int player_scored()
{
  if(ball.x_coord > (COLS - 1))
    return TRUE;
  
  return FALSE;
}

int computer_scored()
{
  if(ball.x_coord < 0)
    return TRUE;

  return FALSE;
}

int winner()
{
  int ch = -1;
  
  if(leftPaddle.score == WINNING_SCORE){

    nodelay(stdscr, FALSE);
    
    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, COLS / 4 , "%s", "WINNER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, (COLS - (COLS / 4)), "%s", "LOSER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    leftPaddle.score = 0;
    rightPaddle.score = 0;
 
    ch = getch();
    
    reset();
  }
  
  if(rightPaddle.score == WINNING_SCORE){

    nodelay(stdscr, FALSE);
      
    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, COLS / 4 , "%s", "LOSER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, (COLS - (COLS / 4)), "%s", "WINNER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    leftPaddle.score = 0;
    rightPaddle.score = 0;
    
    ch = getch();
    
    reset();
  }
  
  return ch;
}

int opening_screen()
{
  int ch;
  
  bkgd(COLOR_PAIR(1));

  attron(A_BOLD);
  attron(COLOR_PAIR(4));
  mvprintw((LINES/10) + 1, COLS / 2 - 15, "%s" , "@@@@ @" );
  mvprintw((LINES/10) + 2, COLS / 2 - 15, "%s" , "@@    @@" );
  mvprintw((LINES/10) + 3, COLS / 2 - 15, "%s" , "@@    @@" );
  mvprintw((LINES/10) + 4, COLS / 2 - 15, "%s" , "@@  @@   @@ @  @ @@@@   @@ @@" );
  mvprintw((LINES/10) + 5, COLS / 2 - 15, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((LINES/10) + 6, COLS / 2 - 15, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((LINES/10) + 7, COLS / 2 - 15, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((LINES/10) + 8, COLS / 2 - 15, "%s" , "@@      @@ @@  @@   @@  @@ @@@"  );
  mvprintw((LINES/10) + 9, COLS / 2 - 15, "%s" , "                            @@"  );
  mvprintw((LINES/10) + 10, COLS / 2 - 15, "%s" , "                         @@ @"  );

  mvprintw((LINES/10) + 15, COLS / 2 - 15, "%s" ,  "   PRESS ANY KEY TO START"  );
  mvprintw((LINES/10) + 16, COLS / 2 - 15, "%s" ,  "      PReSs eSc To EXiT"  );

  mvprintw((LINES/10) + 20, COLS / 2 - 15, "%s" , "   Press Up Arrow to Move Up"  );
  mvprintw((LINES/10) + 21, COLS / 2 - 15, "%s" , " Press Down Arrow to Move Down"  );
  
  attroff(COLOR_PAIR(4));
  attroff(A_BOLD);
   
  ch = getch();
  
  erase();

  return ch;
}
