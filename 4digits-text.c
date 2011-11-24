/*
   4digits - A guess-the-number game, aka Bulls and Cows
   Copyright (c) 2004-2011 Pan Yongzhi <http://fourdigits.sourceforge.net>

   4digits is a guess-the-number puzzle game. It's called Bulls and Cows,
   and in China people simply call it Guess-the-Number. The game's
   objective is to guess a four-digit number in 8 times using as less time
   as possible. It is similar to Mastermind, but the four digits are
   different to each other. 4digits has a graphical user interface version
   4digits and a textual user interface version 4digits-text. 

   4digits is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   4digits is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with 4digits; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
   */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

//#define DEBUG
#define VERSION_STRING "1.1, Nov 2011"

const char COPYRIGHT[] = "4digits " VERSION_STRING "\n"
"4digits comes with NO WARRANTY to the extent permitted by law.\n"
"This program is free software; you can redistribute it and/or\n"
"modify it under the terms of the GNU General Public License as\n"
"published by the Free Software Foundation - version 2. For more\n"
"information about these matters, see the file named COPYING.\n";
const char AUTHOR[] = "Written by Pan Yongzhi.\n";

const char HELP[] = 
"4digits, a guess-the-number game.\n"
"Usage: 4digits [OPTION] ...\n"
"\n"
"You are given eight times to guess a four-digit number. You get\n"
"one A if its value and position are both correct, and you get one\n"
"B if only its value is correct. You win the game when you get 4A0B.\n"
"\n"
"-v, --version \t display the version of 4digits and exit.\n"
"-h, -?, --help \t print this help.\n"      
"\n"
"Report bugs at <https://github.com/fossilet/4digits/issues>.";

static const char *optString = "vh?";
struct globalArgs_t {
    char *version; // version option
} globalArgs;

static const struct option longOpts[] = {
    { "version", no_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
};

void err_msg(const char* msg);
void gen_rand(int ans_digits[]);
int enter_number(void);
void save_score(const int time_taken);
void compare(const int *in_digits, const int *ans_digits, int *A, int *B);
int tenpow(int power);

void err_msg(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

/* generate a random number */
void gen_rand(int ans_digits[]) {
    srand((unsigned)time(NULL));
    int ans = 1000 + (int)(8999.0*rand()/RAND_MAX);
    for(int i=0; i<4; i++)
        ans_digits[i] = (int)(ans/tenpow(3-i)) % 10;

    /* if 4 digits is not different from each other, regenerate it*/
    while(ans_digits[0]==ans_digits[1] || ans_digits[1]==ans_digits[2]
            || ans_digits[2]==ans_digits[3] || ans_digits[0]==ans_digits[2]
            || ans_digits[0]==ans_digits[3] || ans_digits[1]==ans_digits[3])
    {
        ans = 1000 + (int)(8999.0 * ((double)rand()/RAND_MAX)); 
        for(int i=0; i<4; i++)
            ans_digits[i] = (int)(ans / tenpow(3-i))%10;
    }
#ifdef DEBUG
    printf("%d\n", ans);
#endif
}

/* enter a 4-digit number */
int enter_number() {
    char mstr[5]={'\0','\0','\0','\0','\0'}; 
    int c;
    int input;
    int in_digits[4]={0,0,0,0}; /* arrays for the 4 digits of input*/
    bool reinput;
    do {
        reinput = false;
        printf("Input a 4-digit number:");
        if(fgets(mstr, sizeof mstr, stdin) == NULL)
            err_msg("Something's got wrong, I'd better quit...\n");
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
            fprintf(stderr, "Must be a number between 1000 and 9999!\n");
            reinput = true;
            continue;
        }
        for(int i=0; i<4; i++)
            in_digits[i]=(int) (input / tenpow(3-i) )%10;
        if(in_digits[0]==in_digits[1] || in_digits[1]==in_digits[2] 
                || in_digits[2]==in_digits[3] || in_digits[0]==in_digits[2]
                || in_digits[0]==in_digits[3] || in_digits[1]==in_digits[3])
        {
            fprintf(stderr, "Four digits must be unique.\n");
            reinput = true;
            continue;
        }
    }while(reinput);
    return input;
}

/* compare answer and input, refresh A & B */
void compare(const int *in_digits, const int *ans_digits, int *a, int *b) {
    for(register int i=0 ; i<4; i++)
        for(register int j=0 ; j<4; j++)
            if(in_digits[i] == ans_digits[j])
                (i == j) ? (*a)++ : (*b)++;
}

/* save current score in the score file */
void save_score(const int time_taken) {
    time_t tm = time(NULL);
    struct tm *today = localtime(&tm);
    char tmpbuffer[129];
    today = localtime(&tm);
    char appdata_dir[4096]; //XXX why _PC_PATH_MAX is only 4?
    const char *score_filename = "4digits.4digits.scores";
    strcpy(appdata_dir, getenv("HOME"));
    strcat(appdata_dir, "/.4digits/");
    char *scorefile = (char*)malloc(strlen(appdata_dir) + strlen(score_filename) + 1);
    if(!scorefile)
        err_msg("Memory allocation error.\n");
    strcpy(scorefile, appdata_dir);
    strcat(scorefile, score_filename);

    FILE *sfp = fopen(scorefile, "a+");
    if (!sfp) {
        if (errno == ENOENT) {
            DIR *dp = opendir(appdata_dir);
            if(!dp)
                if (errno == ENOENT) {
                    int ret = mkdir(appdata_dir, 0700);
                    if (ret == -1)
                        err_msg("Cannot open score file.\n");
                    sfp = fopen(scorefile, "a+");
                }
        }
        else
            err_msg("Cannot open score file.\n");
    }
    strftime(tmpbuffer, 128, "%a %b %d %H:%M:%S %Y", today); 
    struct passwd *pwd;
    pwd = getpwuid(geteuid());
    // getenv("USERNAME") conforms to C99 thus is more portable.
    fprintf(sfp, "%s %ds %s\n", pwd->pw_name, time_taken, tmpbuffer);
    free(scorefile);
}

int tenpow(int exponent) {
    int output = 1;
    for(int i=0; i < exponent; i++)
        output *= 10;
    return output;
}

int main(int argc, char *argv[]) {
    int opt = 0;
    int longIndex = 0;

    /* Initialize globalArgs before we get to work. */
    globalArgs.version = NULL;    /* false */

    // Process the arguments with getopt_long(), then populate globalArgs
    opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
    while(opt != -1) {
        switch(opt) {
            case 'v':
                globalArgs.version = VERSION_STRING;
                printf("%s\n%s", COPYRIGHT, AUTHOR);
                exit(1);
            case 'h': 
                err_msg(HELP);
                break;
            case '?': /* fall-through is intentional */
                err_msg("Usage: 4digits [OPTION]...\n"
                        "Try `4digits --help' for more information.");
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

    int ans_digits[4];
    gen_rand(ans_digits); /* array for the 4 digits of n*/
    time_t temp = time(NULL);
    time_t tic = temp;
    int guessed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int i;
    bool dup = false;

    for (int num_guess = 0; num_guess < 8; num_guess++) {
        int A = 0, B = 0;
        int input = enter_number();

        for(int i=0; i < num_guess; i++)
            // duplicated input
            if (input == guessed[i]) {
                fprintf(stderr, "You've already guessed it.\n");
                --num_guess;
                dup = true;
                break;
            }

        if (dup == true) {
            dup = false;
            continue;
        }

        int in_digits[4]; /* arrays for the 4 digits of input*/
        for(i=0; i<4; i++) {
            in_digits[i]=(int) (input / tenpow(3-i) )%10;
        }

        compare(in_digits, ans_digits, &A, &B);
        printf("%dA%dB    ", A, B);
        if (num_guess != 7)
            printf("\t %d times left.\n", 7-num_guess);
        guessed[num_guess] = input;

        if(A == 4) {
            time_t toc = time(NULL);
            int score = (int)(toc-tic);
            printf("You win! :) Used %d sec.\n", score);
            save_score(score); /* save score in the score file */
            return 0;
        }
    }
    printf("\nHaha, you lose. It is ");
    for(int i = 0; i < 4; i++)
        printf("%d", ans_digits[i]);
    printf(".\n");
    return 0;
}
