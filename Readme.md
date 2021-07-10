How to run:

To compile server- 
	gcc -o server server.c
To run server-
	./server

To compile client-
	gcc -o client client.c
To run client-
	./client

Server can send any files(not directories) in it's directory it has permission to read.
It sends files to one client at a time.
Client can ask for any number of files from the server using- 
	get file1 file2 
and so on.
If the file already exists it will be overwritten. If the file already exists and we don't have permission to read it client will throw an error and same for if a directory of the same name exists in the client directory.




