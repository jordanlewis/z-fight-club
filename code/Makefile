BUILDDIR = build
TARGET = $(BUILDDIR)/racer
OS = $(shell uname -s)

CFLAGS = -Wall --std=gnu99 -ggdb
CXXFLAGS = -Wall -Wno-deprecated -ggdb

LDFLAGS = -lenet -lSDL -lode

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation #FIXME this is out of date

  # Gets libraries downloaded by MacPorts
  CFLAGS   += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include
  LDFLAGS  += -L/opt/local/lib

else
  GLFLAGS = -lglut -lGL -lGLU
  CFLAGS += -pedantic
  CXXFLAGS += -pedantic
  LDFLAGS += -lm
endif

C_SRCS   = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJECTS  = $(addprefix $(BUILDDIR)/, $(notdir $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)))


all: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir $@

.PHONY: test
test: all
	./$(TARGET)

header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

$(TARGET): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BUILDDIR)%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
