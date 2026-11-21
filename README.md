CS4471proj2

Run the following from the project folder to make new victim files
cp /bin/ls ./victim1
cp /usr/bin/wc ./victim2
cp /usr/bin/file ./victim3



Testing

cd Source
make
./seed ./victim1
chmod u+x ./victim1
./victim1 ./victim2
chmod u+x ./victim2
./victim2 ./victim3
sha256sum ./host1 ./host2 ./host3 ./host4

Mutation

The virus has a four-byte counter placed immediately before the 0xdeadbeef sentinel.
Each time the virus infects a victim, the counter is read and increased by one.
The updated counter changes the prepended virus portion, so infected files have different SHA-256 hash values.