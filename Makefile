CC=cc
CFLAGS=-Wall -Wextra
TARGET=netbsd_exporter
VERSION=`git describe --tags`
PACKAGE=$(TARGET)-$(VERSION).tar.gz
REMOTE_HOST=192.168.2.50
REMOTE_USER=user
REMOTE_DIR=/home/user/patches

all: $(TARGET)

$(TARGET): netbsd_exporter.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
	rm -rf dist

dist:	$(TARGET)
	mkdir -p dist/usr/libexec
	cp $(TARGET) dist/usr/libexec
	mkdir -p dist/usr/share/man/man8
	cp $(TARGET).8 dist/usr/share/man/man8
	(cd dist;tar -cvf ../$(PACKAGE) .)

deploy: dist
	scp $(PACKAGE) $(REMOTE_USER)@$(REMOTE_HOST):$(REMOTE_DIR)

.PHONY: all clean deploy
