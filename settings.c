#include "settings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Default C program name */
const char * DEFAULT_OUT_NAME = "_prgm.c";

/* Initialize settings from arguments */
void getSettings(Settings * settings, int argc, char * argv[]) {
  settings->verbose = 0;
  settings->run = 0;
  settings->print = 0;
  settings->inName = NULL;
  settings->outName = NULL;
  for (int i = 1; i < argc; i++) {
    char * arg = argv[i];
    // Allow debug output
    if (!strcmp(arg, "-v")) { 
      settings->verbose = 1;
    } 
    // Compile and run program
    else if (!strcmp(arg, "-r")) {
      settings->run = 1;
    }
    // Print generated code
    else if (!strcmp(arg, "-p")) {
      settings->print = 1;
    }
    // Filename arguments
    else {
      int len = strlen(arg);
      // Set the file to compile
      if (len > 3 && arg[len - 1] == 'f' && arg[len - 2] == 'b' &&
          arg[len - 3] == '.') {
        if (settings->inName != NULL) {
          error("Already specified .bf file.");
        }
        settings->inName = arg;
      } 
      // Set the name of the output .c file 
      else if (len > 2 && arg[len - 1] == 'c' && arg[len - 2] == '.') {
        if (settings->outName != NULL) {
          error("Already specified .c file.");
        }
        settings->outName = arg;
      }
      else { 
        // TODO: Enable varargs for error(const char *, ...);
        fprintf(stderr, "Unknown argument '%s'.\n", arg);
        exit(1);
      } 
    }
  } 

  if (settings->inName == NULL) {
    error("Must specify a .bf file to compile.");
  }

  if (settings->outName == NULL) {
    settings->outName = (char *) DEFAULT_OUT_NAME;
  }
}

/* Print error to stderr and exit with exit code 1 */
void error(const char * message) {
  fprintf(stderr, message);
  fputc('\n', stderr);
  exit(1);
}


