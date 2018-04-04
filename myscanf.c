#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_LENGTH 1024

typedef unsigned char *variables_list;
#define variables_start(list, param) (list = (((variables_list)&param) + sizeof(param)))
#define variables_arg(list, type)    (*(type *)((list += sizeof(type)) - sizeof(type)))
#define variables_end(list)          (list = NULL);
#define variables_input(list)        (list = malloc(BUFFER_LENGTH));

// -- myprintf.c
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
// -- end of myprintf.c

// Converts string to integer
// char *buffer is a C-string beginning with the representation of an integral number.
int custom_atoi(char *buffer, int base) {

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

void myscanf(char* text, ...) {

  char *input;    // Used to store raw string read from input
  int *concat;    // Used to store numerical values
  char **string;  // Used to store strings in a form of a char array

  // Initializes arguments for myscanf
  char* ptr;
  variables_start(ptr, text);
  variables_input(input);

  while (*text != '\0') {
    if (*text == '%') {
      text++;

      switch (*text) {
        case 'd': {
          concat = variables_arg(ptr, int*);

          read(0, input, BUFFER_LENGTH);
          *concat = custom_atoi(input, 10);
          break;
        }
        case 'b': {
          concat = variables_arg(ptr, int*);

          read(0, input, BUFFER_LENGTH);
          *concat = custom_atoi(input, 2);
          break;
        }
        case 'x': {
          concat = variables_arg(ptr, int*);

          read(0, input, BUFFER_LENGTH);
          *concat = custom_atoi(input, 16);
          break;
        }
        case 's': {
          string = variables_arg(ptr, char**);

          read(0, input, BUFFER_LENGTH);
          *string = input;
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

  myscanf("%d%b%x%s", &integer, &binary, &hexadecimal, &str);
  myprintf("%d, %b, %x, %s\n", integer, binary, hexadecimal, str);

  myscanf("%d", &integer);
  myprintf("Integer: %d\n", integer);

  myscanf("%b", &binary);
  myprintf("Binary: %b\n", binary);

  myscanf("%x", &hexadecimal);
  myprintf("Hexadecimal: %x\n", hexadecimal);

  myscanf("%s", &str);
  myprintf("String: %s\n", str);

  return 0;
}
