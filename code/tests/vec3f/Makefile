CXXFLAGS = -Wall -pedantic -g -I../../Utilities/
CFLAGS   = -Wall -pedantic -std=c99 -g
LDFLAGS = -L../../build -lm

TARGET = target

$(TARGET): ../../build/vec3f.o main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

../../build/vec3f.o:
	make -C ../../Utilities ../build/vec3f.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^


.PHONY: test clean pvars
pvars:
	@echo "SRCS:" $(SRCS)
	@echo "HEADERS:" $(HEADERS)
	@echo "OBJECTS:" $(OBJECTS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf main.o $(TARGET)
