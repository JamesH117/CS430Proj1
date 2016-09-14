all: main.c
	gcc -o ppmrw main.c

clean:
	rm -rf ppmrw *~

