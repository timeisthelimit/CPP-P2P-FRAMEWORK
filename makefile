app: app.o server.o
	g++ app.o server.o -o app

app.o: app.cpp server.hpp cerror.h
	g++ app.cpp -c -o app.o

server.o: server.cpp server.hpp cerror.h
	g++ server.cpp -c -o server.o


clean:
	rm app *.o