build:
	go build -gcflags "-N -l" -o bin/easydb-server main.go

clean:
	rm -rf bin

.PHONY: build clean