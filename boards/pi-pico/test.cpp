
extern "C" {
#include "default.h"
#include "libs/hardware.h"
#include "libs/printf.h"
}
#include "test.h"


class TestClass {
	public:
		int x;
		void doSomething() {
			x = 4;
			doAnotherThing();
			debug_put_char('c');
		}
		void doAnotherThing() {
			x=12;
		}

};

void testfunc() {
			debug_put_char('c');
	TestClass t;
	t.doSomething();
}
