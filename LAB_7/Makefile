main: main.c
	gcc -Wall -Wextra -c add.c sub.c mul.c div.c
	ar rcs libcalc.a *.o
	gcc -Wall -Wextra -o main main.c libcalc.a

clean:
	rm -f *.o libcalc.a main