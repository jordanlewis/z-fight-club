TARGET = racer
OS = $(shell uname -s)

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation #FIXME this is out of date
else
  GLFLAGS = -lSDL -lm
endif

LDFLAGS = $(GLFLAGS) -lenet

CFLAGS = -Wall --std=gnu99 
CXXFLAGS = -Wall -pedantic 

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o))

HEADERS             = $(wildcard *.h)
PRECOMPILED_HEADERS = $(HEADERS:.h=.gch)


.PHONY: test

test: header_test object_test $(TARGET)
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

$(TARGET):      $(OBJECTS)
	g++ $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS) $(TARGET)

%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
