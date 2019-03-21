DIR_SRC = ./src
DIR_INC = ./include
DIR_OBJ = ./build
DIR_BIN = ./bin
DIR_LIB = ./lib

SRC := $(wildcard $(DIR_SRC)/*.cpp)
OBJ := $(patsubst %.cpp, $(DIR_OBJ)/%.o, $(notdir $(SRC)))
LIB := $(wildcard $(DIR_LIB)/*.a)

TARGET = vm_protect

BIN_TARGET = $(DIR_BIN)/$(TARGET)

CC = clang++
CFLAGS = -std=gnu++11 -v -g -O0 -Wall -I$(DIR_INC)

$(BIN_TARGET):$(OBJ)
	$(CC) $(OBJ) $(LIB) -lcapstone -o $@

$(DIR_OBJ)/%.o:$(DIR_SRC)/%.cpp
	$(CC) $(CFLAGS) -lcapstone -c $< -o $@

.PHONY:clean
clean:
	find $(DIR_OBJ) -name *.o -exec rm -rf {}
