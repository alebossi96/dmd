LDLIBS = -lhidapi-libusb -L/home/d/Desktop/DMD/dmd/getbasis -lgetbasis -Wl,-rpath=/home/d/Desktop/DMD/dmd/getbasis
# -lhidapi-hidraw
CCFLAGS = -I/usr/local/include/hidapi -g -Wall -pedantic -I/home/d/Desktop/DMD/dmd/getbasis

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	g++ $(OBJS) -o main $(LDLIBS)
	$(RM) *.o

$(OBJS) : $(SRCS)
	g++ $(CCFLAGS) -c $(SRCS) 



clean:
	$(RM) *.o
	$(RM) main
