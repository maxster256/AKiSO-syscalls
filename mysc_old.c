#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFSIZE 1024

typedef unsigned char *variables_list;
#define variables_start(list, param) (list = (((variables_list)&param) + sizeof(param)))
#define variables_arg(list, type)    (*(type *)((list += sizeof(type)) - sizeof(type)))
#define variables_end(list)          (list = NULL);

// Converts string to integer
// char *buffer   - C-string beginning with the representation of an integral number.

int myAtoi(char *buffer, int base) {

  int n = 0; // Initializes an integer holding the result of our calculations

	while (((*buffer != '\0') && (*buffer >= '0' && *buffer <= '9')) || (*buffer >= 'a' && *buffer <= 'f')) {

    if (*buffer >= 'a' && *buffer <= 'f')
      n = n * base + (*buffer - 'a' + 10);
    else
      n = n * base + (*buffer - '0');

    buffer++;
	}
	return n;
}

void myscanf(char* format, ...) {

  char *input;  // Used to iterate over string-to-print format provided by the user
  char *string = malloc(BUFSIZE);



  //Variables to put on them values of arguments.
  int* intArg;
  char** strArg = malloc(BUFSIZE);

  //Setting pointer to first argument - format.
  //It works like va_start macro.
  input = (char*) &format + sizeof(format);

  while (*format != '\0') {

    if (*format == '%') {
      format++;

      switch (*format) {
        case 'd': {
          intArg = (int*) (*(int*) input);
          input += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 10);
          break;
        }
        case 'b': {
          intArg = (int*) (*(int*) input);
          input += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 2);
          break;
        }
        case 'x': {
          intArg = (int*) (*(int*) input);
          input += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 16);
          break;
        }
        case 's': {
          strArg = (char**) (*(char**) input);
          input += sizeof(*strArg);
          read(0, string, BUFSIZE);
          *strArg = string;
          break;
        }
      }
    }
    format++;
  }
}




int main(){

  int integer, binary, hexadecimal;
  char* str;

  //printf("Let's provide an integer first: ");
  myscanf("%d", &integer);
  printf("Result: %d\n", integer);

  //printf("Let's provide a binary next: ");
  myscanf("%b", &binary);
  printf("Result: %d\n", binary);

  //printf("Let's provide a hex next: ");
  myscanf("%x", &hexadecimal);
  printf("Result: %x\n", hexadecimal);

  //printf("How about a string: ");
  myscanf("%s", &str);
  printf("Result: %s\n", str);

  //printf("All of them combined:");
  //myscanf("%d %b %x %s", &integer, &binary, &hexadecimal, &str);
  //printf("Result: %d, %b, %x, %s", integer, binary, hexadecimal, str);

  return 0;
}
