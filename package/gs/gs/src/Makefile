all: gs test_modbus

CC = gcc
CFLAGS += -Wall -Wunused
LDFLAGS =

DEPS = $(wildcard *.h)

SRC = $(wildcard *.c)

OBJ = $(patsubst %.c, %.o, $(SRC))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gs: $(OBJ)
	$(CC) -o gs gs.o libazt.o buttons.o rk.o can.o settings.o in_4_20_ma.o libazt_ll.o libgs.o $(LDFLAGS)

test_modbus: $(OBJ)
	$(CC) -o test_modbus test_modbus.o libgs.o $(LDFLAGS)

.PHONY: clean

clean:
	rm -f gs test_modbus *.o
