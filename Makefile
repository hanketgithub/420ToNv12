target  = 420ToNv12
sources = main.cpp $(target).cpp
objects = $(patsubst %.cpp,%.o,$(sources))
CC = g++
OPTS = -Wall -O2

all: $(objects)
	$(CC) $(OPTS) -o $(target) $(objects)

$(target).o: $(target).cpp
	$(CC) $(OPTS) -c $<

install:
	cp $(target) /usr/local/bin

clean:
	$(RM) $(target) $(objects)
