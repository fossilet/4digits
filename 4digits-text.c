/*
 * 4digits - This program make the interactive game 
 * number guessing game available on your computer.
 *
 * gcc -Wall -std=c99 -pedantic -o 4digits 4digits.c
 *
 * Copyright (C) 2005, 2006 Pan Yongzhi
 * panyongzhi [at] gmail [dot] com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include <getopt.h>
#include <unistd.h>

//#define DEBUG
#define VERSION_STRING "0.3, Aug 2006"

const char AUTHOR[] = "Written by Pan Yongzhi.\n";
const char COPYRIGHT[] = "4digits " VERSION_STRING "\n"
"4digits comes with NO WARRANTY to the extent permitted by law.\n"
"This program is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU General Public License as\n"
"published by the Free Software Foundation - version 2. For more\n"
"information about these matters, see the file named COPYING.\n";
const char HELP[] = 
"4digits 0.3, a guess-the-number game.\n"
"Usage: 4digits [OPTION]...\n"
"\n"
"You have eight times to guess a 4-digit number. You get one A \n"
"if its value and position are both correct, and one B \n"
"if only its value is correct. You win the game when you get   \n"
"4A0B. Good luck!\n"
"\n"
"-v, --version \t display the version of 4digits and exit.\n"
"-h, -?, --help \t print this help.\n"      
"\n"
"Report bugs to <fossilet@@163.com>.";
static const char *optString = "vh?";
struct globalArgs_t {
    char *version;              // version option
} globalArgs;

static const struct option longOpts[] = {
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
};

void showopt(void);
void print_help(void);
void display_usage(void);
void gen_rand(int answerdgts[4]);
int enter_number(void);
void show_high_score(void);
void save_score(const int time_taken);
void compare(const int *inputdgts, const int *answerdgts, int *A, int *B);
int tenpow(int power);

void showopt(void) {
    printf("version: %s\n", globalArgs.version);
}

void print_help(void) {
    puts(HELP);
    exit(EXIT_FAILURE);
}

void display_usage(void) {
    puts("Usage: 4digits [OPTION]...\n");
    puts("Try `4digits --help' for more information.");
    exit(EXIT_FAILURE);
}

/* generate a random number */
void gen_rand(int answerdgts[4]) {
    srand(time(NULL));
    int answer = 1000 + (int) (8999.0 * rand() / RAND_MAX);
    for(int i=0; i<4; i++)
        answerdgts[i]=(int) (answer / tenpow(3-i))%10;

    /* if 4 digits of n is not different from each other, regenerate it*/
    while(answerdgts[0]==answerdgts[1] || answerdgts[1]==answerdgts[2]
            || answerdgts[2]==answerdgts[3] || answerdgts[0]==answerdgts[2]
            || answerdgts[0]==answerdgts[3] || answerdgts[1]==answerdgts[3]) {
        answer = 1000 + (int)(8999.0 * ((double)rand()/RAND_MAX)); 
        for(int i=0; i<4; i++)
            answerdgts[i] = (int)(answer / tenpow(3-i))%10;
    }
#ifdef DEBUG
    printf("%d\n", answer);
#endif
}

/* enter a 4-digit number */
int enter_number() {
    char mstr[5]={'\0', '\0', '\0', '\0', '\0'}; 
    int c;
    int input;
    int inputdgts[4]={0,0,0,0}; /* arrays for the 4 digits of input*/
    bool reinput;
    do {
        reinput = false;
        printf("Input a 4-digit number:");
        if(fgets(mstr, sizeof mstr, stdin) == NULL) {
            fprintf(stderr, "Something's got wrong, I'd better quit...\n");
            exit(EXIT_FAILURE); 
        }
        // fgets appends the newline entered, if it appears in the first 4
        // elements of mstr, then it's sure less than 4 digits are entered
        bool flag = false;
        if(mstr[0]!='\n'&& mstr[1]!='\n'&& mstr[2]!='\n' &&mstr[3]!='\n') 
            /* discard the character */
            while((c = getchar()) != '\n' && c != EOF)
                flag = true;
        if (flag == true) {
            fprintf(stderr, "Input too long!\n");
            reinput = true;
            continue;
        }
        input = atoi(mstr);
        if (input < 1000 || input > 9999) {
            fprintf(stderr, \
                    "%d: Input error, must be a number between 1000 and 9999!\n", input);
            reinput = true;
            continue;
        }
        for(int i=0; i<4; i++)
            inputdgts[i]=(int) (input / tenpow(3-i) )%10;
        if(inputdgts[0]==inputdgts[1] || inputdgts[1]==inputdgts[2] 
                ||inputdgts[2]==inputdgts[3] || inputdgts[0]==inputdgts[2]
                || inputdgts[0]==inputdgts[3] || inputdgts[1]==inputdgts[3]) {
            fprintf(stderr, "%d: Input error, 4 digits must be unique!\n", input);
            reinput = true;
            continue;
        }
    }while(reinput);
    return input;
}

/* compare answer and input, refresh A & B */
void compare(const int *inputdgts, const int *answerdgts, int *a, int *b) {
    for(register int i=0 ; i<4; i++)
        for(register int j=0 ; j<4; j++)
            if(inputdgts[i] == answerdgts[j])
                (i == j) ? (*a)++ : (*b)++;
}

/* save current score in the score file */
void save_score(const int time_taken) {
    time_t tm = time(NULL);
    struct tm *today = localtime(&tm);
    char tmpbuffer[129];

    /* user name, eg. tux, mimic emacs tetris-scores */
    today = localtime(&tm);
    const char *sfpath = getenv("HOME");
#ifdef DEBUG
    const char sfname[] = "/.fourdgtsscrdbg";
#else 
    const char sfname[] = "/.4digits-scores";
#endif
    char *scorefile = (char *)malloc(strlen(sfpath) + strlen(sfname) + 1);
    if(!scorefile) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
    }
    strcpy(scorefile, sfpath);
    strcat(scorefile, sfname);
    FILE *sfp = fopen(scorefile, "a+");
    if (!sfp) {
        fprintf(stderr, "Cannot open score file!\n");
        exit(EXIT_FAILURE);
    }
    strftime(tmpbuffer, 128, "%a %b %d %H:%M:%S %Y", today); 
    fprintf(sfp, "%ds %s %s\n", time_taken, getlogin(), tmpbuffer);
    free(scorefile);
}

/* show high scores */
void show_high_score(void) {
    // open sfp
    // read scores
    // sort lines according to scores
    // print lines after sorting
}

int tenpow(int exponent) {
    int output = 1;
    for(int i=0; i < exponent; i++)
        output *= 10;
    return output;
}

int main(int argc, char *argv[]) {
    // puts(COPYRIGHT); 
    int opt = 0;
    int longIndex = 0;

    /* Initialize globalArgs before we get to work. */
    globalArgs.version = NULL;    /* false */

    // Process the arguments with getopt_long(), then populate globalArgs. 
    opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while(opt != -1) {
        switch(opt) {
            case 'v':
                globalArgs.version = VERSION_STRING;
                printf("%s\n%s", COPYRIGHT, AUTHOR);
                exit(1);
            case 'h': 
                print_help();
                break;
            case '?': /* fall-through is intentional */
                display_usage();
                break; 
            case 0:    /* long option without a short arg */
                if(strcmp("version", longOpts[longIndex].name) == 0)
                    break; 
            default:
                /* You won't actually get here. */
                break;
        }
        opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    }

#ifdef DEBUG
    showopt();
#endif
    //return EXIT_SUCCESS;

    // while(1) {
    int answerdgts[4];
    gen_rand(answerdgts); /* array for the 4 digits of n*/
    time_t temp = time(NULL);
    time_t tic = temp;
    int lastinput = 0;
    // int guessed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int i;
    for (int num_guess = 0; num_guess < 8; num_guess++) {
        int A = 0, B = 0;
        int input = enter_number();
        //  guessed[num_guess] = input;
        //#ifdef DEBUG
        ////    printf("I guessed: %d\n", guessed[num_guess]);
        ////#endif
        /*    for(i = 0; i < num_guess; ++i) 
              if (input == guessed[i]) { // duplicated input
              fprintf(stderr, "%d: You have entered that number!\n", input);
              break;
              }
              --num_guess;
              continue;
              */
        if (input == lastinput) { // duplicated input
            fprintf(stderr, "%d: You have entered that number!\n", input);
            --num_guess;
            continue;
        }
        int inputdgts[4]; /* arrays for the 4 digits of input*/
        for(i=0; i<4; i++) {
            inputdgts[i]=(int) (input / tenpow(3-i) )%10;
        }
        compare(inputdgts, answerdgts, &A, &B);
        printf("%dA%dB    ", A, B);
        if (num_guess != 7)
            printf("\t %d times left.\n", 7-num_guess);
        lastinput = input;
        if(A == 4) {
            time_t toc = time(NULL);
            int score = (int)(toc-tic);
            printf("You win in %d sec.\n", score);
            save_score(score); /* save score in the score file */
            return 0;
        }
    }
    printf("\nHahah, you lose. The number is ");
    for(int i = 0; i < 4; i++)
        printf("%d", answerdgts[i]);
    printf(".\n");
    return 0;
    //  }
}
