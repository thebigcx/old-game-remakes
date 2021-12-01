SRCDIR := src
OBJDIR := obj
BINDIR := bin
BINNAME := snake
INCDIRS := -Isrc
CFLAGS := -O3 -Wall -g
LIBS := -lSDL2 -lSDL2_image

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
CC := gcc

$(BINDIR)/$(BINNAME): $(OBJECTS)
	mkdir -p $(BINDIR)
	gcc $^ -o $@ -g $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	gcc -c $< -o $@ -g $(INCDIRS) $(CFLAGS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY:
	clean
