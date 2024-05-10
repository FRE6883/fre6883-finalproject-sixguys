CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -lpthread


main:  main.o Ticker.o GetPrice.o Matrix.o Calculation.o Bootstrap.o
	$(CC) $(CFLAGS) -o main  main.o Ticker.o GetPrice.o Matrix.o Calculation.o Bootstrap.o

main.o: main.cpp Ticker.h Calculation.h GetPrice.h Matrix.h plotGnuplot.h
	$(CC) $(CFLAGS) -c main.cpp

Ticker.o: Ticker.h Ticker.cpp Matrix.h
	$(CC) $(CFLAGS) -c Ticker.cpp

Calculation.o: Calculation.h Calculation.cpp Matrix.h Ticker.h
	$(CC) $(CFLAGS) -c Calculation.cpp

GetPrice.o: GetPrice.h GetPrice.cpp Matrix.h Ticker.h
	$(CC) $(CFLAGS) -c GetPrice.cpp

Matrix.o: Matrix.h Matrix.cpp
	$(CC) $(CFLAGS) -c Matrix.cpp

Bootstrap.o: Bootstrap.h Bootstrap.cpp Calculation.h Ticker.h Matrix.h
	$(CC) $(CFLAGS) -c Bootstrap.cpp

clean:
	rm -rf  main  *.o
