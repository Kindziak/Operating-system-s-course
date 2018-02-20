All:
	#biblioteki statyczne
	gcc -Wall -g -c addressBook.c -o addressBook.o
	ar rcs libaddressbook.a addressBook.o
	gcc -Wall -g main.c -o main1 libaddressbook.a
	./main1
	rm *.a *.o main1
