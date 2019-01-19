#ifndef GEN_H
#define GEN_H
#include "settings.h"

extern const int DEFAULT_BUFFER_SIZE;

extern const char * HEAD, * TAIL;

/* Direct translations of BF code to C code */
extern const char * INCREMENT, * DECREMENT, * MOVE_LEFT, * MOVE_RGHT;
extern const char * WHILE_NZR, * END_WHILE, * PRNT_CHAR, * READ_CHAR;
extern const char * INC_TEMPL, * DEC_TEMPL, * MVL_TEMPL, * MVR_TEMPL;
/* Abstract translations from BF to C */
extern const char * ASN_TEMPL;

typedef struct {
  char * code;
  int length;
  int capacity;
  // Number of nested loops
  int depth;
  // Pass the settings around
  Settings * settings;
} Program;

void initProgram(Program *, Settings *, int capacity);

typedef struct Atom_ Atom;
void genAtom(Atom * atom, Program * program);
void genRepeat(Atom *, Program *);
void genAssign(Atom *, Program *);

typedef struct Abstract_ Abstract;
void generate(Program *, Abstract *);


 
#endif
