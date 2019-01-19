#ifndef SETTINGS_H
#define SETTINGS_H

/* Represents the compiler settings */
typedef struct {
  // Allow debug output
  char verbose;   // = 0
  // Compile and run program
  char run;       // = 0
  // Print generated code
  char print;     // = 0
  char * inName;  // = NULL
  char * outName; // = "_prgm.c"
} Settings;

/* Initialize settings from arguments */
void getSettings(Settings * settings, int argc, char * argv[]);

/* Print error to stderr and exit with exit code 1 */
void error(const char * message);

#endif 
