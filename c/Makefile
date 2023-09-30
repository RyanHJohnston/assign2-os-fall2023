# Full compile command looks like this:
# gcc -Wall -g -pthread main.c lib.c -o main.o

CC = gcc
CFLAGS = -pthread -Wall -g
TARGET = main.o
SRC = main.c
LIB = lib.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIB)

run:
	./$(TARGET) 1000 

clean:
	rm -f $(TARGET) *.o
