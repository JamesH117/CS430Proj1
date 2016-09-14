all: main.c
	gcc main.c -o ppmrw
clean:
	rm -rf main *~