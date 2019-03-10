DIR_SRC = ./src
DIR_INC = ./include
DIR_OBJ = ./build
DIR_BIN = ./bin

SRC := $(wildcard $(DIR_SRC)/*.cpp)
OBJ = $(patsubst %.cpp, $(DIR_OBJ)/%.o, $(notdir $(SRC)))

TARGET = vm_protect

BIN_TARGET = $(DIR_BIN)/$(TARGET)

CC = clang++
CFLAGS = -std=gnu++11 -v -g -Wall -I$(DIR_INC)

$(BIN_TARGET):$(OBJ)
	$(CC) $(OBJ) -o $@

$(DIR_OBJ)/%.o:$(DIR_SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	find $(DIR_OBJ) -name *.o -exec rm -rf {}
