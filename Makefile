make_socket: server.c 
	$(CC) server.c -o server ;
	cd www && ../server
run:
	cd www && ../server
debug:
	cd www && strace ../server
