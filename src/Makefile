CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lpthread -lncurses -lboost_program_options -lboost_filesystem -lboost_system -lcurl
OBJ = Arguments.o GUI.o Semaphore.o URL.o

all: so_pro

so_pro: main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

run:
	./so_pro -n 4 -i urls.txt

clean:
	rm -f so_pro *.o
