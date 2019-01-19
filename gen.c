#include "gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "abstract.h"

const int DEFAULT_BUFFER_SIZE = 256;

/* Begins an output C program */
const char * HEAD = "#include <stdio.h>\n"
                    "#include <stdlib.h>\n"
                    "#include <string.h>\n"
                    "unsigned tapeSize;\n"
                    "unsigned * tape;\n"
                    "unsigned * ptr;\n" 
                    "const size_t ELEM_SIZE = sizeof(*ptr);\n"
                    "void moveLeft(int steps) {\n"
                    "  while (ptr - steps < tape) {\n"
                    "    int offset = tape - ptr;\n"
                    "    int * newTape = calloc(tapeSize * 2, ELEM_SIZE);\n"
                    "    memset(newTape, 0, tapeSize * ELEM_SIZE);\n"
                    "    memcpy(newTape + tapeSize, tape, tapeSize * ELEM_SIZE);\n"
                    "    free(tape);\n"
                    "    tape = newTape;\n"
                    "    ptr = tape + tapeSize + offset;\n"
                    "    tapeSize *= 2;\n" 
                    "  }\n"
                    "  ptr -= steps;\n"
                    "}\n"
                    "void moveRight(int steps) {\n"
                    "  ptr += steps;\n"
                    "  while (ptr >= tape + tapeSize) {\n"
                    "    int overshoot = ptr - (tape + tapeSize);\n"
                    "    int * newTape = calloc(tapeSize * 2, ELEM_SIZE);\n"
                    "    memset(newTape + tapeSize, 0, tapeSize * ELEM_SIZE);\n"
                    "    memcpy(newTape, tape, tapeSize * ELEM_SIZE);\n"
                    "    free(tape);\n"
                    "    tape = newTape;\n"
                    "    ptr = tape + tapeSize + overshoot;\n"
                    "    tapeSize *= 2;\n" 
                    "  }\n"
                    "}\n"
                    "int main() {\n"
                    "  tapeSize = 256;\n"
                    "  tape = calloc(tapeSize, ELEM_SIZE);\n"
                    "  ptr = tape;\n"
                    "  memset(tape, 0, tapeSize);\n";
/* Finishes an output C program */
const char * TAIL = "  free(tape);\n"
                    "  return 0;\n"
                    "}\n";

const char * INCREMENT = "++*ptr;";
const char * DECREMENT = "--*ptr;";
const char * MOVE_LEFT = "moveLeft(1);";
const char * MOVE_RGHT = "moveRight(1);";
const char * WHILE_NZR = "while(*ptr) {";
const char * END_WHILE = "}";
const char * PRNT_CHAR = "putchar(*ptr);";
const char * READ_CHAR = "*ptr = getchar();";

const char * INC_TEMPL = "*ptr += %d;";
const char * DEC_TEMPL = "*ptr -= %d;";
const char * MVL_TEMPL = "moveLeft(%d);";
const char * MVR_TEMPL = "moveRight(%d);";

const char * ASN_TEMPL = "*ptr = %d;";



void initProgram(Program * program, Settings * settings, int capacity) {
  program->code = malloc(capacity);
  program->length = 0;
  program->code[0] = '\0';
  program->capacity = capacity;
  program->depth = 2;
  program->settings = settings;
}

/* Resizes a program string if needed */
void tryResize(Program * program) {
  assert(program->length <= program->capacity);
  if (program->length == program->capacity) {
    char * newCode = malloc(program->capacity * 2);
    memcpy(newCode, program->code, program->capacity);
    free(program->code);
    program->code = newCode;
    program->capacity *= 2;
  }
}

/* Emit a single character to the program */
void emitc(Program * program, char c) {
  tryResize(program);
  program->code[program->length++] = c;
  // if (program->settings->verbose) {
  //   putchar(c);
  // }
}

/* Emit text to the given C program */
void emit(Program * program, const char * text) {
  while (*text != 0)
    emitc(program, *(text++));
}


/* Emit a given number of spaces to the program */
void emitSpaces(Program * program, int spaces) {
  while(spaces--)
    emitc(program, ' ');
}

/* Emit code specifically, and finish a comment */
void emitCode(Program * program, const char * code) {
  emitSpaces(program, program->depth);
  emit(program, code);
}

void genAtom(Atom * atom, Program * program) {
  switch(atom->symbol) {
    case '[': emitCode(program, WHILE_NZR); program->depth += 2; break;
    case ']': program->depth -= 2; emitCode(program, END_WHILE); break;
    case '.': emitCode(program, PRNT_CHAR); break;
    case ',': emitCode(program, READ_CHAR); break;
    default: printf("%d\n", atom->symbol);error("Incorrect symbol.");
  }
}

void genRepeat(Atom * atom, Program * program) {
  int times = ((Repeat *) atom)->times;
  // Just emit ++/--
  if (times == 1) {
    switch (atom->symbol) {
      case '+': emitCode(program, INCREMENT); break;
      case '-': emitCode(program, DECREMENT); break;
      case '<': emitCode(program, MOVE_LEFT); break;
      case '>': emitCode(program, MOVE_RGHT); break;
      default: printf("%d\n", atom->symbol); error("Incorrect symbol.");
    }
  } 
  // Emit +=/-= %d
  else {
    char buff[256];
    const char * template;
    switch(atom->symbol) {
      case '+': template = INC_TEMPL; break;
      case '-': template = DEC_TEMPL; break;
      case '<': template = MVL_TEMPL; break;
      case '>': template = MVR_TEMPL; break;
      default: printf("%d\n", atom->symbol); error("Incorrect symbol."); 
    }
    sprintf(buff, template, times);
    emitCode(program, buff);
  }
}

void genAssign(Atom * atom, Program * program) {
  assert(atom->symbol == 0);
  char buff[256];
  sprintf(buff, ASN_TEMPL, ((Assign *) atom)->value);
  emitCode(program, buff);
}

void generate(Program * program, Abstract * abstract) {
  if (program->settings->verbose) printf("\n====== Generating  Code ======\n");
  for (Atom * atom = abstract->front; atom != NULL; atom = atom->next) {
    atom->generate(atom, program);
    emitc(program, '\n');
  }
} 

