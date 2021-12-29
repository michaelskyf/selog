PROJECT_NAME := test

MAKEFLAGS += -rR

ifneq ("$(origin CC)", "commandline")
  CC := gcc
endif
files := test.c selog.c
CFLAGS := -Wall -Wextra -Wpedantic -g -Os
CFLAGS += -Iinclude

all: $(PROJECT_NAME)

$(PROJECT_NAME): FORCE
	$(CC) $(files) -o $(PROJECT_NAME) $(CFLAGS)

run: $(PROJECT_NAME)
	clear
	./$(PROJECT_NAME)
clean:
	rm -f $(PROJECT_NAME)

FORCE: ;
