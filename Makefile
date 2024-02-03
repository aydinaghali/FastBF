CXX=g++
MAKE=make
INCLUDE=-I/opt/homebrew/include
LIB=-L/opt/homebrew/lib
CPPFLAGS=-std=c++17 -g

LINK_LIB=

NAME=bf

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

.PHONY : clean build run all

all: build run

%.o: %.cpp %.h
	$(CXX) $(INCLUDE) -c $(CPPFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(INCLUDE) -c $(CPPFLAGS) $< -o $@

build: $(OBJS)
	$(CXX) $(OBJS) -o $(NAME) $(LIB) $(LINK_LIB) $(FRAMEWORKS)
	dsymutil $(NAME)

run:
	./$(NAME)

clean:
	rm -f $(NAME) $(OBJS)
	rm -rf $(NAME).dSYM

