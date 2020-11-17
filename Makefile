LDLIBS = -lhidapi-libusb
# -lhidapi-hidraw
CCFLAGS = -I/usr/local/include/hidapi -g -Wall -pedantic

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
