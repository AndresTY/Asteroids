CXX = g++
CXXFLAGS = -I/opt/sfml2/include -std=c++17
LDFLAGS = -L/opt/sfml2/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

build:
	$(CXX) -c main.cpp $(CXXFLAGS)
	$(CXX) main.o -o asteroids $(LDFLAGS)
	rm main.o

run:
	export LD_LIBRARY_PATH=/opt/sfml2/lib:$LD_LIBRARY_PATH
	./asteroids

