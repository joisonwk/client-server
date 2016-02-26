#Makefile
#CC = /home/joison/invengo-work/usr/local/arm/3.4.1/bin/arm-linux-gcc
CC = gcc
CFLAGS = -Iinclude

DEPENDS = conn/tcpclient.o 

client-bin:$(DEPENDS) main.o
	$(CC) $(CFLAGS) -o $@ $^

client-test:$(DEPENDS) main-for_test.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(DEPENDS) $(TARGET) main.o main-for_test.o
