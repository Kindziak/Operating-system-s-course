All:
	gcc -c FIFO.c -o FIFO.o
	ar rcs libFIFO.a FIFO.o
	gcc -pthread sleepingBarber.c -L. -lFIFO -o sleepingBarber -lrt
	gcc -pthread clients.c -L. -lFIFO -o clients -lrt
