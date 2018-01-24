main: main.o threadpool.o queue.o
	gcc -o main main.o threadpool.o queue.o -lm -lpthread
main.o: main.c threadpool.c queue.c taskfeeder.h queue.h threadpool.h
	gcc -c -w -g main.c threadpool.c queue.c -lm -lpthread 

clean:
	rm *.o main



