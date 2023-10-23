all: bin/listeclient bin/awale bin/client bin/server 

bin/awale: awale/awale.c
	gcc -Wall awale/awale.c -o bin/awale

bin/client: client/client.c
	gcc -Wall client/client.c -o bin/client

bin/listeclient: server/listeclient.c
	gcc -Wall server/listeclient.c -o bin/listeclient

bin/server: server/server.c
	gcc -Wall server/server.c -o bin/server

start:
	gnome-terminal -- bin/server && gnome-terminal --tab -- bin/client 127.0.0.1 valery && gnome-terminal --tab -- bin/client 127.0.0.1 georges && gnome-terminal --tab -- bin/client 127.0.0.1 jacques

clean:
	rm -rf bin/*