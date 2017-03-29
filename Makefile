CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lpthread -lncurses -lboost_program_options -lboost_filesystem -lboost_system

all: so_pro


so_pro: main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

run:
	./so_pro -n 4 -i urls.txt


clean:
	rm -f so_pro
