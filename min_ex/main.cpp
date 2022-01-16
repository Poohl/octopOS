
#include <cstdio>
#include "api.hpp"
void FooBar::setFoo(int x) {
	printf("foo %x\n", x);
}
void FooBar::setBar(int x) {
	printf("bar %d\n", x);
}

static FooBar f;
int main() {
    initAPI(&f);
}