#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "settings.h"
#include "abstract.h"
#include "gen.h"

/* Read in the BF file
 * MUST FREE THE RETURNED STRING
 */
char * readFile(Settings * settings, int * bfLen) {
  FILE * input = fopen(settings->inName, "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot find file named '%s'.\n", settings->inName);
    exit(1);
  }
  fseek(input, 0, SEEK_END);
  *bfLen = ftell(input);
  rewind(input);

  char * bfCode = malloc(*bfLen + 1);
  fread(bfCode, 1, *bfLen, input);
  bfCode[*bfLen] = '\0';

  fclose(input);
  return bfCode; 
}

/* Write program to C file */
void writeFile(Program * program) {
  FILE * file = fopen(program->settings->outName, "w");
  if (file == NULL) {
    fprintf(stderr, "Unable to create file '%s'.\n", program->settings->outName);
    exit(1);
  }
  fputs(HEAD, file);
  fwrite(program->code, 1, program->length, file);
  fputs(TAIL, file);
  fclose(file); 
}

/* Compile and run the C file */
void runFile(Settings * settings) {
  char buff[256];
  // Get executable name by cutting off '.c'
  char exeName[256];
  sprintf(exeName, settings->outName);
  int len = strlen(exeName);
  exeName[len - 2] = '\0';
  // Build and execute shell commands
  sprintf(buff, "gcc %s -o %s -O3", settings->outName, exeName);
  system(buff);
  sprintf(buff, "./%s", exeName);
  system(buff);
}

int main(int argc, char * argv[]) {
  initRules();

  Settings settings;
  getSettings(&settings, argc, argv);
  
  int bfLen;
  char * bfCode = readFile(&settings, &bfLen); 

  if (settings.verbose) {
    printf("%s: %d bytes\n", settings.inName, bfLen - 1);
    //printf(bfCode);
  }

  // Our output program, initialize to empty string
  Program program;
  initProgram(&program, &settings, DEFAULT_BUFFER_SIZE); 

  Abstract abstract;
  initAbstract(&abstract); 
  
  // THE COMPILATION/OPTIMIZATION PROCESS  
  // (1) Build abstract representation, begin to chunk code
  findRepeats(&abstract, bfCode, &settings);
  // (2) Apply transformation rules
  transform(&abstract, &settings);
  // (3) Generate C code
  generate(&program, &abstract);  

  if (settings.verbose) printf("Writing to '%s'.\n", settings.outName);  

  // Finally, write the program to the output file
  writeFile(&program); 
 
  if (settings.print) printf(program.code); 
  if (settings.run)   runFile(&settings);

  delete(&abstract);
  deleteRules();
 
  free(program.code);
  free(bfCode);
}
