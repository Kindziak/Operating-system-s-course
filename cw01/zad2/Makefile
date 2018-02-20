All:
	#biblioteki statyczne
	gcc -Wall -c addressBook.c -o addressBook.o
	ar rcs libaddressbook.a addressBook.o
	gcc -Wall main.c -o main1 libaddressbook.a
	./main1
	rm *.o *.a main1
