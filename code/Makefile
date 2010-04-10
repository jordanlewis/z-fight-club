OS = $(shell uname -s)

ifeq ($(OS),Darwin)
  LDFLAGS = -framework GLUT -framework OpenGL -framework Foundation
else
  LDFLAGS = -lglut -lGL -lGLU -lm
endif

CFLAGS = -Wall --std=gnu99
CXXFLAGS = -Wall -pedantic

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o))

HEADERS             = $(wildcard *.h)
PRECOMPILED_HEADERS = $(HEADERS:.h=.gch)
#OBJECTS = matrix.o scheduler.o agent.o
#PRECOMPILED_HEADERS = agent.gch collision.gch matrix.gch network.gch \
                      physics.gch quat.gch scheduler.gch sound.gch vector.gch \
                      world.gch

.PHONY: clean test

test: header_test
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS)

%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
