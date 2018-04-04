/*
 * Excercise 1.
 * Write own printf, scanf without using libraries and va_start, va_arg, va_end macros
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef unsigned char *variables_list;
#define variables_start(list, param) (list = (((variables_list)&param) + sizeof(param)))
#define variables_arg(list, type)    (*(type *)((list += sizeof(type)) - sizeof(type)))
#define variables_end(list)          (list = NULL);

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
  unsigned int concat;  // Used to store integer values provided by the user
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


int main(int argc, char *argv[]) {

  myprintf("String: %s\nDecimal: %d\nHexadecimal: %x\nBinary: %b\n", "pawel.narolski@gmail.com", 36, 0xAB, 0b101);

  return 0;
}
