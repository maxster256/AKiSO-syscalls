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

// myprintf.c
// used here for the proof that the code works!
char *convert(unsigned int num, int base) {
	static char Representation[] = "0123456789ABCDEF";
	static char buffer[50];
	char *ptr;

	ptr = &buffer[49];
	*ptr = '\0';

	do {
		*--ptr = Representation[num%base];
		num /= base;
	}
  while(num != 0);

	return(ptr);
}

void myprintf(char *format, ...) {

  char *output;         // Used to iterate over string-to-print format provided by the user
  int concat;  // Used to store integer values provided by the user
  char *string;         // Used to store strings provided by the user

  // Initalizes arguments for myprintf
  variables_list arg;
  variables_start(arg, format);

  for (output = format; *output != '\0'; output++) {

    if (*output == '%') {
      output++;

      if (*output == 'c') {
        // Puts char into the placeholder
        concat = variables_arg(arg, int);

				write(1, (void *)concat, sizeof(concat));
      }
      else if (*output == 'd') {
        // Puts integer into placeholder
        concat = variables_arg(arg, int);

        if (concat < 0) {
          concat = -concat;

					write(1, "-", sizeof(int));
        }
				write(1, (void *)convert(concat, 10), sizeof(concat));

      }
      else if (*output == 'b') {
        // Puts binary representation
        concat = variables_arg(arg, unsigned int);

				write(1, (void *)convert(concat, 2), sizeof(concat));
      }
      else if (*output == 's') {
        // Puts string
        string = variables_arg(arg, char *);

				while (*string != '\0') {
					write(1, string, sizeof(*string));
					string++;
				}
      }
      else if (*output == 'x') {
        // Puts hexadecimal number

        concat = variables_arg(arg, unsigned int);
				write(1, (void *)convert(concat, 16), sizeof(concat));
      }
    }
    else {
      // While there's no indication of the need to add some argument
			write(1, (void *)output, sizeof(*output));
    }
  }
  variables_end(arg);
}
// end of myprintf.c

int myAtoi(char *buffer, int base) {

  int n = 0; // Initializes an integer holding the result of our calculations

	while ((*buffer != '\0' && (*buffer >= '0' && *buffer <= '9')) || (*buffer >= 'a' && *buffer <= 'f')) {

    if (*buffer >= 'a' && *buffer <= 'f')
      n = n * base + (*buffer - 'a' + 10);
    else
      n = n * base + (*buffer - '0');

    buffer++;
	}
	return n;
}

void myScanf(char* text, ...) {
  char* string = malloc(BUFSIZE);

  //Pointer to arguments of myPrintf.
  char* ptr;

  //Variables to put on them values of arguments.
  int* intArg;
  char** strArg = malloc(BUFSIZE);

  //Setting pointer to first argument - text.
  //It works like va_start macro.
  ptr = (char*) &text + sizeof(text);

  while (*text != '\0') {
    if (*text == '%') {
      text++;
      switch (*text) {
        case 'd': {
          intArg = (int*) (*(int*) ptr);
          ptr += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 10);
          break;
        }
        case 'b': {
          intArg = (int*) (*(int*) ptr);
          ptr += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 2);
          break;
        }
        case 'x': {
          intArg = (int*) (*(int*) ptr);
          ptr += sizeof(int*);
          read(0, string, BUFSIZE);
          *intArg = myAtoi(string, 16);
          break;
        }
        case 's': {
          strArg = (char**) (*(char**) ptr);
          ptr += sizeof(*strArg);
          read(0, string, BUFSIZE);
          *strArg = string;
          break;
        }
      }
    }
    text++;
  }
}

int main(){

  int integer, binary, hexadecimal;
  char* str;

  myScanf("%d", &integer);
  myprintf("Integer: %d\n", integer);

  myScanf("%b", &binary);
  myprintf("Binary: %b\n", binary);

  myScanf("%x", &hexadecimal);
  myprintf("Hexadecimal: %x\n", hexadecimal);

  myScanf("%s", &str);
  myprintf("String: %s\n", str);

  return 0;
}
