PRECOMPILED_HEADERS = agent.gch collision.gch matrix.gch network.gch \
                      physics.gch quat.gch sound.gch vector.gch world.gch

header_test: $(PRECOMPILED_HEADERS)

clean:
	rm $(PRECOMPILED_HEADERS)

%.gch: %.h
	$(CXX) -Wall -c $< -o $@
