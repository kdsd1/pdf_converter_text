CC=gcc
CFLAGS=-I../libharu/libharu-master/include -O2 -Wall 
LDFLAGS += -fPIC -s -L../libharu/libharu-master/ -L../zlib-1.2.8 -lhpdf -lm -lz
objects = main.o

EXEC = pdfcvter

all: $(EXEC) 

$(EXEC): $(objects)
	$(CC)  $(CFLAGS) -o $(EXEC) $(objects) $(LDFLAGS)
clean: 
	rm -f $(EXEC) *.o
.PHONY: clean
