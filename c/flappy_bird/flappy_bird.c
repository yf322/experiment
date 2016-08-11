#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define CHAR_BIRD 'O'
#define CHAR_STONE '*'
#define CHAR_BLANK ' '
#define MAX_LEN 23

typedef struct node {
	int x, y;
	struct node *next;
}node, *Node;

Node head, tail;
int bird_x, bird_y;
int ticker;

void init();
void init_bird();
void init_draw();
void init_head();
void init_wall();
void drop(int sig);
int set_ticker(int n);

int set_ticker(int n_msec) {
    struct itimerval timeset;
    long n_sec, n_usec;

    n_sec = n_msec / 1000;
    n_usec = (n_msec % 1000) * 1000L;

    timeset.it_interval.tv_sec = n_sec;
    timeset.it_interval.tv_usec = n_usec;

    timeset.it_value.tv_sec = n_sec;
    timeset.it_value.tv_usec = n_usec;

    return setitimer(ITIMER_REAL, &timeset, NULL);
}
	
void drop(int sig) {
    int j;
    Node tmp, p;

    move(bird_y, bird_x);
    addch(CHAR_BLANK);
    refresh();
    bird_y ++;
    move(bird_y, bird_x);
    addch(CHAR_BIRD);
    refresh();
    if((char)inch() == CHAR_STONE) { // hit the pillar 
        set_ticker(0);
        sleep(1);
        endwin();
        exit(0);
    }

    p = head->next;
    if(p->x < 0) {
        head->next = p->next;
        free(p);
        tmp = malloc(sizeof(node));
        tmp->x = 99;
        do {
            tmp->y = rand() % 16; // height
        } while(tmp->y < 5); 
        tail->next = tmp;
        tmp->next = NULL;
        tail = tmp;
        ticker -= 10;  
        set_ticker(ticker);
    }

    for(p = head->next; p->next != NULL; p->x--, p = p->next) { // x position
        for(j = 0; j < p->y; j++) { // for the blank between two pillar
            move(j, p->x);
            addch(CHAR_BLANK);
            refresh();
        }
        for(j = p->y+5; j <= MAX_LEN; j++) { // for the blank area in between up and down
            move(j, p->x);
            addch(CHAR_BLANK);
            refresh();
        }

        if(p->x-10 >= 0 && p->x < 80) { // the pillars' base
            for(j = 0; j < p->y; j++) { // the pillars' height below
                move(j, p->x-10);
                addch(CHAR_STONE);
                refresh();
            }
            for(j = p->y + 5; j <= MAX_LEN; j++) { // the pillars' height above
                move(j, p->x-10);
                addch(CHAR_STONE);
                refresh();
            }
        }
    }
    tail->x--; // move one frame
}

int main() {
    char ch;

    init();
    while(1) { // w or W or space to go up
        ch = getch();
        if(ch == ' ' || ch == 'w' || ch == 'W') {
            move(bird_y, bird_x);
            addch(CHAR_BLANK);
            refresh();
            bird_y --;
            move(bird_y, bird_x);
            addch(CHAR_BIRD);
            refresh();
            if((char)inch() == CHAR_STONE) {
                set_ticker(0);
                sleep(1);
                endwin();
                exit(0);
            }
        }
        else if(ch == 'z' || ch == 'Z') { // pause
            set_ticker(0);
            do {
                ch = getch();
            } while(ch != 'z' && ch != 'Z');
            set_ticker(ticker);
        }
        else if(ch == 'q' || ch == 'Q') { // quit
            sleep(1);
            endwin();
            exit(0);
        }
    }

    return 0;
}

void init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    srand(time(0));
    signal(SIGALRM, drop);

    init_bird(); // initial everything
    init_head();
    init_wall();
    init_draw();
    sleep(1);
    ticker = 500;
    set_ticker(ticker);
}

void init_bird() {
    bird_x = 5;
    bird_y = 15; // initial position at 5,15
    move(bird_y, bird_x);
    addch(CHAR_BIRD);
    refresh();
    sleep(1);
}

void init_head() { 
    Node tmp;

    tmp = malloc(sizeof(node));
    tmp->next = NULL;
    head = tmp;
    tail = head;
}

void init_wall() {
    int i;
    Node tmp, p;

    p = head;
    for(i = 19; i <= 99; i += 20) { // first blank 9, first wall 10, 10 for the rest
        tmp = malloc(sizeof(node));
        tmp->x = i;
        do {
            tmp->y = rand() % 16; // no taller than 16
        }while(tmp->y < 5);
        p->next = tmp;
        tmp->next = NULL;
        p = tmp;
    }
    tail = p;
}

void init_draw() {
    Node p;
    int i, j;

    for(p = head->next; p->next != NULL; p = p->next) { // draw the pillar
        for(i = p->x; i > p->x-10; i--) { // the base
            for(j = 0; j < p->y; j++) { // draw the below
                move(j, i);
                addch(CHAR_STONE);
                refresh();
            }
            for(j = p->y+5; j <= MAX_LEN; j++) { // draw the above 
                move(j, i);
                addch(CHAR_STONE);
                refresh();
            }
        }
        sleep(1);
    }
}

