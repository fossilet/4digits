/*
  4digits - A guess-the-number game, aka Bulls and Cows
  Copyright (c) 2004-2009 Pan Yongzhi and MA Xu <http://fourdigits.sourceforge.net>

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

//#define DEBUG
#define VERSION_STRING "0.9.1, Nov 2008"
#define DIGITS_MAX 9
/* 10 is Equal to 9
 * Since There Are Only 10 Digital Number
 */
static const int GUESS_MAX[DIGITS_MAX+1] ={
  /*0 1 2 3 4 5 6  7  8  9  10 */
    0,4,5,6,7,9,11,13,15,17
};
int digits_type = 4;
const char NAME_GAME[DIGITS_MAX+1][6]={
  "Zero","One","Two","Three","Four","Five","Six","Seven","Eight","Nine"
};

const char COPYRIGHT[] = "4digits " VERSION_STRING "\n"
  "4digits comes with NO WARRANTY to the extent permitted by law.\n"
  "This program is free software; you can redistribute it and/or\n"
  "modify it under the terms of the GNU General Public License as\n"
  "published by the Free Software Foundation - version 2. For more\n"
  "information about these matters, see the file named COPYING.\n";

const char AUTHOR[] =
  "Written by Pan Yongzhi.\n"
  "Maintain by MA Xu ( njmarshal@gmail.com ) \n";

const char HELP[] =
  "4digits, a guess-the-number game.\n"
  "Usage: 4digits [OPTION] ...\n"
  "\n"
  "You are given eight times to guess a four-digit number. You get\n"
  "one A if its value and position are both correct, and you get one\n"
  "B if only its value is correct. You win the game when you get 4A0B.\n"
  "\n"
  "-d [num], --digits [num] \t change the number of digits to num, default is 4. \n"
  "-v, --version \t display the version of 4digits and exit.\n"
  "-h, -?, --help \t print this help.\n"
  "\n"
  "Report bugs at <http://sourceforge.net/projects/fourdigits/>.";

static const char *optString = "d:vh?";
struct globalArgs_t {
  char *version; // version option
} globalArgs;

static const struct option longOpts[] = {
  { "digits", optional_argument, NULL, 'd'},
  { "version", no_argument, NULL, 'v' },
  { "help", no_argument, NULL, 'h' },
  { NULL, no_argument, NULL, 0 }
};

void print_help(void);
void display_usage(void);
void gen_rand(int ans_digits[]);
int enter_number(void);
void save_score(const int time_taken);
void compare(const int *in_digits, const int *ans_digits, int *A, int *B);
void change_digits(int);
int tenpow(int);
void print_help(void) {
  (void)puts(HELP);
  exit(EXIT_FAILURE);
}

void display_usage(void) {
  (void)puts("Usage: 4digits [OPTION]...\n");
  (void)puts("Try `4digits --help' for more information.");
  exit(EXIT_FAILURE);
}

/* generate random numbers sequence */
void gen_rand(int ans_digits[]){
  float ftemp,rand_num[ DIGITS_MAX ];
  int min,i,j,itemp;

  srand((unsigned)time(NULL));
  do{
    for(i=0; i<DIGITS_MAX; i++){
      ans_digits[i] = i;
      rand_num[i] = rand();
    }
    for(i=0; i<DIGITS_MAX; i++){
      min = i;
      for(j=i+1; j<DIGITS_MAX; j++){
        if(rand_num[j]<rand_num[min])
          min=j;
      }
      if(min!=i){
        itemp = ans_digits[i];
        ans_digits[i] = ans_digits[min];
        ans_digits[min] = itemp;
        ftemp = rand_num[i];
        rand_num[i] = rand_num[min];
        rand_num[min] = ftemp;
      }
    }
  }while(0==ans_digits[0]);

#ifdef DEBUG
  for (i=0;i<digits_type;i++){
    printf("%d ", ans_digits[i]);
  }
  printf("\n");
#endif
}

#if false
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
#endif

/* enter a 4-digit number */
int enter_number() {
  char mstr[DIGITS_MAX+1]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
  int c;
  int input;
  int in_digits[DIGITS_MAX]={0,0,0,0,0,0,0,0,0}; /* arrays for the 4 digits of input*/
  bool reinput;
  do {
    reinput = false;
    printf("Input a %s-digit [ %d ] number:", NAME_GAME[digits_type], digits_type);
    if(fgets(mstr, digits_type+1, stdin) == NULL) {
      fprintf(stderr, "Something's got wrong, I'd better quit...\n");
      exit(EXIT_FAILURE);
    }
    // fgets appends the newline entered, if it appears in the first 4
    // elements of mstr, then it's sure less than 4 digits are entered
    bool effective_flag = true;
    bool flag=false;

    for (int i=0;i<digits_type;i++)
      if (mstr[i]=='\n')
        effective_flag = false;

    if (true == effective_flag)
      /* discard the character */
      while((c = getchar()) != '\n' && c != EOF)
        flag = true;

    if (flag == true) {
      fprintf(stderr, "Input too long!\n");
      reinput = true;
      continue;
    }
    input = atoi(mstr);
    if (input < tenpow(digits_type-1) || input > tenpow(digits_type)-1) {
      fprintf(stderr, "Must be a number between %d and %d!\n",tenpow(digits_type-1),tenpow(digits_type)-1);
      reinput = true;
      continue;
    }
    for(int i=0; i<digits_type; i++)
      in_digits[i]=(int) (input / tenpow(digits_type-1-i) )%10;
    bool unique_flug = true;
    for(int i=0; i<digits_type; i++)
      for(int j=i+1; j< digits_type; j++)
        if(in_digits[i]==in_digits[j])
          unique_flug = false;

    if(false == unique_flug)
      {
        fprintf(stderr, "%s digits must be unique.\n",NAME_GAME[digits_type]);
        reinput = true;
        continue;
      }
  }while(reinput);
  return input;
}

/* compare answer and input, refresh A & B */
void compare(const int *in_digits, const int *ans_digits, int *a, int *b) {
  for(register int i=0 ; i<digits_type; i++)
    for(register int j=0 ; j<digits_type; j++)
      if(in_digits[i] == ans_digits[j])
        (i == j) ? (*a)++ : (*b)++;
}

/* save current score in the score file */
void save_score(const int time_taken) {
  time_t tm = time(NULL);
  struct tm *today = localtime(&tm);
  char tmpbuffer[129];
  today = localtime(&tm);
  const char *sfpath = getenv("HOME");
  const char score_filename[] = "/.4digits/4digits-text.4digits.scores";
  char *scorefile = (char *)malloc(strlen(sfpath) + strlen(score_filename) + 1);
  if(!scorefile) {
    fprintf(stderr, "Memory allocation error.\n");
    exit(EXIT_FAILURE);
  }
  strcpy(scorefile, sfpath);
  strcat(scorefile, score_filename);
  FILE *sfp = fopen(scorefile, "a+");
  if (!sfp) {
    fprintf(stderr, "Cannot open score file.\n");
    exit(EXIT_FAILURE);
  }
  strftime(tmpbuffer, 128, "%a %b %d %H:%M:%S %Y", today);
  fprintf(sfp, "%s %ds %s\n", getlogin(), time_taken, tmpbuffer);
  free(scorefile);
}

void change_digits(int num){
  if( num>0 && num <= DIGITS_MAX ){
    printf("%s Changed to %s digit(s).\n",NAME_GAME[digits_type],NAME_GAME[num]);
    digits_type = num;
  }
  else
    printf("%s digit(s) Unchanged.\n",NAME_GAME[digits_type]);
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
    case 'd':
      /* printf("%s",optarg); */
      if(NULL != optarg)
        change_digits(atoi(optarg));
      break;
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

  int ans_digits[DIGITS_MAX];
  gen_rand(ans_digits); /* array for the 4 digits of n*/
  time_t temp = time(NULL);
  time_t tic = temp;
  int guessed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int i;
  bool dup = false;

  for (int num_guess = 0; num_guess < GUESS_MAX[digits_type]+1; num_guess++) {
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

    int in_digits[digits_type]; /* arrays for the 4 digits of input*/
    for(i=0; i<digits_type; i++) {
      in_digits[i]=(int) (input / tenpow(digits_type-1-i) )%10;
    }

    compare(in_digits, ans_digits, &A, &B);
    printf("%dA%dB    ", A, B);
    //if (num_guess != 7)
    //  printf("\t %d times left.\n", 7-num_guess);
    if (num_guess != GUESS_MAX[digits_type])
      printf("\t %d times left.\n", GUESS_MAX[digits_type] - num_guess);
    guessed[num_guess] = input;
    //if(A == 4) {
    if(A == digits_type) {
      time_t toc = time(NULL);
      int score = (int)(toc-tic);
      printf("You win! :) Used %d sec.\n", score);
      save_score(score); /* save score in the score file */
      return 0;
    }
  }
  printf("\nHaha, you lose. It is ");
  for(int i = 0; i < digits_type; i++)
    printf("%d", ans_digits[i]);
  printf(".\n");
  return 0;
}
