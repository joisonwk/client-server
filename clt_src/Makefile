#Makefile
CC = gcc
CFLAGS = -Iinclude

DEPENDS = conn/tcpclient.o 

TARGET=client-bin
$(TARGET):$(DEPENDS) main.o
	$(CC) $(CFLAGS) -o $@ $^

client-test:$(DEPENDS) main-for_test.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(DEPENDS) $(TARGET)
