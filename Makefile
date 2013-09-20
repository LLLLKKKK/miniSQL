CC := g++
SRCDIR := src
test_SRCDIR := test
BUILDDIR := build
#CXXFLAGS := -std=c++0x -Wall -g3 -fno-inline -O0 -D_GLIBCXX_DEBUG
CXXFLAGS := -std=c++0x -Wall -O2
TARGET := test_main

SRCEXT := cpp

SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
test_SOURCES := $(shell find $(test_SRCDIR) -type f -name '*.$(SRCEXT)')

OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
OBJECTS += $(patsubst $(test_SRCDIR)/%,$(BUILDDIR)/test/%,$(test_SOURCES:.$(SRCEXT)=.o))

DEPS := $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(BUILDDIR)/$(TARGET)
##	valgrind -v --num-callers=20 --leak-check=yes --leak-resolution=high --show-reachable=yes $(BUILDDIR)/$(TARGET) --log_level=message
	$(BUILDDIR)/$(TARGET) --log_level=message

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) 
	@mkdir -p $(BUILDDIR)
	$(CC)  $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/test/%.o: $(test_SRCDIR)/%.$(SRCEXT) 
	@mkdir -p $(BUILDDIR)/test/
	$(CC)  $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(BUILDDIR)

-include $(DEPS)

.PHONY: clean
