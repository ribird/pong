#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>


#define WIDTH getmaxy(stdscr)
#define LENGTH getmaxx(stdscr)
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define DELAY 60000
#define BRIGHT_WHITE -1
#define WINNING_SCORE 10


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
  
} Ball;

static Ball ball;
static Paddle leftPaddle;
static Paddle rightPaddle;
static int player1score = 0;
static int player2score = 0;

void initialize_color_pairs();
void start_game();
void reset();

void move_leftPaddle(int ch);
void move_leftPaddle_down();
void move_leftPaddle_up();
int leftPaddle_hit_top();
int leftPaddle_hit_bottom();


void move_rightPaddle(int leftOrRight);
void move_rightPaddle_down();
void move_rightPaddle_up();
int rightPaddle_hit_top();
int rightPaddle_hit_bottom();

int move_ball_leftOrRight(int leftOrRight);
int move_ball_upOrDown(int upOrDown);
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

void player_1_scored();
void player_2_scored();
void winner();
void opening_screen();

int main(int argc, char *argv[])
{
  initscr(); // initialize the window
  noecho(); // Don't echo any key strokes
  raw(); // Don't wait to recieve key stokes
  curs_set(FALSE); // Don't display a cursor
  keypad(stdscr, TRUE); // Keypad enabled
  initialize_color_pairs(); 

  srand(time(NULL));
  nodelay(stdscr, FALSE);
  reset();

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


void start_game()
{
  int leftOrRight = rand() % 2;
  int upOrDown = (rand() % 2) + 2;
  int ch;
  
  while(ball.x_coord > 0 && ball.x_coord < (LENGTH - 1) && ch != 27){

    nodelay(stdscr, TRUE);
    
    ch = getch();
     
    move_leftPaddle(ch);

    move_rightPaddle(leftOrRight);

    leftOrRight = move_ball_leftOrRight(leftOrRight);

    upOrDown = move_ball_upOrDown(upOrDown);   

    player_1_scored();
    
    player_2_scored();

    usleep(DELAY);
      
    refresh();	
  }

  if(ch != 27){
    winner();
    reset();
  }

}

void reset()
{
  nodelay(stdscr, FALSE);
  create_objects();
  render_game();
  refresh();
  start_game();
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
  if((leftPaddle.y_coord + leftPaddle.width - 1) == (WIDTH - 1))
    return TRUE;

  return FALSE;
}

void move_rightPaddle(int leftOrRight)
{
  int accuracy = rand() % 100;

  if(accuracy < 95 && leftOrRight == RIGHT){
    
    if(ball.y_coord < rightPaddle.y_coord && !rightPaddle_hit_top() && !ball_hit_above_rightPaddle())
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
  if((rightPaddle.y_coord + rightPaddle.width - 1) == (WIDTH - 1))
    return TRUE;

  return FALSE;
}

int move_ball_leftOrRight(int leftOrRight)
{
  if(leftOrRight == LEFT){
	  
    if(!ball_hit_leftPaddle()){
      move_ball_left();
    }
    else{
      return RIGHT;
    }
    
  }
  else if(leftOrRight == RIGHT){

    if(!ball_hit_rightPaddle()){
      move_ball_right();
    }
    else{
      return LEFT;
    }
    
  } 
  return leftOrRight;
}

int move_ball_upOrDown(int upOrDown)
{ 
  if(upOrDown == UP){
      
    if(!ball_hit_top() && !ball_hit_below_leftPaddle() && !ball_hit_below_rightPaddle()){
      move_ball_up();
    }
    else{
      return DOWN;
    }
      
  }
  else if(upOrDown == DOWN){
      
    if(!ball_hit_bottom() && !ball_hit_above_leftPaddle() && !ball_hit_above_rightPaddle()){     
      move_ball_down();
    }
    else{
      return UP;
    }
    
  }
  return upOrDown;
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
  if(ball.y_coord == (WIDTH - 1))
    return TRUE;
  
  return FALSE;
}

int ball_hit_above_leftPaddle()
{
  int ballAboveLeftPaddle = ball.y_coord == leftPaddle.y_coord-1;
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
  int ballAboveRightPaddle = ball.y_coord == rightPaddle.y_coord-1;
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
  leftPaddle.y_coord = (WIDTH - leftPaddle.width) / 2;
}

void create_rightPaddle()
{
  rightPaddle.width = 5;
  rightPaddle.length = 2;
  rightPaddle.x_coord = LENGTH - 1;
  rightPaddle.y_coord = (WIDTH - rightPaddle.width) / 2;
}

void create_ball()
{
  ball.width = 1;
  ball.length = 1;
  ball.x_coord = LENGTH / 2;
  ball.y_coord = rand() % WIDTH;
}


void render_game()
{
  if(player1score + player2score == 0){
    render_background();
    opening_screen();
    getch();
    erase();
  }
  
  render_background();
  render_new_ball(0, 0);
  render_new_leftPaddle(0, 0);
  render_new_rightPaddle(0, 0);
  render_score();
}

void render_background()
{ 
  int i, j;
  
  for(i = 0; i < WIDTH; i++){

    for(j = 0; j < LENGTH; j++){

      if(j == LENGTH / 2){
	attron(COLOR_PAIR(2));
	mvaddch(i, j, ' ');
	attroff(COLOR_PAIR(2));
      }
      else{
	attron(COLOR_PAIR(1));
	mvaddch(i, j, ' ');
	attroff(COLOR_PAIR(1));
      }
      
    }
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
  refresh();
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
  refresh();
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
  mvprintw(3, LENGTH / 4 , "%d", player1score);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(4));

  attron(COLOR_PAIR(4));
  attron(A_BOLD);
  mvprintw(3, (LENGTH - (LENGTH / 4)), "%d", player2score);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(4));
}

void cover_old_ball(int y, int x)
{
  if(ball.x_coord == LENGTH / 2){
    attron(COLOR_PAIR(2));
    mvaddch(ball.y_coord, ball.x_coord, ' ');
    attroff(COLOR_PAIR(2));
  }
  else if(ball.y_coord == 3 && ( ball.x_coord == (LENGTH / 4) || ball.x_coord == (LENGTH - (LENGTH / 4)))){
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

void player_1_scored()
{
  if(ball.x_coord == (LENGTH - 1))
    player1score++;
}

void player_2_scored()
{
  if(ball.x_coord == 0)
    player2score++;
}

void winner()
{
  if(player1score == WINNING_SCORE){
      nodelay(stdscr, FALSE);
    
    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, LENGTH / 4 , "%s", "WINNER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, (LENGTH - (LENGTH / 4)), "%s", "LOSER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    player1score = 0;
    player2score = 0;
  }
  
  if(player2score == WINNING_SCORE){
      nodelay(stdscr, FALSE);
      
    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, LENGTH / 4 , "%s", "LOSER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(4));
    attron(A_BOLD);
    mvprintw(3, (LENGTH - (LENGTH / 4)), "%s", "WINNER!");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(4));

    player1score = 0;
    player2score = 0;
  }

  getch();
}

void opening_screen()
{
  attron(COLOR_PAIR(4));
  attron(A_BOLD);
  mvprintw((WIDTH/10) + 1, LENGTH / 8, "%s" , "@@@@ @" );
  mvprintw((WIDTH/10) + 2, LENGTH / 8, "%s" , "@@    @@" );
  mvprintw((WIDTH/10) + 3, LENGTH / 8, "%s" , "@@    @@" );
  mvprintw((WIDTH/10) + 4, LENGTH / 8, "%s" , "@@  @@   @@ @  @ @@@@   @@ @@" );
  mvprintw((WIDTH/10) + 5, LENGTH / 8, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((WIDTH/10) + 6, LENGTH / 8, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((WIDTH/10) + 7, LENGTH / 8, "%s" , "@@     @@   @@ @@   @@ @@   @@"  );
  mvprintw((WIDTH/10) + 8, LENGTH / 8, "%s" , "@@      @@ @@  @@   @@  @@ @@@"  );
  mvprintw((WIDTH/10) + 9, LENGTH / 8, "%s" , "                            @@"  );
  mvprintw((WIDTH/10) + 10, LENGTH / 8, "%s" , "                         @@ @"  );

  mvprintw((WIDTH/10) + 15, LENGTH / 8, "%s" ,  "   PRESS ANY KEY TO START"  );
  mvprintw((WIDTH/10) + 16, LENGTH / 8, "%s" ,  "      PReSs eSc To EXiT"  );

  mvprintw((WIDTH/10) + 20, LENGTH / 8, "%s" , "   Press Up Arrow to Move Up"  );
  mvprintw((WIDTH/10) + 21, LENGTH / 8, "%s" , " Press Down Arrow to Move Down"  );
  
  attroff(A_BOLD);
  attroff(COLOR_PAIR(4));
}
