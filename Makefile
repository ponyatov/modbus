MODULE = $(notdir $(CURDIR))

all: ./$(MODULE)
	LD_LIBRARY_PATH=/usr/local/lib ./$(MODULE)

C = $(MODULE).cpp
H = $(MODULE).hpp

L += -lmodbus

./$(MODULE): $(C) $(H) Makefile
	$(CXX) $(CXXFLAGS) -o $@ $(C) $(L)
