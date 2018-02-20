All:
	#biblioteki statyczne
	gcc -Wall -c addressBook.c -Os -o addressBook.o
	ar rcs libaddressbook.a addressBook.o
	gcc -Wall main.c -Os -o main1 libaddressbook.a
	./main1
	rm *.a *.o main1
