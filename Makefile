LDLIBS = -lhidapi-libusb -L/home/d/Desktop/DMD/dmd/getbasis -lgetbasis -Wl,-rpath=/home/d/Desktop/DMD/dmd/getbasis -lm
# -lhidapi-hidraw
CCFLAGS = -I/usr/local/include/hidapi -g -Wall -pedantic -I/home/d/Desktop/DMD/dmd/getbasis -lm

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	gcc $(OBJS) $(LDLIBS) -o main 
	$(RM) *.o

$(OBJS) : $(SRCS)
	gcc $(CCFLAGS) -c $(SRCS) 



clean:
	$(RM) *.o
	$(RM) main
