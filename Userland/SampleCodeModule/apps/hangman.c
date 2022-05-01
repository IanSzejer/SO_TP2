
#include "hangman.h"
#define MAX_FAILS 6
static char GUESSING_WORD[]= "HOLA";     //QUIERO QUE SEA FINAL

static char userWord[40] ={0};
static int gameEnded=0;
static char lettersUsed[52] = {0};
static int letterIndex=0;
static int wrongLetters = 0;
static char levels[7][70] = {
"  +---+\n\
  |   |\n\
      |\n\
      |\n\
      |\n\
      |\n\
=========",

"  +---+\n\
  |   |\n\
  O   |\n\
      |\n\
      |\n\
      |\n\
=========",
"  +---+\n\
  |   |\n\
  O   |\n\
  |   |\n\
      |\n\
      |\n\
=========",
"  +---+\n\
  |   |\n\
  O   |\n\
 /|   |\n\
      |\n\
      |\n\
=========",
"  +---+\n\
  |   |\n\
  O   |\n\
 /|\\  |\n\
      |\n\
      |\n\
=========",
"  +---+\n\
  |   |\n\
  O   |\n\
 /|\\  |\n\
 /    |\n\
      |\n\
=========",
"  +---+\n\
  |   |\n\
  O   |\n\
 /|\\  |\n\
 / \\  |\n\
      |\n\
========="
};


//Tengo que chequear que lo ingresador sea solo una letra
int letterCheck(char* c) {
    if (*(c+1)!=0)
        return MORE_THAN_1_LETTER;           //No metieron un unico ascii
    if (!IS_ALPHA(*c))
        return NOT_LETTER;          //El ascii ingresado no es una letra
    *c = toUpper(*c);
    
    for (int i=0; i< letterIndex;i+=2){     //De a dos ya que hay un guion entre cada letra
        if (*c==lettersUsed[i])
            return LETTER_REPEATED;           //La letra ingresada ya fue utilizada
    }
    return 0;
}

int inputLetter(char* shellBuffer) {
    if (gameEnded==1) {
        if (wrongLetters == MAX_FAILS)
            return GAME_LOST;
        else
            return GAME_COMPLETED;
    }
    int check= letterCheck(shellBuffer);  

    if (check!=0)
        return check;
    
    int i=0;
    int j=0;
    int found = 0;
    while(GUESSING_WORD[i] != 0){
        if (GUESSING_WORD[i]==*shellBuffer && userWord[j]=='_'){       //Si coincide una letra y estaba todavia desconocido la agrego
            userWord[j]=GUESSING_WORD[i];
            found = 1;
        }
        i++;
        j+=2;
    }           //Se van a llenar todos los espacion con la letra si coincide

    if (!found) {
        lettersUsed[letterIndex++]=*shellBuffer;
        lettersUsed[letterIndex++]='-';
        lettersUsed[letterIndex+1]=0;
        wrongLetters++;
    }

    if (wrongLetters == MAX_FAILS) {
        gameEnded = 1;
        return GAME_LOST;
    }
        

    char* auxPointer=userWord;
    while(*auxPointer){
        if (*auxPointer=='_'){
            return 0;
        }
        auxPointer++;
    }
    gameEnded=1;   
    return GAME_COMPLETED;
}

void solveHangman(){
    int i=0;
    int j=0;
    while(GUESSING_WORD[i]!=0){
        userWord[j]=GUESSING_WORD[i];
        i++;
        j += 2;
    }
    gameEnded = 1;
}

void initHangman() {
    letterIndex = gameEnded = 0;
    lettersUsed[0]=0;
    wrongLetters = 0;
    int i;
    for (i=0; GUESSING_WORD[i]; i++) {
        userWord[2*i]='_';
        userWord[2*i+1]=' ';
    }
    userWord[2*i-1]=0;
}

void printHangman() {
    char color = GREY;

    if (wrongLetters == MAX_FAILS)
        color = RED;
    else if (gameEnded)
        color = GREEN;

    printColor(levels[wrongLetters],color);
    putChar('\n');
    print(userWord);

    putChar('\n');
    print(lettersUsed);
    putChar('\n');
}




