#
# Creates draw1 executable.  'make clean' will rm all object files,
# the executable and a core file.
#
# Style is not what is expected for asn1 (e.g. no header in this
# comment), and this Makefile is not guaranteed to work with your
# code.  Feel free to use it as a template, though. 

pengda: pengda.o
	gcc -Wall -std=c99 -o pengda pengda.o -lm

pengda.o: pengda.h pengda.c
	gcc -Wall -std=c99 -c pengda.c -g

clean:
	-rm -f *.o pengda core

