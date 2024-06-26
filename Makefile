CC=cc
CFLAGS=-Wall -Wextra
TARGET=netbsd_exporter
VERSION=0.9.4

PACKAGE=$(TARGET)-$(VERSION).tar.gz

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

install: $(TARGET)
	cp $(TARGET) /usr/libexec
	cp $(TARGET).8 /usr/share/man/man8

.PHONY: all clean install
