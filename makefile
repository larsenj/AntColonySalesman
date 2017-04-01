
CC = g++
OBJECTS = AntColony.o
FLAGS = -std=c++11 -pthread

all: $(OBJECTS) main.o
	$(CC) $(FLAGS) $(OBJECTS) main.o -o  tsp
	@echo
	@echo Usage: tsp [FILE] [OPTIONS]...
	@echo
	
test: $(OBJECTS) test.o

%.o: %.cpp
	$(CC) -c $(FLAGS) $<

clean:
	rm -f $(OBJECTS) test.o main.o tsp test
