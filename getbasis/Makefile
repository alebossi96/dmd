CCFLAGS =  -g -Wall -pedantic -fPIC

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	g++ -shared -Wl,-soname,libgetbasis.so  -o libgetbasis.so $(OBJS)
	$(RM) *.o

$(OBJS) : $(SRCS)
	g++ $(CCFLAGS) -c $(SRCS)



clean:
	$(RM) *.o
	$(RM) getbasis.so 
