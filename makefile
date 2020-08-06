CC = g++

breadth: 
	$(CC) -std=c++11 -O3 ./src/main.cpp ./src/solution.cpp ./src/cvrp.cpp -o cvrp

clean:
	rm cvrp