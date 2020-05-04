ncm: main.cpp
	g++ -Wall -Wextra main.cpp -o ncm -lncurses

run: ncm
	./ncm

.PHONY: run
