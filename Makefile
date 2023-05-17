all:
	g++ -g -c Letter.cpp -o build/letter.o
	g++ -g -c Conf.cpp -o build/conf.o
	g++ -g -c Phonotext.cpp -o build/phonotext.o
	g++ -g -c phonotry.cpp -o build/phonotry.o
	g++ build/letter.o build/conf.o build/phonotext.o build/phonotry.o -o build/phonotry
