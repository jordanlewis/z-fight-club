OS = $(shell uname -s)

ODEFLAGS = $(shell ode-config --cflags)
CFLAGS   = -Wall --std=gnu99 -ggdb $(ODEFLAGS) -I.
CXXFLAGS = -Wall -Wno-long-long -Wno-deprecated -ggdb $(ODEFLAGS) -I.

LDFLAGS  = -lenet -lSDL -lode -lboost_program_options-mt

ifeq ($(OS),Darwin)
  GLFLAGS  = -framework GLUT -framework OpenGL -framework Foundation
  ALFLAGS  = -framework OpenAL
  SDLFLAGS = -lSDLmain -framework Cocoa

  # Gets libraries downloaded by MacPorts
  CFLAGS   += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include
  LDFLAGS  += -L/opt/local/lib

else
  GLFLAGS   = -lglut -lGL -lGLU
  ALFLAGS   = -lopenal
  SDLFLAGS  =

  CFLAGS   += -pedantic
  CXXFLAGS += -pedantic
  LDFLAGS  += -lm
endif

LDFLAGS += $(GLFLAGS) $(ALFLAGS) $(SDLFLAGS)
