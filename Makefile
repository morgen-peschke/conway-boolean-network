SHELL := /bin/bash

.SUFFIXES:
.SUFFIXES: .cpp

.cpp:
	g++ -o $* $*.cpp

all: conway boolnet generateRules rules

conway: conway.cpp
boolnet: boolnet.cpp

debug: conway.cpp
	g++ -g -o conway conway.cpp
	gdb ./conway

generateRules: generateRules.cpp
rules: generateRules

clean:
	cleanout
	rcsclean
	co animate.pl randomConway

clean.all:
	cleanout	
	rm conway generateRules boolnet bitsToNum
	rcsclean
