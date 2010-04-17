OS = $(shell uname -s)

CFLAGS = -Wall --std=gnu99 -ggdb
CXXFLAGS = -Wall -Wno-deprecated -ggdb

LDFLAGS = -lenet -lSDL -lode

ifeq ($(OS),Darwin)
  GLFLAGS = -framework GLUT -framework OpenGL -framework Foundation

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

