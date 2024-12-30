main: main.c
	gcc -Wall -Wextra -fPIC -c add.c sub.c mul.c div.c
	gcc -shared -o libcalc.so *.o
	gcc -Wall -Wextra -o main main.c -L. -lcalc -Wl,-rpath=.

clean:
	rm -f *.o libcalc.so main