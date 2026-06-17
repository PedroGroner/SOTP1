CC = gcc
CFLAGS = -pthread -lm
TARGET = trabalho
SRC = main.c

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)