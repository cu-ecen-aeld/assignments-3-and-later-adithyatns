
CROSS:= $(CROSS_COMPILE)gcc
CC  = $(CROSS_COMPILE)gcc

# Add 'all' as the default target and point it to 'writer'
.PHONY: all
all: writer

writer: writer.c
	$(CC) writer.c -o writer

clean:
	rm writer
