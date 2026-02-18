CXX = g++

CXXFLAGS = -std=c++17 -Wall -Irapidjson/include

LIBS = -lcurl

TARGET = graph_crawler

SRC = graph_crawler.cpp

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
