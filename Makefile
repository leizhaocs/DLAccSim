VPATH=./src/
OBJDIR=./obj/
EXEC=DLAccSim

CC=gcc
CPP=g++

COMMON=-Isrc/
CFLAGS=-O3
LDFLAGS=

SRC_CPP=$(notdir $(shell find src/ -type f -name '*.cpp'))
OBJ=$(SRC_CPP:.cpp=.o)

EXECOBJ=$(addprefix $(OBJDIR), $(OBJ))
DEPS=$(wildcard src/*) Makefile

all: obj $(EXEC)

$(EXEC): $(EXECOBJ)
	$(CPP) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CPP) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(EXEC) $(EXECOBJ) $(OBJDIR)/*
