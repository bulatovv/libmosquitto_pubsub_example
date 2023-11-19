all: subscriber publisher publisher_test

subscriber: subscriber.o shared.o
	gcc -lmosquitto -o subscriber subscriber.o shared.o

publisher: publisher.o shared.o metrics.o
	gcc -lmosquitto -o publisher publisher.o shared.o metrics.o

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o subscriber publisher

.PHONY: all clean 
