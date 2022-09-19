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
int           score;
int           state; // 0: game, 1: victory, -1: defeate, 2:quit
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
void drawMap() {
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
void drawScore() {
  char formatedScore[20];
  sprintf(formatedScore, "score: %d", score);
  mvaddstr(Y_OFFSET - 1, X_OFFSET, formatedScore);
}

/* Display a smiling face in green */
void drawWinScr() {
  int picture[6][5] = {{0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0},
		       {0, 0, 0, 0, 0}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}};
  int i,j;

  attron(COLOR_PAIR(CBODY));
  for (i = 0; i < 6; ++i) {
    for (j = 0; j < 5; ++j) {
      if (picture[i][j] == 1) {
	mvaddstr(i + (LINES - 6)/2, 2*j + (COLS - 5)/2, "  ");
      }
    }
  }
  attroff(COLOR_PAIR(CBODY));
}

/* Display not smilings face in red */
void drawLooseScr() {
  int picture[6][5] = {{0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0},
		       {0, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}};
  int i,j;

  attron(COLOR_PAIR(CFRUIT));
  for (i = 0; i < 6; ++i) {
    for (j = 0; j < 5; ++j) {
      if (picture[i][j] == 1) {
	mvaddstr(i + (LINES - 6)/2, 2*j + (COLS - 5)/2, "  ");
      }
    }
  }
  attroff(COLOR_PAIR(CFRUIT));
}

/* Main draw function */
void draw() {
  if (state == 0) {
    drawMap();
    drawScore();
  } else if (state == -1) {
    drawLooseScr();
  } else {
    drawWinScr();
  }
  mvaddstr(LINES - 1, COLS - 1, NULL); // get rid of the white cursor
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

/**
 * Update the global variable `posf` with a new position for the new fruit.
 */
void generateNewFruit() {
  int i, j;
  int founded = 0;

  posf.x = rand() % GRID_WIDTH;
  posf.y = rand() % GRID_HEIGHT;

  /* Move the fruit if it appears on the snake.
   * Just search an available case to put the fruit in before or after the
   * random position we've just found so the food never appears on the snake and
   * it still "random".
   */
  if (map[posf.y][posf.x] > 0) {
    for (i = posf.y; !founded && i < GRID_HEIGHT; ++i) {
      for (j = posf.x; !founded && j < GRID_WIDTH; ++j) {
	if (map[i][j] == 0) {
	  posf.y = i;
	  posf.x = j;
	  founded = 1;
	}
      }
    }
    for (i = posf.y; !founded && i > 0; --i) {
      for (j = posf.x; !founded && j > 0; --j) {
	if (map[i][j] == 0) {
	  posf.y = i;
	  posf.x = j;
	  founded = 1;
	}
      }
    }
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
    score++;
    if (score == GRID_WIDTH * GRID_HEIGHT) {
      state = 1;
    } else {
      // TODO: get a position out of the snake
      posf.x = rand() % GRID_WIDTH;
      posf.y = rand() % GRID_HEIGHT;
    }
  }
}

/**
 * Move and update the snake or end the game if the snike hit a wall or itself.
 */
void moveSnake(char nextDir) {
  switch (nextDir) {
  case 'k':
    if (moveUp()) {
      updateMovement();
      dir = 'k';
    } else if (dir != 'j') {
      state = -1;
    }
    break;
  case 'j':
    if (moveDown()) {
      updateMovement();
      dir = 'j';
    } else if (dir != 'k') {
      state = -1;
    }
    break;
  case 'l':
    if (moveRight()) {
      updateMovement();
      dir = 'l';
    } else if (dir != 'h') {
      state = -1;
    }
    break;
  case 'h':
    if (moveLeft()) {
      updateMovement();
      dir = 'h';
    } else if (dir != 'l') {
      state = -1;
    }
    break;
  }
}

/**
 * Take the key entered by the player, if there is no key (ERR) the snake goes
 * kstrait forward so the direction doesn't change. Otherwise, either the game is
 * finished so we quit the program on any key-presse, or we return the key entered
 * kby the player.
*/
char eventHandler(char key) {
  char nextDir = key;
    if (key == ERR) // no answer
      nextDir = dir;
    else if (state == 1 || state == -1 || key == 'q') // end the game on any keypress
      state = 2;
    return nextDir;
}

/**
 * Update the highest score in the file: `./highestScore.txt`
 */
void updateHighestScore() {
  FILE *f = fopen("./highestScore.txt", "w");
  int oldScore = 0;

  if (f) {
    // get the old score
    rewind(f);
    fscanf(f, "%d", &oldScore);

    // write the new score
    if (score > oldScore) {
      fprintf(f, "%d\n", score);
    }

    fclose(f);
  } else {
    fprintf(stderr, "ERROR: can't open score file.\n");
  }
}

void run() {
  char resp;
  char nextDir;

  initscr();
  cbreak(); // turn on cbreak mode
  noecho(); // turn on noecho mode

  /* if colors are not supported by the terminal or tty */
  if (has_colors() == FALSE) {
    fprintf(stderr, "error: colors not supported!\n");
    endwin();
    exit(EXIT_FAILURE);
  }

  // define some colors
  start_color();
  init_pair(CMAP, COLOR_BLACK, COLOR_GREEN);
  init_pair(CBODY, COLOR_BLACK, COLOR_CYAN);
  init_pair(CHEAD, COLOR_BLACK, COLOR_BLUE);
  init_pair(CFRUIT, COLOR_BLACK, COLOR_RED);
  halfdelay(WAIT_TIME);

  while (state != 2) {
    resp = getch();
    nextDir = eventHandler(resp);
    moveSnake(nextDir); // move the snake at each iteration
    clear();
    draw();
    refresh();
  }
  endwin();

  printf("Score: %d\n", score);
  updateHighestScore();
}

int main(void) {
  // init randomizer
  srand(time(NULL));
  // setting up global varibles
  posh.x = 0;
  posh.y = 10;
  post.x = 0;
  post.y = 9;
  score = 0;
  dir = 'j';
  state = 0;
  posf.x = rand() % GRID_WIDTH;
  posf.y = rand() % GRID_HEIGHT;
  initMap();
  run();
  return 0;
}
