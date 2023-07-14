# Makefile for RedVsBlue simulation
 
# *****************************************************
# Variables to control Makefile operation
 
CC = g++
CFLAGS = -Wall -g
 
# ****************************************************
# Targets needed to bring the executable up to date
 
redvsblue: battle_sim.o config.o armies_implementation.o utils.o snap_read.o
	$(CC) $(CFLAGS) -o redvsblue battle_sim.o config.o armies_implementation.o utils.o snap_read.o
 
 
battle_sim.o: battle_sim.cpp snap_reader.h utilities.h units_armies.h config.h
	$(CC) $(CFLAGS) -c battle_sim.cpp
 
config.o: config.cpp config.h utilities.h units_armies.h
	$(CC) $(CFLAGS) -c config.cpp
 
armies_implementation.o: armies_implementation.cpp units_armies.h utilities.h
	$(CC) $(CFLAGS) -c armies_implementation.cpp

utils.o: utils.cpp utilities.h
	$(CC) $(CFLAGS) -c utils.cpp
	
snap_read.o: snap_read.cpp snap_reader.h utilities.h units_armies.h
	$(CC) $(CFLAGS) -c snap_read.cpp
	
clean:
	rm -rf *.o