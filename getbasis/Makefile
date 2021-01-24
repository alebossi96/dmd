CCFLAGS =  -g -Wall -pedantic -fPIC -lm

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	gcc -shared -Wl,-soname,libgetbasis.so -lm -o libgetbasis.so $(OBJS)
	$(RM) *.o

$(OBJS) : $(SRCS)
	gcc $(CCFLAGS) -c $(SRCS)



clean:
	$(RM) *.o
	$(RM) getbasis.so 
