project : project2.o queue.o Bank.o
	gcc -o server project2.o queue.o Bank.o
project2.o : project2.c project2.h
	gcc -c project2.c
queue.o: queue.c queue.h
	gcc -c queue.c
Bank.o: Bank.c Bank.h
	gcc -c Bank.c
