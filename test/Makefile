
all:
	gcc -shared -fPIC -DLINUX_VERSION_CODE=0x50489 -o libsecmem.so secmem.c
	gcc -Wall -O2 -I. -L.  -o testapp testapp.c -lsecmem

clean:
	rm -rf testapp libsecmem.so
