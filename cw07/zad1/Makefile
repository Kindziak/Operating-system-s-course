All:
	gcc -c FIFO.c -o FIFO.o
	ar rcs libFIFO.a FIFO.o
	gcc sleepingBarber.c -L. -lFIFO -o sleepingBarber
	gcc clients.c -L. -lFIFO -o clients
