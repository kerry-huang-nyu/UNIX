CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

TARGET = du
SRC = du.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	$(TARGET)
