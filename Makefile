wordcount: main.o
	gcc -o wordcount main.o

main.o: main.c
	gcc -c main.c

.PHONY: clean
clean:
	rm -rf wordcount *.o