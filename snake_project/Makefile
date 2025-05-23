snake: main.o map.o scoreboard.o snake.o item.o gate.o
	g++ -o snake main.o map.o scoreboard.o snake.o item.o gate.o -lncurses

item.o: item.cpp item.h
	g++ -c item.cpp



main.o: main.cpp map.h scoreboard.h
	g++ -c main.cpp

map.o: map.cpp map.h
	g++ -c map.cpp

scoreboard.o: scoreboard.cpp scoreboard.h
	g++ -c scoreboard.cpp

clean:
	rm -f *.o snake

snake.o: snake.cpp snake.h
	g++ -c snake.cpp

gate.o: gate.cpp gate.h
	g++ -c gate.cpp


