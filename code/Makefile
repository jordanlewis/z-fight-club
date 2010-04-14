TARGET = racer
OS = $(shell uname -s)

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation #FIXME this is out of date
else
  GLFLAGS = -lSDL -lm
endif

LDFLAGS = $(GLFLAGS) -lenet

CFLAGS = -Wall --std=gnu99 -ggdb
CXXFLAGS = -Wall -pedantic -ggdb

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o))

HEADERS             = $(wildcard *.h)
PRECOMPILED_HEADERS = $(HEADERS:.h=.gch)


.PHONY: test

all: $(TARGET)
test: $(TARGET)
	./$<
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS) $(TARGET)

%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
