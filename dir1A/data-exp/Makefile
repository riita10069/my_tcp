CC = gcc
CFLAGS = -m32 -static -O3 -g
LIBS = libcomfncs.a

all: sd rv compfiles libcomfncs.a 

sd: libcomfncs.a sd.c
	$(CC) $(CFLAGS) -o sd sd.c $(LIBS)

rv: libcomfncs.a rv.c
	$(CC) $(CFLAGS) -o rv rv.c $(LIBS)

compfiles: compare.c
	$(CC) $(CFLAGS) -o compfiles compare.c

libcomfncs.a: comfncs.c inet.h
	$(CC) $(CFLAGS) -c comfncs.c
	ar rcv libcomfncs.a comfncs.o
	ranlib libcomfncs.a

clean:
	rm -f *~ *.o sd rv compfiles libcomfncs.a
