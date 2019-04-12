CC = g++
CPPFLAGS  = -Wall -O2 -g
INCLUDEFLAGS =
LDFLAGS = -lpthread
TARGETS = utest

SRCS = $(wildcard ./*.cpp)
OBJS = $(patsubst %.cpp,%.o, $(SRCS))

.PHONY: all clean

all: $(TARGETS)

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $(INCLUDEFLAGS) $< -o $@

$(TARGETS): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(DEPS)
	rm -f $(OBJS)
	rm -f $(TARGETS)
