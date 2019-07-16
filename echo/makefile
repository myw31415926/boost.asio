# boost echo makefile

CPP = g++
FLAGS = -g -Wall -I/usr/local/include 
LIBS = -L/usr/local/lib -lboost_system -lboost_thread -lpthread

TARGET := $(subst .cpp,,$(wildcard *.cpp))

all: $(subst .cpp,,$(wildcard *.cpp))

% : %.cpp
	$(CPP) -o $@ $^ $(FLAGS) $(LIBS)

.PHONY : clean
clean:
	rm -rf $(TARGET)
