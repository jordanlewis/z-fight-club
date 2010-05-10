CXXFLAGS = -Wall -pedantic -Wno-deprecated -g
CFLAGS   = -Wall -pedantic -Wno-deprecated -std=c99 -g
LDFLAGS = -lenet -lboost_program_options-mt

ARCH = $(shell uname -m)

ifeq ($(ARCH),x86_64)
  BUILDDIR = build64
else  
  BUILDDIR = build32
endif

TARGET = $(BUILDDIR)/target

$(TARGET): $(BUILDDIR)/main.o $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(BUILDDIR)/main.o: main.cpp $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: test clean pvars
pvars:
	@echo "SRCS:" $(SRCS)
	@echo "HEADERS:" $(HEADERS)
	@echo "OBJECTS:" $(OBJECTS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILDDIR)
