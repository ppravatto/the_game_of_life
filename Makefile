CPPC=g++
FLAGS= -std=c++11 -lsfml-graphics -lsfml-window -lsfml-system -O4

all: main.cpp
	$(CPPC) main.cpp -o main.exe $(FLAGS)

clear:
	rm -rf main.exe