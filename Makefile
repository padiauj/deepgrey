CC          = g++
CFLAGS      = -Wall -ansi -pedantic -O2
OBJS        = player.o board.o
PLAYERNAME  = deepgrey

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) wrapper.o
	$(CC) -o $@ $^

testgame: testgame.o
	$(CC) -o $@ $^

testminimax: $(OBJS) testminimax.o
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) -c $(CFLAGS) -x c++ $< -o $@
	
java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame testminimax
	
.PHONY: java testminimax
