MODULE = $(notdir $(CURDIR))

$(MODULE).log: ./$(MODULE)
	./$(MODULE) > $@ && tail $(TAIL) $@

C = $(MODULE).cpp
H = $(MODULE).hpp

./$(MODULE): $(C) $(H)
	$(CXX) $(CXXFLAGS) -o $@ $(C)

