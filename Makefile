build: 
	g++ -c main.cpp
	g++ main.o -o asteroids -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	rm *.o

run: 
	./asteroids
