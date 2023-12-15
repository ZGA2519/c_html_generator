CC=g++
CFLAG=-std=c++20 -g

htmlc : main.o
	$(CC) $(CFLAG) -o htmlc main.o
	
main.o : main.cpp
	$(CC) $(CFLAG) -c main.cpp
	
clean :
	rm -f htmlc
	rm -f *.o
	rm -f index.html
