
CC = g++
OBJECTS = main.o AntColony.o
FLAGS = -std=c++11

all: $(OBJECTS) main.o
	$(CC) $(FLAGS) $(OBJECTS) -o  tsp
	@echo
	@echo Usage: tsp [FILE] [OPTIONS]...
	@echo
	
%.o: %.cpp
	$(CC) -c $(FLAGS) $<

clean:
	rm -f $(OBJECTS) tsp
