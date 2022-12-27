.PHONY: clean build all

# compilation settings
CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -Werror -pedantic -std=gnu99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -g
LDFLAGS = -lm
# directory paths
INCLUDE_DIR = ./
SRC_DIR = ./
OBJ_DIR = ./

# file lists
CFILES = hashmap.c search.c scorelist.c querylist.c 
OBJS = hashmap.o search.o scorelist.o querylist.o 

# binary
BIN = search

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(BIN) *~
