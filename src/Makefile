
TARGET =libnetco.so
CC =g++ -fPIC -g -O3 -Wall -std=c++11
INCLUDE =$(shell find ../include -name "*.h")
SOURCE =$(shell find ./ -name "*.cc")
OBJS =$(SOURCE:%.cc=%.o)
LIBS = -lpthread

$(TARGET):$(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.cc $(INCLUDE)
	$(CC) -c $< -o $@ $(LIBS)
	
all:$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
