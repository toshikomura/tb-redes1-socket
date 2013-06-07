all:
	gcc -c socket.c -ggdb
	gcc -c servidor.c -ggdb
	gcc -c cliente.c -ggdb
	gcc -c protocolo.c -ggdb
	gcc -o servidor socket.o servidor.o protocolo.o -ggdb 
	gcc -o cliente socket.o cliente.o protocolo.o -ggdb
	rm socket.o
	rm servidor.o
	rm cliente.o
	rm protocolo.o
