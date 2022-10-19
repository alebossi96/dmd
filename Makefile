LDLIBS = -lhidapi-libusb -L/home/a/Desktop/dmd/getbasis -lgetbasis -Wl,-rpath=/home/a/Desktop/dmd/getbasis -lm
# -lhidapi-hidraw
CCFLAGS = -I/usr/local/include/hidapi -g -Wall -pedantic -I/home/a/Desktop/dmd/getbasis -lm

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
