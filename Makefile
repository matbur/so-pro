CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lpthread -lncurses -lboost_program_options -lboost_filesystem -lboost_system -lcurl
OBJ = URL.o Semaphore.o

all: so_pro


so_pro: main.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@


run:
	./so_pro -n 4 -i _urls.txt


clean:
	rm -f so_pro
