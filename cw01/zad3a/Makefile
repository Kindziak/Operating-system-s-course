All:
	#biblioteki statyczne
	gcc -Wall -c addressBook.c -o addressBook.o
	ar rcs libaddressbook.a addressBook.o
	gcc -Wall main.c -o main1 libaddressbook.a
	./main1
	rm *.o *.a main1
	#biblioteki dzielone
	gcc -Wall -fPIC -shared addressBook.c -o libaddressbook.so
	gcc -Wall main.c -L. -Wl,-rpath=. -laddressbook -o main2
	./main2
	rm *.so main2
	#biblioteki ladowane dynamicznie
	gcc -Wall -c -fPIC -o libdynamic.o addressBook.c
	gcc -Wall -shared -o libdynamic.so libdynamic.o
	gcc maind.c -o main3 -ldl -Wl,-rpath=.
	./main3
	rm *.o *.so main3
