CC = gcc
CFLAGS = -std=c99 -Wall -ffast-math -O3
LDFLAGS =

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

OUTPUT = nov2017

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
