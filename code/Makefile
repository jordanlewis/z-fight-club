BUILDDIR = build
TARGET = $(BUILDDIR)/racer
OS = $(shell uname -s)

CFLAGS = -Wall --std=gnu99 -ggdb
CXXFLAGS = -Wall -ggdb

LDFLAGS = 

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation #FIXME this is out of date
  
  CFLAGS   += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include

  LDFLAGS  += -L/opt/local/lib
else
  GLFLAGS = -lm
endif

LDFLAGS += $(GLFLAGS) -lenet -lSDL -lode

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(BUILDDIR)/, $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)))


all: game

.PHONY: test

all: $(BUILDDIR) $(TARGET)
	echo "LDFLAGS: $(LDFLAGS)"

$(BUILDDIR):
	mkdir $@

test: all
	./$(TARGET)
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS) game

#	rm -f$(PRECOMPILED_HEADERS) $(OBJECTS) $(TARGET)

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BUILDDIR)%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

game: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
