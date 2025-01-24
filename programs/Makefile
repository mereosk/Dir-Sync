SOURCE		= quic
CC		    = gcc
ARGS        = -v -d ../../project4 ~/project4_testing
OUT		= quic
INCLUDE = ../include
MODULES = ../modules
MISC    = ../misc
LDFLAGS = 

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall  -g  -lpthread -I$(INCLUDE)

# Αρχεία .o
OBJS = $(SOURCE).o $(MODULES)/ancillary_funcs.o

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(CFLAGS)

run: $(OUT)
	./$(OUT) $(ARGS)

valgrind: $(OUT)
	valgrind --leak-check=full ./$(OUT) $(ARGS)

clean:	
	rm -f $(OUT) $(OBJS)
