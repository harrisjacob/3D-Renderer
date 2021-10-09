CC := g++
CFLAGS := -g -Wall
SRCDIR := src
BUILDDIR := build
TARGET := bin/runner
LD := g++
LDFLAGS := -L
INC := -I include

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))

$(TARGET) : $(OBJECTS) 
	@echo " Linking..."
	$(CC) -o $@ $^ $(CFLAGS) $(INC)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) -c -o $@ $^ $(CFLAGS) $(INC)

clean:
	@echo " Cleaning..."
	@rm -r -f $(BUILDDIR) $(TARGET)

.PHONY: clean

