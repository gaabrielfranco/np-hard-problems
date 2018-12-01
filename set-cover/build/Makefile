FLAGS= -std=c++17 -O3
set-cover: main.o
	g++ $(FLAGS) main.o -o set-cover

main.o: ../main.cpp
	g++ $(FLAGS) -c ../main.cpp

clean:
	find -not -name '.' -not -name '.gitignore' -not -name 'Makefile' -delete