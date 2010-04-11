OS = $(shell uname -s)

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation
else
  GLFLAGS = -lglut -lGL -lGLU -lm
endif

LDFLAGS = $(GLFLAGS) -L compiled_libraries/lib -lenet

CFLAGS = -Wall --std=gnu99 -I compiled_libraries/include
CXXFLAGS = -Wall -pedantic -I compiled_libraries/include

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o))

HEADERS             = $(wildcard *.h)
PRECOMPILED_HEADERS = $(HEADERS:.h=.gch)
LIBRARIES           = enet
#OBJECTS = matrix.o scheduler.o agent.o
#PRECOMPILED_HEADERS = agent.gch collision.gch matrix.gch network.gch \
                      physics.gch quat.gch scheduler.gch sound.gch vector.gch \
                      world.gch

.PHONY: clean test

test: libraries header_test
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)
libraries: $(LIBRARIES)

enet: 
	cd $(PWD)/packaged_libraries/enet-1.2.1; \
	./configure --prefix=$(PWD)/compiled_libraries; \
	make; make install

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS) -r compiled_libraries

%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
