#ifndef ABSTRACT_H
#define ABSTRACT_H
#include "gen.h"
#include "settings.h"

/* An object representing a chunk of C code */
typedef struct Atom_ Atom;
//typedef struct Program Program;
struct Atom_ {
  char symbol;
  Atom * next, * prev;
  void (*generate)(Atom *, Program *);
  char (*equals)(Atom *, Atom *);
  Atom * (*copy)(Atom *);
};

/* A single chunk of C representing a repeated
 * sequence of BF code */
typedef struct {
  Atom super;
  int times;
} Repeat;

/* Represents an assignment to *ptr */
typedef struct {
  Atom super;
  int value;
} Assign;

/* Abstract representation of the original BF 
 * code as a sequence of Atoms */
struct Abstract_ {
  Atom * front, * back;
  int length;
  int capacity;
};

void initRules();
void deleteRules();

void initAbstract(Abstract *);

Atom * newAtom(char);
Atom * newRepeat(char, int times);
Atom * newAssign(int value);

void findRepeats(Abstract *, char * bfCode, Settings *);
void transform(Abstract *, Settings *);

void delete(Abstract *);

#endif
