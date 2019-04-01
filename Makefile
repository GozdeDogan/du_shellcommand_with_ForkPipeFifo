all: buNeDuFPF

buNeDuFPF: main.o
	gcc main.o -o buNeDuFPF

131044019_main.o: main.c
	gcc -c main.c

clean:
	rm *.o buNeDuFPF
