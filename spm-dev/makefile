CC=gcc
CPP=g++
default:spm-dev
main.o:
	 $(CPP) -c -o main.o main.cpp -lstdc++
spm-dev: main.o 
	$(CC) -o spm-dev main.cpp -lstdc++
clean:
	rm main.o
	