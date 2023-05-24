CC=cc
CFLAGS=-Wall -Wextra
TARGET=netbsd_exporter
REMOTE_HOST=192.168.2.51
REMOTE_USER=user
REMOTE_DIR=/home/user/netbsd_exporter

all: $(TARGET)

$(TARGET): netbsd_exporter.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

deploy: $(TARGET)
	scp $(TARGET) $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)

.PHONY: all clean deploy
