C	= gcc
OBJECTS	= tcp_client.o 
PROGS 	= tcp_client
all : $(PROGS)
$(PROGS) : $(OBJECTS)
	$(CC) -o $@ $^ 
%.o: %.c
	$(CC) -c $(CFLAGS) $<
clean:
	rm -f *.o $(PROGS)

