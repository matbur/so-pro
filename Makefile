CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lpthread -lncurses -lboost_program_options -lboost_filesystem -lboost_system -lcurl
OBJ = src/Arguments.o src/GUI.o src/Semaphore.o src/URL.o

all: so_pro

so_pro: src/main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

run:
	./so_pro -n 4 -i urls.txt

clean:
	rm -f so_pro src/*.o
