all: run

run: compile
	java -cp ".:lib/*" Client

compile: client.java 	
	javac -cp ".:lib/*" Client.java

clean:
	rm *.class
