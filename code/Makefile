include Makefile.inc

BUILDDIR = build
TARGET = $(BUILDDIR)/racer
LIBDIRS = Agents Engine Graphics Network Physics Parser Sound Utilities

C_SRCS   = $(wildcard */*.c)
CPP_SRCS = $(wildcard */*.cpp)

#OBJNAMES = i.e. Agents/agent.o
#OBJECTBUILDS  = i.e. build/Agents/agent.o
#OBJECTS = i.e. build/agent.o

# The strategy here is to build everything into build/$(some_lib_dir)/file.o,
# hard link that object file to build/file.o, and then link as normal. this will
# fail if someone tries to make an object file with the same name as one in
# another directory, but that shouldn't happen anyway.
#
OBJNAMES  = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)
OBJECTBUILDS = $(addprefix $(BUILDDIR)/, $(OBJNAMES))
OBJECTS = $(addprefix $(BUILDDIR)/, $(notdir $(OBJNAMES)))

all: $(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir -p $(addprefix $(BUILDDIR)/, $(LIBDIRS))

header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTBUILDS)

$(TARGET): $(OBJECTBUILDS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: test clean
test: all
	./$(TARGET)
clean:
	rm -rf $(BUILDDIR)


$(BUILDDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ && ln -f $@ $(BUILDDIR)/
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ && ln -f $@ $(BUILDDIR)/
$(BUILDDIR)%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
