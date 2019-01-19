#include "abstract.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct {
  Atom * x, * y;
  int xLength, yLength;
} Rule;

#define NUM_RULES 1
Rule rules[NUM_RULES];

/* Connect next/prev of an Atom array */
void connect(Atom * a[], int len) {
  for (int i = 0; i < len - 1; i++) {
    a[i]->next = a[i + 1];
    a[len - i - 1]->prev = a[len - i - 2];
  }
}

/* Construct a new rule given two Atom arrays x -> y */
void buildRule(Atom * x[], Atom * y[], int xl, int yl) {
  static int currentRule = 0;
  connect(x, xl);
  connect(y, yl);
  rules[currentRule].x = x[0];
  rules[currentRule].y = y[0];
  rules[currentRule].xLength = xl;
  rules[currentRule].yLength = yl;
  currentRule++;
}

void initRules() {
  buildRule((Atom*[]){newAtom('['), newRepeat('-', 1), newAtom(']')}, (Atom*[]){newAssign(0)}, 3, 1);
  /*Atom * a = newAtom('[');
  Atom * b = newRepeat('-', 1);
  Atom * c = newAtom(']');
  a->next = b;
  b->next = c;
  c->prev = b;
  b->prev = a;
  rules[0].x = a;
  rules[0].y = newAssign(0);
  rules[0].xLength = 3;
  rules[0].yLength = 1;*/ 
}


void deleteRules() {
  for (int i = 0; i < NUM_RULES; i++) {
    for (Atom * atom = rules[i].x; atom != NULL; atom = atom->next) {
      free(atom);
    }
    for (Atom * atom = rules[i].y; atom != NULL; atom = atom->next) {
      free(atom);
    } 
  }
}









/* Initialize an Abstract */
void initAbstract(Abstract * abstract) {
  abstract->front = NULL;
  abstract->back = NULL;
}

Atom * cpyAtom(Atom * atom) {
  return newAtom(atom->symbol);
}

char eqAtom(Atom * a, Atom * b) {  
  return ((a == NULL) && (b == NULL)) ||
         ((a != NULL) && (b != NULL) && (a->symbol == b->symbol));
}

Atom * newAtom(char symbol) {
  Atom * atom = malloc(sizeof(Atom));
  atom->symbol = symbol;
  atom->generate = genAtom;
  atom->equals = eqAtom;
  atom->copy = cpyAtom;
  atom->next = NULL;
  atom->prev = NULL;
  return atom;
}

Atom * cpyRepeat(Atom * atom) {
  return newRepeat(atom->symbol, ((Repeat *)atom)->times);
}

char eqRepeat(Atom * a, Atom * b) {
  return eqAtom(a, b) && ((Repeat *)a)->times == ((Repeat *)b)->times;
}


Atom * newRepeat(char symbol, int times) {
  Atom * atom = malloc(sizeof(Repeat));
  atom->symbol = symbol;
  atom->generate = genRepeat;
  atom->equals = eqRepeat;
  atom->copy = cpyRepeat;
  atom->next = NULL;
  atom->prev = NULL;
  ((Repeat *)atom)->times = times;
  return atom;
}

Atom * cpyAssign(Atom * atom) {
  return newAssign(((Assign *)atom)->value);
}

char eqAssign(Atom * a, Atom * b) {
  return eqAtom(a, b) && ((Assign *)a)->value == ((Assign *)b)->value;
}

Atom * newAssign(int value) {
  Atom * atom = malloc(sizeof(Assign));
  atom->symbol = 0;
  atom->generate = genAssign;
  atom->equals = eqAssign;
  atom->copy = cpyAssign;
  atom->next = NULL;
  atom->prev = NULL;
  ((Assign *)atom)->value = value;
  return atom;
}

/* Append a single Atom to an Abstract */
void append(Abstract * abstract, Atom * atom) {
  if (abstract->front == NULL) {
    abstract->front = abstract->back = atom;
  } else {
    Atom * oldBack = abstract->back;
    abstract->back->next = atom;
    abstract->back = atom;
    abstract->back->prev = oldBack;
  }
}

/* Appends a new Repeat atom to the Abstract given the code */
void appendRepeat(Abstract * abstract, char symbol, char ** code) {  
  Atom * atom = newRepeat(symbol, 0);
  while (**code == symbol) {
    ((Repeat *)atom)->times++;
    (*code)++;
  }
  append(abstract, atom);
}

/* Delete an Abstract and all of its Atoms */
void delete(Abstract * abstract) {
  for (Atom * atom = abstract->front; atom != NULL; atom = atom->next) {
    free(atom);
  }
}

/* Alternate repeat symbol, allows merging of +/- and </> */
char alternate(char symbol) {
  switch(symbol) {
    case '+': return '-';
    case '-': return '+';
    case '<': return '>';
    case '>': return '<';
  }
  return 0;
}

/* First pass .... */
void findRepeats(Abstract * abstract, char * bfCode, Settings * settings) {
  if (settings->verbose) 
    printf("\n===== Building Abstract =====\n");
  for (char c; (c = *bfCode) != 0;) {
    switch(c) {
      case '+': case '-': case '<': case '>': 
        appendRepeat(abstract, c, &bfCode);
        // As we go along, merge consecutive Repeat atoms,
        // Also merge alternate atoms like +/- and </>
        Atom * curr = abstract->back;
        Atom * prev = curr->prev;
        if (prev != NULL) { 
          char equSym = prev->symbol == c;
          char altSym = prev->symbol == alternate(c);
          if ((equSym || altSym)) {
            if (settings->verbose) {
               printf("Merging %d %c with %d %c\n", ((Repeat *)prev)->times, prev->symbol,
                                                     ((Repeat *)curr)->times, c);
            }
 
            if (equSym) {
              ((Repeat *) prev)->times += ((Repeat *) curr)->times;
            } else {
              ((Repeat *) prev)->times -= ((Repeat *) curr)->times;
            } 
            // Get rid of newest Repeat atom 
            prev->next = NULL;
            abstract->back = prev;
            free(curr);
            // If sum to 0 now, eliminate both actually.
            if (((Repeat *) prev)->times == 0) {
               abstract->back = prev->prev;
               if (prev->prev != NULL) {
                 prev->prev->next = NULL;
               } else {
                 abstract->front = NULL;
               }
               free(prev);
            }
          }
        }
      break;
      case '[': case ']': case '.': case ',':
        if (settings->verbose) printf("Appending %c\n", c, abstract->back);
        append(abstract, newAtom(c));
      default: bfCode++;
    }
  }
}


void transform(Abstract * abstract, Settings * settings) {
  if (settings->verbose)
    printf("\n==== Transformation Rules ====\n");
  for (Atom * atom = abstract->front; atom != NULL; atom = atom->next) {
    for (int rule = 0; rule < NUM_RULES; rule++) {
      // First compare this rule with the current node from the Abstract
      char alike = 1;
      Atom * currAtom = atom;
      Atom * ruleAtom = rules[rule].x;
      
      for (; (currAtom != NULL) && (ruleAtom != NULL); currAtom = currAtom->next, 
                                                       ruleAtom = ruleAtom->next) {
        if ((currAtom == NULL) != (ruleAtom == NULL) ||
            (!currAtom->equals(currAtom, ruleAtom))) {
          alike = 0;
          break;
        } 
      }
      
     
      // If they are a like, replace the nodes with copies of the transform
      if (alike) {
        printf("Matching rule #%d\n", rule);
        // Delete old nodes
        for (int i = 0; i < rules[rule].xLength - 1; i++) {
          free(atom->next);
	  atom->next = atom->next->next;
        }
        // We want to connect rule.y all the way to atom->next
        Atom * prev = atom->prev;
        Atom * end = atom->next;
        Atom * transformCurr = rules[rule].y;
        // Begin building new set of Atoms
        Atom * newCurr = transformCurr->copy(transformCurr);
        // Connect old chain to start of new chain
        prev->next = newCurr;
        newCurr->prev = prev;
        // All done with the old start
        free(atom);
        // Next, loop through 
        for (int i = 0; i < rules[rule].yLength - 1; i++) {
          // Copy over the next atom
          newCurr->next = transformCurr->next->copy(transformCurr->next);
          // Link up and advance
          newCurr->next->prev = newCurr;
          newCurr = newCurr->next;
          // The transform is already linked, just advance
          transformCurr = transformCurr->next;
        }
        // Finally, connect back to the old chain
        newCurr->next = end;
        if (end != NULL)
          end->prev = newCurr;
        // Take a step back in case we want to run this rule again
        // (May possible want to start all the way over in the future?)
        atom = prev;
      }
    }
  }
}
















