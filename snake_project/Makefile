snake: main.o map.o scoreboard.o snake.o item.o gate.o
	g++ -std=c++11 -o snake main.o map.o scoreboard.o snake.o item.o gate.o -lncurses

main.o: main.cpp map.h scoreboard.h
	g++ -std=c++11 -c main.cpp

map.o: map.cpp map.h
	g++ -std=c++11 -c map.cpp

scoreboard.o: scoreboard.cpp scoreboard.h
	g++ -std=c++11 -c scoreboard.cpp

snake.o: snake.cpp snake.h
	g++ -std=c++11 -c snake.cpp

item.o: item.cpp item.h
	g++ -std=c++11 -c item.cpp

gate.o: gate.cpp gate.h
	g++ -std=c++11 -c gate.cpp

clean:
	rm -f *.o snake
