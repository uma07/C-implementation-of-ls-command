# makefile for the assignment2 

main:		main.o error.o
		gcc -o main main.o error.o

main.o:		main.c	apue.h
		gcc -c main.c

error.o:	error.c	apue.h
		gcc -c error.c

clean:
		rm main.o error.o
