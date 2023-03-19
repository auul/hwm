all:
	gcc hwm.c -lX11 -o hwm

clean:
	rm hwm

install:
	cp hwm /usr/bin/

.PHONY:
	clean install
