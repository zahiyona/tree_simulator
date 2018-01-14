CC=g++
CFLAGS=-O2
OMPFLAGS=-fopenmp
C64FLAGS=$(CFLAGS)
LFLAGS=
DEBUGFLAGS=-g -O0
PROFILEFLAGS=-pg
OBJS=spr_supertree
all: $(OBJS)

spr_supertree: spr_supertree.cpp *.h
	$(CC) $(CFLAGS) -o spr_supertree spr_supertree.cpp


.PHONY: test
.PHONY: debug
.PHONY: hyb
.PHONY: profile

debug:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) -o spr_supertree spr_supertree.cpp
profile:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) $(PROFILEFLAGS) -o spr_supertree spr_supertree.cpp
omp:
	$(CC) $(CFLAGS) $(OMPFLAGS) -o spr_supertree-omp spr_supertree.cpp
omp-debug:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) $(OMPFLAGS) -o spr_supertree-omp spr_supertree.cpp
