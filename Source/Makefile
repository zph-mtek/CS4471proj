all: host virus seed

host:
	gcc -Wall -Wextra host.c -o host

virus:
	gcc -Wall -Wextra virus.c -o virus

seed: virus host
	cp virus seed
	printf '\x01\x00\x00\x00' >> seed
	printf '\xde\xad\xbe\xef' >> seed
	cat host >> seed
	chmod u+x seed

clean:
	rm -f host virus seed
	rm -rf tem