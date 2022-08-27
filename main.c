#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define GRID_WIDTH 20
#define GRID_HEIGHT 20
#define WAIT_TIME 2
#define X_OFFSET (COLS / 2) - GRID_WIDTH
#define Y_OFFSET (LINES / 2) - (GRID_HEIGHT / 2)
/* Colors: */
#define CBODY 1
#define CHEAD 2
#define CFRUIT 3
#define CMAP 5

/* STRUCTS */
typedef struct {
  int x;
  int y;
} pos_t;

/* GLOBAL VARIABLES */
unsigned char map[GRID_HEIGHT][GRID_WIDTH];
pos_t         posh; // position of the head
pos_t         post; // position of the tail
pos_t         posf; // fruit position
int           length;
char          dir;

/* FUNCTIONS */

static inline int isTail(int x, int y) { return x == post.x && y == post.y; }

static inline int isHead(int x, int y) { return x == posh.x && y == posh.y; }

static inline int isFruit(int x, int y) { return x == posf.x && y == posf.y; }

void initMap() {
  int i, j;
  for (i = 0; i < GRID_HEIGHT; ++i) {
    for (j = 0; j < GRID_WIDTH; ++j) {
      map[i][j] = 0;
    }
  }
  map[posh.y][posh.x] = 2; // snake head
  map[post.y][post.x] = 3; // snake tail
}

/* display `g_map` to stdout */
void displayMap() {
  int i, j;

  for (i = 0; i < GRID_HEIGHT; ++i) {
    for (j = 0; j < GRID_WIDTH; ++j) {
      if (isFruit(j, i)) { // fruit color
        attron(COLOR_PAIR(CFRUIT));
        mvaddstr(i + Y_OFFSET, 2 * j + X_OFFSET, "  ");
        attroff(COLOR_PAIR(CFRUIT));
      } else if (isHead(j, i)) { // head color
        attron(COLOR_PAIR(CHEAD));
        mvaddstr(i + Y_OFFSET, 2 * j + X_OFFSET, "  ");
        attroff(COLOR_PAIR(CHEAD));
      } else if (map[i][j] > 0) { // body color
        attron(COLOR_PAIR(CBODY));
        mvaddstr(i + Y_OFFSET, 2 * j + X_OFFSET, "  ");
        attroff(COLOR_PAIR(CBODY));
      } else { // map color
        attron(COLOR_PAIR(CMAP));
        mvaddstr(i + Y_OFFSET, 2 * j + X_OFFSET, "  ");
        attroff(COLOR_PAIR(CMAP));
      }
    }
  }
}

/* display the current score on the right of the map */
void displayScore() {
  char formatedScore[20];
  sprintf(formatedScore, "score: %d", length - 2);
  mvaddstr(Y_OFFSET - 1, X_OFFSET, formatedScore);
}

/* moving function */

int moveUp() {
  int r = (posh.y > 0 && map[posh.y - 1][posh.x] == 0);
  if (r)
    posh.y--;
  return r;
}

int moveDown() {
  int r = (posh.y + 1 < GRID_HEIGHT && map[posh.y + 1][posh.x] == 0);
  if (r)
    posh.y++;
  return r;
}

int moveLeft() {
  int r = (posh.x > 0 && map[posh.y][posh.x - 1] == 0);
  if (r)
    posh.x--;
  return r;
}

int moveRight() {
  int r = (posh.x + 1 < GRID_WIDTH && map[posh.y][posh.x + 1] == 0);
  if (r)
    posh.x++;
  return r;
}

/* update tail position (and the tail in the map) after a mouvement */
void moveTail() {
  unsigned char tailNum = map[post.y][post.x];
  map[post.y][post.x] = 0;
  // update the tail
  if (post.x < GRID_WIDTH && map[post.y][post.x + 1] == tailNum - 1) {
    post.x++;
  } else if (post.x > 0 && map[post.y][post.x - 1] == tailNum - 1) {
    post.x--;
  } else if (post.y < GRID_HEIGHT && map[post.y + 1][post.x] == tailNum - 1) {
    post.y++;
  } else if (post.y > 0 && map[post.y - 1][post.x] == tailNum - 1) {
    post.y--;
  }
}

/* Update all the snake after a movement */
void updateSnake(int x, int y) {
  map[y][x]++;
  if (!isTail(x, y)) {
    if (x < GRID_WIDTH && map[y][x + 1] == map[y][x])
      updateSnake(x + 1, y);
    else if (x > 0 && map[y][x - 1] == map[y][x])
      updateSnake(x - 1, y);
    else if (y < GRID_HEIGHT && map[y + 1][x] == map[y][x])
      updateSnake(x, y + 1);
    else if (y > 0 && map[y - 1][x] == map[y][x])
      updateSnake(x, y - 1);
  }
}

/* update the last movement
 *
 * The snake is a suite of numbers, starting from 1 (the head) and continuing
 * up to the tail. If it moves, we have to remove the old tail and add 1 to all
 * the parts of its body, from the new head to the new tail position.
 */
void updateMovement() {
  map[posh.y][posh.x]++;
  updateSnake(posh.x, posh.y);
  if (!isFruit(posh.x, posh.y)) {
    moveTail();
  } else {
    length++;
    posf.x = rand() % GRID_WIDTH;
    posf.y = rand() % GRID_HEIGHT;
  }
}

int eventHandler(char act) {
  int output = 1;

  switch (act) {
  case 'k':
    if (moveUp()) {
      updateMovement();
      dir = 'k';
    } else if (dir != 'j') {
      output = 0;
    }
    break;
  case 'j':
    if (moveDown()) {
      updateMovement();
      dir = 'j';
    } else if (dir != 'k') {
      output = 0;
    }
    break;
  case 'l':
    if (moveRight()) {
      updateMovement();
      dir = 'l';
    } else if (dir != 'h') {
      output = 0;
    }
    break;
  case 'h':
    if (moveLeft()) {
      updateMovement();
      dir = 'h';
    } else if (dir != 'l') {
      output = 0;
    }
    break;
  case 'q':
    output = 0;
    break;
  }
  return output;
}

void run() {
  int  cont = 1;
  char resp;

  initscr();
  cbreak(); // turn on cbreak mode
  noecho(); // turn on noecho mode

  /* if colors are not supported by the terminal or tty */
  if (has_colors() == FALSE) {
    fprintf(stderr, "error: colors not supported!\n");
    endwin();
    exit(EXIT_FAILURE);
  }

  start_color();
  init_pair(CMAP, COLOR_BLACK, COLOR_GREEN);
  init_pair(CBODY, COLOR_BLACK, COLOR_CYAN);
  init_pair(CHEAD, COLOR_BLACK, COLOR_BLUE);
  init_pair(CFRUIT, COLOR_BLACK, COLOR_RED);
  halfdelay(WAIT_TIME);
  while (cont) {
    clear();
    displayMap();
    displayScore();
    mvaddstr(LINES-1, COLS-1, NULL); // get rid of the white cursor
    refresh();
    resp = getch();
    if (resp == ERR)
      resp = dir;
    cont = eventHandler(resp);
  }
  endwin();

  // TODO: update a score file
  printf("IMPORTANT: Y_OFFSET: %d\n", Y_OFFSET);
  printf("Score:\n");
  printf("- fruit: %d\n", length - 2);
  printf("- length: %d\n", length);
}

int main(void) {
  // init randomizer
  srand(time(NULL));
  // setting up global varibles
  posh.x = 0;
  posh.y = 10;
  post.x = 0;
  post.y = 9;
  length = 2;
  dir = 'j';
  posf.x = rand() % GRID_WIDTH;
  posf.y = rand() % GRID_HEIGHT;
  initMap();
  run();
  return 0;
}
