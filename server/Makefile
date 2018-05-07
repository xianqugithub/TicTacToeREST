all: run

docker_make:
	docker build -t ttt-server .

docker_run: docker_make
	docker run -it -p 1234:1234 ttt-server

run: server
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib ./server

server: server.c
	gcc -o server server.c -lulfius -ljansson

clean:
	docker rmi -f ttt-server
	rm ./server