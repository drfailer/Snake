#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#define WIDTH 20
#define HEIGHT 20
#define FRAME_RATE 0.5
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
unsigned char map[HEIGHT][WIDTH];
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
  for (i = 0; i < HEIGHT; ++i) {
    for (j = 0; j < WIDTH; ++j) {
      map[i][j] = 0;
    }
  }
  map[posh.y][posh.x] = 2; // snake head
  map[post.y][post.x] = 3; // snake tail
}

/* display `g_map` to stdout */
void displayMap() {
  int i, j;
  int x = 0, y = 0;

  for (i = 0; i < HEIGHT; ++i, y+=1) {
    for (j = 0; j < WIDTH; ++j, x+=1) {
      if (isFruit(j, i)) { // fruit color
        attron(COLOR_PAIR(CFRUIT));
        mvaddstr(i, 2*j, "  ");
        attroff(COLOR_PAIR(CFRUIT));
      } else if (isHead(j, i)) { // head color
        attron(COLOR_PAIR(CHEAD));
        mvaddstr(i, 2*j, "  ");
        attroff(COLOR_PAIR(CHEAD));
      } else if (map[i][j] > 0) { // body color
        attron(COLOR_PAIR(CBODY));
        mvaddstr(i, 2*j, "  ");
        attroff(COLOR_PAIR(CBODY));
      } else { // map color
        attron(COLOR_PAIR(CMAP));
        mvaddstr(i, 2*j, "  ");
        attroff(COLOR_PAIR(CMAP));
      }
    }
  }
}

/* moving function */

int moveUp() {
  int r = posh.y > 0 && map[posh.y - 1][posh.x] == 0;
  if (r)
    posh.y--;
  return r;
}

int moveDown() {
  int r = posh.y < HEIGHT && map[posh.y + 1][posh.x] == 0;
  if (r)
    posh.y++;
  return r;
}

int moveLeft() {
  int r = posh.x > 0 && map[posh.y][posh.x - 1] == 0;
  if (r)
    posh.x--;
  return r;
}

int moveRight() {
  int r = posh.x < WIDTH && map[posh.y][posh.x + 1] == 0;
  if (r)
    posh.x++;
  return r;
}

/* update tail position (and the tail in the map) after a mouvement */
void moveTail() {
  unsigned char tailNum = map[post.y][post.x];
  map[post.y][post.x] = 0;
  // update the tail
  if (post.x < WIDTH && map[post.y][post.x + 1] == tailNum - 1) {
    post.x++;
  } else if (post.x > 0 && map[post.y][post.x - 1] == tailNum - 1) {
    post.x--;
  } else if (post.y < HEIGHT && map[post.y + 1][post.x] == tailNum - 1) {
    post.y++;
  } else if (post.y > 0 && map[post.y - 1][post.x] == tailNum - 1) {
    post.y--;
  }
}

/* Update all the snake after a movement */
void updateSnake(int x, int y) {
  map[y][x]++;
  if (!isTail(x, y)) {
    if (x < WIDTH && map[y][x + 1] == map[y][x])
      updateSnake(x + 1, y);
    else if (x > 0 && map[y][x - 1] == map[y][x])
      updateSnake(x - 1, y);
    else if (y < HEIGHT && map[y + 1][x] == map[y][x])
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
    posf.x = rand() % WIDTH;
    posf.y = rand() % HEIGHT;
  }
}

int eventHandler(char act) {
  int output = 1;

  switch (act) {
  case 'k':
    if (moveUp()) {
      updateMovement();
      dir = 'k';
    } else {
      output = 0;
    }
    break;
  case 'j':
    if (moveDown()) {
      updateMovement();
      dir = 'j';
    } else {
      output = 0;
    }
    break;
  case 'l':
    if (moveRight()) {
      updateMovement();
      dir = 'l';
    } else {
      output = 0;
    }
    break;
  case 'h':
    if (moveLeft()) {
      updateMovement();
      dir = 'h';
    } else {
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
  start_color();
  init_pair(CMAP, COLOR_BLACK, COLOR_GREEN);
  init_pair(CBODY, COLOR_BLACK, COLOR_CYAN);
  init_pair(CHEAD, COLOR_BLACK, COLOR_BLUE);
  init_pair(CFRUIT, COLOR_BLACK, COLOR_RED);
  halfdelay(2);
  while (cont) {
    clear();
    refresh();
    displayMap();
    resp = getch();
    if (resp == ERR)
      resp = dir;
    cont = eventHandler(resp);
  }
  endwin();

  printf("Score:\n");
  printf("- fruit: %d\n", length - 2);
  printf("- length: %d\n", length);
}

int main(void) {
  // init randomizer
  srand(time(NULL));
  // setting up global varibles
  posh.x = 10;
  posh.y = 10;
  post.x = 10;
  post.y = 9;
  length = 2;
  dir = 'j';
  posf.x = rand() % WIDTH;
  posf.y = rand() % HEIGHT;
  initMap();
  run();
  return 0;
}
