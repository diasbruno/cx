.PHONY: all

SOURCES=parser.c cell.c main.c token.c
OBJECTS=$(SOURCES:%.c=%.o)
TESTS=test_cell.c

%.o: %.c
	clang -Wall -Werror -g -c $< -o $@

build: $(OBJECTS)

app: build
	clang -Wall -Werror -g -o cx $(OBJECTS)

all: app

run:
	./cx test.cx

clean:
	rm -rf cx $(OBJECTS)
