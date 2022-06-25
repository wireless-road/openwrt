all: cboot

CC = gcc
CFLAGS = -Wall
LDFLAGS = 

DEPS = $(wildcard *.h)

SRC = $(wildcard *.c)

OBJ = $(patsubst %.c, %.o, $(SRC))

%.o: %.c $(DEPS)
	@echo 
	@echo Compiling: $<	
	@$(CC) -c -o $@ $< $(CFLAGS)

cboot: $(OBJ)
	@echo 
	@echo Linking: $@	
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f cboot *.o

