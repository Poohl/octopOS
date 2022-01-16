
#include "api.hpp"

static Foo* f_p;

void helper() {
   f_p->setFoo(5); // this assembles to the "undefined instruction" instruction
}

void initAPI(Foo* _f_p) {
   f_p = _f_p;
   f_p->setFoo(4); // this is fine
   helper();
}