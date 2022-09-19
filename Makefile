CC := clang
DIR_SRC := src

CFLAGS :=

TARGET := main

CSRCS := $(shell find $(DIR_SRC) -type f -name "*.c")
COBJS := $(patsubst %.c, %.o, $(CSRCS))

.PHONY: all clean run

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(COBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(COBJS)


