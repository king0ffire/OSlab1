linker: main.cpp
	g++-9.2 -g main.cpp -o linker # I always compile with -g to enable debugging
clean:
	rm -f linker