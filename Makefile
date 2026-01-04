CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -O3

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = nice.exe

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

