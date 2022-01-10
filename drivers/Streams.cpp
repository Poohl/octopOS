
#include "Streams.hpp"

sequence_io_status BlockingInStream::read(byte* dest, uint len) {
	sequence_io_status out;
	for (out = {0}; out.io < len && (out.err = read_byte()) >= 0; ++out.io);
	if (out.err >= 0)
		out.err = 0;
	return out;
}

sequence_io_status BlockingOutStream::write(const byte* data, uint len) {
	sequence_io_status out;
	for (out = {0}; out.io < len && (out.err = write_byte(data[out.io])) >= 0; ++out.io);
	if (out.err >= 0)
		out.err = 0;
	return out;
}

int AsyncInStream::available() {
	return 0;
}

class ReadAvailableCallback : public Callback<sequence_io_status> {
	private:
		sequence_io_status* dest;
	public:
		ReadAvailableCallback(sequence_io_status* d) : dest(d) {}
		virtual void call(sequence_io_status out) {
			*dest = out;
		}
};

sequence_io_status AsyncInStream::read_available(byte* dest, uint len) {
	sequence_io_status out = {0};
	int a = available();
	a = a > len ? len : a;
	if ( a > 0) {
		ReadAvailableCallback c(&out);
		read(dest, a, &c);
		while (out.io != a)
			asm volatile("" : : : "memory");
	}
	return out;
}

sequence_io_status AsyncInStream::discard() {
	byte dump[128];
	sequence_io_status out = {0,0};
	while (available() > 0) {
		out.io += read_available(dump, 128).io;
	}
	return out;
}
