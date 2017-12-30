ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

all: Nestor

clean:
	rm -rf Nestor lib/*.o

Nestor: Nestor.o NestorInvestment.o CSVReaderInterface.o CSVWriterInterface.o
	c++ lib/Nestor.o lib/NestorInvestment.o lib/CSVReaderInterface.o lib/CSVWriterInterface.o -o Nestor $(ROOTFLAGS) $(ROOTLIBS) -O2

CSVReaderInterface.o: CSVInterface/CSVReaderInterface.cc
	c++ -c CSVInterface/CSVReaderInterface.cc -O2 -Wall
	mv CSVReaderInterface.o lib/

CSVWriterInterface.o: CSVInterface/CSVWriterInterface.cc
	c++ -c CSVInterface/CSVWriterInterface.cc -O2 -Wall
	mv CSVWriterInterface.o lib/

NestorInvestment.o: src/NestorInvestment.cc
	c++ -c src/NestorInvestment.cc -c $(ROOTFLAGS) -O2
	mv NestorInvestment.o lib/

Nestor.o: src/Nestor.cc src/StocksPage.cc src/FundsPage.cc 
	c++ -c src/Nestor.cc -c $(ROOTFLAGS) -O2
	mv Nestor.o lib/
