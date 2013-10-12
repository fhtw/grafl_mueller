all:
	g++ -std=c++0x -Wall -o ./bin/Debug/server server.cpp
	g++ -std=c++0x -Wall -o ./bin/Debug/client client.cpp 
clean:
	rm -f ./bin/Debug/server
	rm -f ./bin/Debug/client

dist:
	mkdir ./bin/Debug/twmailer_abgabe
	cp ./server.cpp ./bin/Debug/twmailer_abgabe/server.cpp
	cp ./client.cpp ./bin/Debug/twmailer_abgabe/client.cpp	
	cp ./Makefile ./bin/Debug/twmailer_abgabe/Makefile
	tar -czf twmailer_abgabe.tar.gz twmailer_abgabe
	rm -rf twmailer_abgabe

distclean:
	rm -rf twmailer_abgabe.tar.gz
