.PHONY: all

build:
	clang -Wall -Werror main.c -o cx

all: build

run:
	./cx test.cx

clean:
	rm cx
