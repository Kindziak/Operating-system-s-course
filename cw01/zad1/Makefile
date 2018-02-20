All:
	#biblioteki statyczne
	gcc -Wall -c addressBook.c -o addressBook.o
	ar rcs libaddressbook.a addressBook.o
	gcc -Wall main.c -o main1 libaddressbook.a
	./main1
	rm *.a *.o main1
	#biblioteki dzielone
	gcc -Wall -fPIC -shared addressBook.c -o libaddressbook.so
	gcc -Wall main.c -L. -Wl,-rpath=. -laddressbook -o main2
	./main2
	rm *.so main2
