# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall

# Name of the output executable
OUTPUT = circuit-analysis

# Source files
SRCS = main.cpp circuit.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) $(OBJS) -o $(OUTPUT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUTPUT)