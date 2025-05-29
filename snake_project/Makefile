snake: main.o map.o scoreboard.o snake.o item.o gate.o
	g++ -std=c++17 -o snake main.o map.o scoreboard.o snake.o item.o gate.o -lncurses

item.o: item.cpp item.h
	g++ -std=c++17 -c item.cpp

main.o: main.cpp map.h scoreboard.h
	g++ -std=c++17 -c main.cpp

map.o: map.cpp map.h
	g++ -std=c++17 -c map.cpp

scoreboard.o: scoreboard.cpp scoreboard.h
	g++ -std=c++17 -c scoreboard.cpp

snake.o: snake.cpp snake.h
	g++ -std=c++17 -c snake.cpp

gate.o: gate.cpp gate.h
	g++ -std=c++17 -c gate.cpp

clean:
	rm -f *.o snake
