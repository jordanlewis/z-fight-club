OS = $(shell uname -s)

ODEFLAGS = $(shell ode-config --cflags)
CFLAGS   = -Wall --std=gnu99 -ggdb $(ODEFLAGS) -I. -fsignaling-nans
CXXFLAGS = -Wall -Wno-long-long -Wno-deprecated -ggdb $(ODEFLAGS) -I. -fsignaling-nans

############ GPROF PROFILING: UNCOMMENT THESE **************
#CFLAGS   = -Wall --std=gnu99 -ggdb $(ODEFLAGS) -I. -pg
#CXXFLAGS = -Wall -Wno-long-long -Wno-deprecated -ggdb $(ODEFLAGS) -I. -pg

LIBRARIES = -lode -lboost_program_options-mt -lpng -lz -lstdc++
LDFLAGS =

ifeq ($(OS),Darwin)
  # Disable static linking in darwin for now, so that everything compiles
  STATIC_LINK_FLAGS = #-Wl,-bstatic
  DYNAMIC_LINK_FLAGS = #-Wl,-bdynamic
  GLFLAGS  = -framework GLUT -framework OpenGL -framework Foundation
  ALFLAGS  = -framework OpenAL
  SDLFLAGS = -lSDL -lSDLmain -framework Cocoa
  MATHFLAGS = 

  # Gets libraries downloaded by MacPorts
  CFLAGS   += -I/opt/local/include
  CXXFLAGS += -I/opt/local/include -Wno-non-virtual-dtor
  LDFLAGS  += -L/opt/local/lib

else
  STATIC_LINK_FLAGS = -Wl,-Bstatic
  DYNAMIC_LINK_FLAGS = -Wl,-Bdynamic
  GLFLAGS   = -lglut -lGL -lGLU
  ALFLAGS   = -lopenal
  SDLFLAGS  = -lSDL
  MATHFLAGS = -lm

  CFLAGS   += -pedantic
  CXXFLAGS += -pedantic
endif

LDFLAGS += $(STATIC_LINK_FLAGS) $(LIBRARIES) $(MATHFLAGS) $(DYNAMIC_LINK_FLAGS) $(GLFLAGS) $(ALFLAGS) $(SDLFLAGS) -lenet
