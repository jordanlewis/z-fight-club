CXXFLAGS = -Wall -pedantic -g
CFLAGS   = -Wall -pedantic -std=c99 -g

TARGET = target

SRCS    = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

OBJECTS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $^

main: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

headers: $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: test clean pvars
pvars:
	@echo "SRCS:" $(SRCS)
	@echo "HEADERS:" $(HEADERS)
	@echo "OBJECTS:" $(OBJECTS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -r $(OBJECTS) $(TARGET) main headers
