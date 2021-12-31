PROJECT_NAME := test

ifneq ("$(origin CC)", "commandline")
  CC := gcc
endif

files := test.c selog.c
CFLAGS := -Wall -Wextra -Wpedantic -g -Os

all: $(PROJECT_NAME)

$(PROJECT_NAME): FORCE
	$(CC) $(files) -o $(PROJECT_NAME) $(CFLAGS)

run: $(PROJECT_NAME)
	./$(PROJECT_NAME)
clean:
	rm -f $(PROJECT_NAME)

FORCE: ;
