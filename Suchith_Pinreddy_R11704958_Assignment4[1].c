/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
This program is created to help us understand how a recursive descent parser for a grammar rule that was provided to us by our instructor. The Programming is inspired by the existing code of lexical Analyzer from the assignment and the parser code provided by the instructor.
BNF:
P::= S
S::= V=E | read(V) | print(V) | do{S} while C | S;S
C::= E < E | E > E | E == E | E <> E | E <= E | E >= E
E::= T | E + T | E - T
T::= F | T * F | T / F | T % F
F::= (E) | N | V
V::= a | b | ... | y | z | aV | bV | ... | yV | zV
N::= 0 | 1 | ... | 8 | 9 | 0N | 1N | ... | 8N | 9N
Name: Suchith Chandan Reddy Pinreddy

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
/* Token codes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 2
#define INT_LIT 3
#define IDENT 4
#define EQUAL_OP 5
#define ASSIGN_OP 6
#define ADD_OP 7
#define SUB_OP 8
#define MULT_OP 9
#define DIV_OP 10
#define LEFT_PAREN 11
#define RIGHT_PAREN 12
#define MOD_OP 13
#define LESSER_OP 14
#define GREATER_OP 15
#define NEQUAL_OP 16
#define LEQUAL_OP 17
#define GEQUAL_OP 18
#define SEMICOLON 19
#define LEFT_CBRACE 20
#define RIGHT_CBRACE 21
#define KEY_READ 22
#define KEY_PRINT 23
#define KEY_WHILE 24
#define KEY_DO 25

/* Global Variable */
int nextToken;
char array[26][200];
/* Local Variables */
static int charClass;
static char lexeme[100];
static char nextChar;
static int lexLen;
static FILE *in_fp;

/* Local Function declarations */
static void addChar();
static void getChar();
static void getNonBlank();
static void term();
static void expr();
static void factor();
static void ident();
static void int_literals();
static void stmts();
int lex();
static void error();



/*----------------------------------------------------------------------------------------------------------------------------------------------*/
/* main driver */
int main(int argc, char *argv[]) {
  /* Open the input data file and process its contents */
  printf("DanC Analyzer :: R11704958\n\n");
  if (argc > 1) { //Argc the number of arguments provided via command line.
    if ((in_fp = fopen(argv[1], "r")) == NULL) {
      printf("Error Encounter: The File that was provided as a command line argumen is not available in directory \n");
      exit(3);
    } else {
      getChar();
      do {
        lex();
        stmts(); 
      } while (nextToken != EOF);
      if (nextToken == EOF) {
        printf("Syntax Validated\n");
        exit(0);
      }
    }
  } else {
    printf("Error Encounter: The File was not provided as a command line argument\n");
    exit(2);
  }

  return 0;
}

/*-------------------------------------------------------------------------------
  FunctionName: lookup
  Purpose: a function to lookup operators and parentheses and return the token
  Type: Static Integer
  Parameters: Character
  -------------------------------------------------------------------------------*/
static int lookup(char ch) {
  //Lookups Characters like symbols such as < etc
  switch (ch) {
  case '=':
    addChar();
    getChar();
    if (nextChar == '=') {
      addChar();
      nextToken = EQUAL_OP;
    } else {
      ungetc(nextChar, in_fp);
      nextToken = ASSIGN_OP;
    }
    break;
  case '<':
    addChar();
    getChar();
    if (nextChar == '=') {
      addChar();
      nextToken = LEQUAL_OP;
    } else if (nextChar == '>') {
      addChar();
      nextToken = NEQUAL_OP;
    } else {
      ungetc(nextChar, in_fp);
      nextToken = LESSER_OP;
    }
    break;
  case '(':
    addChar();
    nextToken = LEFT_PAREN;
    break;
  case ')':
    addChar();
    nextToken = RIGHT_PAREN;
    break;
  case '+':
    addChar();
    nextToken = ADD_OP;
    break;
  case '-':
    addChar();
    nextToken = SUB_OP;
    break;
  case '*':
    addChar();
    nextToken = MULT_OP;
    break;
  case '/':
    addChar();
    nextToken = DIV_OP;
    break;
  case '>':
    addChar();
    getChar();
    if (nextChar == '=') {
      addChar();
      nextToken = GEQUAL_OP;
    } else {
      ungetc(nextChar, in_fp);
      nextToken = GREATER_OP;
    }
    break;
  case ';':
    addChar();
    nextToken = SEMICOLON;
    break;
  case '%':
    addChar();
    nextToken = MOD_OP;
    break;
  case '{':
    addChar();
    nextToken = LEFT_CBRACE;
    break;
  case '}':
    addChar();
    nextToken = RIGHT_CBRACE;
    break;
  default:
    addChar();
    nextToken = UNKNOWN;
    break;
  }
  return nextToken;
} /* End of function lookup */

/*-------------------------------------------------------------------------------
  FunctionName: addChar
  Purpose: a function to add nextChar to lexeme
  Type: Static Void
  Parameters: NONE
  -------------------------------------------------------------------------------*/
static void addChar() {
  //Combines the characters until space found
  if (lexLen <= 98) {
    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = 0;
  } else {
    printf("Error - lexeme is too long \n");
  }
}/* End of function addChar */

/*-----------------------------------------------------------------------------------------
  FunctionName: getChar
  Purpose: a function to get the next character of input and determine its character class
  Type: Static Void
  Parameters: NONE
  -------------------------------------------------------+----------------------------------*/
static void getChar() {
  //Retrieves character from source file
  if ((nextChar = getc(in_fp)) != EOF) {
    if (isalpha(nextChar)) {
      charClass = LETTER;
    } else if (isdigit(nextChar)) {
      charClass = DIGIT;
    } else {
      charClass = UNKNOWN;
    }
  } else {
    charClass = EOF;
  }
} /* End of function getChar */

/*-----------------------------------------------------------------------------------
  FunctionName: getNonBlank
  Purpose: a function to call getChar until it returns a non-whitespace character
  Type: Static Void
  Parameters: NONE
  -----------------------------------------------------------------------------------*/
static void getNonBlank() {
  while (isspace(nextChar))
    getChar();
} /* End of function getNonBlank */

/*-----------------------------------------------------------------------------------
  FunctionName: lex
  Purpose: a simple lexical analyzer for arithmetic expressions 
  Type: int
  Parameters: NONE
  -----------------------------------------------------------------------------------*/
int lex() {
  //Storage of Tokens based on the number of each token
  strcpy(array[0], "LETTER");
  strcpy(array[1], "DIGIT");
  strcpy(array[2], "UNKNOWN");
  strcpy(array[3], "INT_LIT");
  strcpy(array[4], "IDENT");
  strcpy(array[5], "EQUAL_OP");
  strcpy(array[6], "ASSIGN_OP");
  strcpy(array[7], "ADD_OP");
  strcpy(array[8], "SUB_OP");
  strcpy(array[9], "MULT_OP");
  strcpy(array[10], "DIV_OP");
  strcpy(array[11], "LEFT_PAREN");
  strcpy(array[12], "RIGHT_PAREN");
  strcpy(array[13], "MOD_OP");
  strcpy(array[14], "LESSER_OP");
  strcpy(array[15], "GREATER_OP");
  strcpy(array[16], "NEQUAL_OP");
  strcpy(array[17], "LEQUAL_OP");
  strcpy(array[18], "GEQUAL_OP");
  strcpy(array[19], "SEMICOLON");
  strcpy(array[20], "LEFT_CBRACE");
  strcpy(array[21], "RIGHT_CBRACE");
  strcpy(array[22], "KEY_READ");
  strcpy(array[23], "KEY_PRINT");
  strcpy(array[25], "KEY_DO");
  strcpy(array[24], "KEY_WHILE");
  lexLen = 0;
  getNonBlank();
  switch (charClass) {
  /* Parse identifiers */
  case LETTER:
    addChar();
    getChar();
    while (charClass == LETTER || charClass == DIGIT) {
      addChar();
      getChar();
    }
    if (strncmp(lexeme, "while", strlen(lexeme)) == 0) {
      nextToken = KEY_WHILE;
    } else if (strncmp(lexeme, "do", strlen(lexeme)) == 0) {
      nextToken = KEY_DO;
    } else if (strncmp(lexeme, "print", strlen(lexeme)) == 0) {
      nextToken = KEY_PRINT;
    } else if (strncmp(lexeme, "read", strlen(lexeme)) == 0) {
      nextToken = KEY_READ;
    } else {
      nextToken = IDENT;
    }
    break;

  /* Parse integer literals */
  case DIGIT:
    addChar();
    getChar();
    while (charClass == DIGIT) {
      addChar();
      getChar();
    }
    nextToken = INT_LIT;
    break;

  /* Parentheses and operators */
  case UNKNOWN:
    lookup(nextChar);
    getChar();
    break;

  /* EOF */
  case EOF:
    nextToken = EOF;
    lexeme[0] = 'E';
    lexeme[1] = 'O';
    lexeme[2] = 'F';
    lexeme[3] = 0;
    break;
  } /* End of switch */
  if (nextToken != -1) {
    //printf("%s\t\t%s\n", lexeme, array[nextToken]);
  }

  return nextToken;
} /* End of function lex */

/*--------------------------------------------------------------------------------
  FunctionName: conditions
  Purpose: Parse the C from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  --------------------------------------------------------------------------------*/

void conditions() {
  //Grammar Rule: E < E | E > E | E == E | E <> E | E <= E | E >= E
  expr();
  if (nextToken == LEQUAL_OP || nextToken == GEQUAL_OP ||
      nextToken == NEQUAL_OP || nextToken == EQUAL_OP ||
      nextToken == GREATER_OP || nextToken == LESSER_OP) { 
    lex();
    expr();
  }/* End of function conditions */
}
/*---------------------------------------------------------------------------------
  FunctionName: stmts
  Purpose: Parse the S from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  ---------------------------------------------------------------------------------*/
void stmts() {
  if (nextToken == KEY_DO) { //For do{S} while C rule
    lex();
    if (nextToken == LEFT_CBRACE) {
      lex();
      stmts();
      if (nextToken == RIGHT_CBRACE) {
        lex();
        if (nextToken == KEY_WHILE) {
          lex();
          conditions();
        } else {
          error();
        }
      } else {
        error();
      }
    } else {
      error();
    }
  }
  else if (nextToken == KEY_READ || nextToken == KEY_PRINT) { //For print(V) and read(V)
    lex();
    if (nextToken == LEFT_PAREN) {
      lex();
      ident();
      if (nextToken == RIGHT_PAREN) {
        lex();
      } else {
        error();
      }
    } else {
      error();
    }
  } else if (nextToken == IDENT) { //for V = E rule
    lex();
    if (nextToken == ASSIGN_OP) {
      lex();
      expr();
    } else {
      error();
    }
  }  else {
    error();
  }
  do {
    if (nextToken == SEMICOLON) {
      lex();
      if (nextToken == EOF) {
        error();
        break;
      } else {
        stmts(); //recursion
      }
    } else {
      return;
    }
  } while (nextToken != EOF); //loop to continue S;S rule
} /* End of function stmts */


/*--------------------------------------------------------------------------------
  FunctionName: expr
  Purpose: Parse the E from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  -------------------------------------------------------------------------------*/
void expr() {
  term(); //Grammar Rule: T 
  while (nextToken == ADD_OP || nextToken == SUB_OP) { //Grammar Rule: E + T | E - T
    lex();
    term();
  }
}/* End of function expr */


/*---------------------------------------------------------------------------------
  FunctionName: term
  Purpose: Parse the T from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  ---------------------------------------------------------------------------------*/
void term() {
  factor(); //Grammar Rule: F
  while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) { //Grammar Rule: T * F | T / F | T % F
    lex();
    factor();
  }
} /* End of function term */

/*----------------------------------------------------------------------------------
  FunctionName: factor
  Purpose: Parse the F from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  ----------------------------------------------------------------------------------*/
void factor() {
  //Grammar Rule: (E)
  if (nextToken == LEFT_PAREN) {
    lex();
    expr();
    if (nextToken == RIGHT_PAREN) {
      lex();
    } else {
      error();
    }
  } //Grammar Rule: N
    else if (nextToken == INT_LIT) {
    int_literals();
  } //Grammar Rule: V
    else if (nextToken == IDENT) {
    ident();
  } else {
    error();
  }
} /* End of function factor */


/*-------------------------------------------------------------------------------
  FunctionName: int_literals
  Purpose: Parse the N from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  -------------------------------------------------------------------------------*/
void int_literals() {
  //Grammar Rule: 0 | 1 | ... | 8 | 9 | 0N | 1N | ... | 8N | 9N
  if (nextToken == INT_LIT) {
    lex();
    int_literals();
  }
} /* End of function int_literals */

/*---------------------------------------------------------------------------------
  FunctionName: ident
  Purpose: Parse the V from the BNF grammar rule provided
  Type: void
  Parameters: NONE
  ---------------------------------------------------------------------------------*/
void ident() {
  //Grammar Rule: a | b | ... | y | z | aV | bV | ... | yV | zV
  if (nextToken == IDENT) {
    lex();
    ident();
  }
} /* End of function ident */

/*------------------------------------------------------------------------------------
  FunctionName: error
  Purpose: Displays error message with exit code of 1
  Type: void
  Parameters: NONE
  ------------------------------------------------------------------------------------*/
void error() {
  if (nextToken == EOF) {
    printf("Error encounter:  The next lexeme was %s and the next token was "
           "EOF \n",
           lexeme);
  } else {
    printf(
        "Error encounter:  The next lexeme was %s and the next token was %s \n",
        lexeme, array[nextToken]);
  }
  exit(1);
} /* End of function error */
