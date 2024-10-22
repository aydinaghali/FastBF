CXX=g++
MAKE=make
INCLUDE=-I/opt/homebrew/include
LIB=-L/opt/homebrew/lib
LINK_LIB=
NAME=bf
CPPFLAGS=-std=c++17
DEBUG_CPPFLAGS=-g
RELEASE_CPPFLAGS=-O3

PROFILE?=DEBUG

ifeq ($(PROFILE), DEBUG)
	CPPFLAGS+=$(DEBUG_CPPFLAGS)
else ifeq ($(PROFILE), RELEASE)
	CPPFLAGS+=$(RELEASE_CPPFLAGS)
endif

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

.PHONY : clean build run all

all: build run

%.o: %.cpp %.h
	$(CXX) $(INCLUDE) -c $(CPPFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(INCLUDE) -c $(CPPFLAGS) $< -o $@

build: $(OBJS)
	$(CXX) $(OBJS) -o $(NAME) $(LIB) $(LINK_LIB)
ifeq ($(PROFILE), DEBUG)
	dsymutil $(NAME)
endif

run:
	./$(NAME)

clean:
	rm -f $(NAME) $(OBJS)
	rm -rf $(NAME).dSYM

