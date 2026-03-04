
CROSS:= $(CROSS_COMPILE)gcc

# Add 'all' as the default target and point it to 'writer'
.PHONY: all
all: writer

writer: writer.c
	$(CROSS) writer.c -o writer

clean:
	rm writer
