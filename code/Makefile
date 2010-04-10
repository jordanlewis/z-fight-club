CFLAGS = -Wall -std=c99
CXXFLAGS = -Wall

OBJECTS = matrix.o
HEADERS = $(wildcard *.h)
#PRECOMPILED_HEADERS = agent.gch collision.gch matrix.gch network.gch \
                       physics.gch quat.gch sound.gch vector.gch world.gch
PRECOMPILED_HEADERS = $(HEADERS:.h=.gch)

test: header_test
header_test: $(PRECOMPILED_HEADERS)
object_test: $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(PRECOMPILED_HEADERS) $(OBJECTS)

%.gch: %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
