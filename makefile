all: bin/awale bin/client bin/server

bin/awale: awale/awale.c
	gcc -Wall awale/awale.c -o bin/awale

bin/client: client/client.c
	gcc -Wall client/client.c -o bin/client

bin/server: server/server.c
	gcc -Wall server/server.c -o bin/server

clean:
	rm -rf bin/*