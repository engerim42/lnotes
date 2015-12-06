
CC = gcc

lnotes: lnotes.c 
	$(CC) `gtk-config --cflags`  lnotes.c -o lnotes `gtk-config --libs`

clean: 
	rm -f *.o lnotes
