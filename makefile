test:
	g++ -std=c++11 -g test.cpp -o test

clean:
	rm -f test

.PHONY: test clean