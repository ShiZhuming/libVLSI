all: main

# dynamic library libVLSI.so
main: main.cpp bin/libVLSI.so
	g++ main.cpp -o main -L bin -lVLSI -Wl, -std=c++14

# compile the dynamic library
bin/libVLSI.so: src/VLSI.cpp include/VLSI.h src/utility.cpp include/utility.h
	g++ -shared -fPIC -o bin/libVLSI.so src/utility.cpp src/VLSI.cpp -std=c++14

clean:
	rm -f main
	rm -f bin/*.so
	rm -f *.o
	rm -f *.so
	rm -f *.a