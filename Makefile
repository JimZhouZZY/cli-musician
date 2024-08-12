# Define the compiler
CXX = g++

# Define the compiler flags
CXXFLAGS = -I/opt/homebrew/Cellar/portaudio/19.7.0/include -I/opt/homebrew/cellar/ncurses/6.5/include

# Define the linker flags
LDFLAGS = -L/opt/homebrew/Cellar/portaudio/19.7.0/lib -lportaudio -L/opt/homebrew/cellar/ncurses/6.5/lib -lncurses

# Define the target executable
TARGET = main

# Define the source files
SOURCES = main.cpp

# Define the object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

# Rule to build the object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

