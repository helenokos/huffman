all : huffman.cpp
	g++ -std=c++11 -c huffman.cpp
	g++ -std=c++11 -o huffman.exe huffman.cpp
clean :
	rm -f huffman.exe
	rm -f huffman.o
	rm -f compress*
	rm -f copy*
