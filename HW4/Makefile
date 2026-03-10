CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

TARGET = shell
SRC = shell.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	$(TARGET)
