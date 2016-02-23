#Makefile
CC = gcc
CFLAGS = -Iinclude

DEPENDS = conn/tcpclient.o main.o

TARGET=client-bin
$(TARGET):$(DEPENDS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(DEPENDS) $(TARGET)
