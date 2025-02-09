g++ -c src/main.cpp -o build/main.o
g++ -c src/lexer.cpp -o build/lexer.o

g++ build/main.o build/lexer.o -o zc

./zc small.zi test_program