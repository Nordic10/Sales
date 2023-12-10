default: all sales

all:
	# "building city data reader example"
	g++ -O -Wall -o datareader datareader.cpp

sales:
	# "building sales.cpp"
	g++ -O -Wall -o sales sales.cpp

clean:
	rm -f datareader *~ *png
	rm -f sales *~ *png
