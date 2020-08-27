
TARGET =netco_test
CC =g++ -L../src -g -O3 -Wall -std=c++11
INCLUDE =$(shell find ../include -name "*.h")
SOURCE =$(shell find ./ -name "*.cc")
OBJS =$(SOURCE:%.cc=%.o)
LIBS =-lnetco
CFLAGS =-I../src

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.cc $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
	
all:$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
