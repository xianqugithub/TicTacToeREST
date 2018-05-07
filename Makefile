all: server_run  
			
server_run: 
	cd ./server && make docker_run

client_run: 
	cd ./client && make run
