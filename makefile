LINFLAG=-DLINUX

dij.o: 
	gcc -o dij.o $(LINFLAG) grafos.cpp Logger.cpp Parser.cpp

r:	dij.o
	./dij.o $(LOG)
	rm dij.o

clean:
	rm dij.o
