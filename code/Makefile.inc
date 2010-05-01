OS = $(shell uname -s)

ODEFLAGS = $(shell ode-config --cflags)
CFLAGS   = -Wall --std=gnu99 -ggdb $(ODEFLAGS) -I.
CXXFLAGS = -Wall -Wno-deprecated -ggdb $(ODEFLAGS) -I.

LDFLAGS  = -lenet -lSDL -lode -lboost_program_options-mt -lopenal

ifeq ($(OS),Darwin)
  GLFLAGS  = -framework GLUT -framework OpenGL -framework Foundation
  SDLFLAGS = -lSDLmain -framework Cocoa

  # Gets libraries downloaded by MacPorts
  CFLAGS   += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include/openal
  LDFLAGS  += -L/opt/local/lib

else
  GLFLAGS   = -lglut -lGL -lGLU
  SDLFLAGS  =

  CFLAGS   += -pedantic
  CXXFLAGS += -pedantic -I/usr/include/AL
  LDFLAGS  += -lm
endif

LDFLAGS += $(GLFLAGS) $(SDLFLAGS)
