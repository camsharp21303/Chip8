CC=g++

SRC_DIR=src
INCLUDE_DIR=include
OBJ_DIR=build

CFLAGS=-g -Wall -I $(INCLUDE_DIR) $$(pkg-config sdl2 --cflags) 

SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,$(SRCS))

all: $(OBJ_DIR) exe
exe: $(OBJS)
	$(CC) $(CFLAGS) $^ -o chip8 $$(pkg-config sdl2 --libs)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir build

clean:
	rm -rf $(OBJ_DIR)
	rm chip8
