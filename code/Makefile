BUILDDIR = build
TARGET = $(BUILDDIR)/racer
LIBDIRS = Agents Engine Graphics Network Physics Parser Sound Utilities
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

C_SRCS   = $(wildcard *.c) $(wildcard Utilities/*.c)
CPP_SRCS =  $(wildcard Agents/*.cpp) $(wildcard Engine/*.cpp) $(wildcard Graphics/*.cpp) $(wildcard Physics/*.cpp) $(wildcard Network/*.cpp) $(wildcard Sound/*.cpp) $(wildcard Utilities/*.cpp)
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

$(OBJECTS): $(CPP_SRCS) $(C_SRCS)
	for D in $(LIBDIRS); do make -C $$D; done

$(LIBDIRS): $(wildcard $@/*)
	make -C $@

$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BUILDDIR)%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
