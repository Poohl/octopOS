
class Foo {
	public:
		virtual void setFoo(int);
};

class Bar {
	public:
		virtual void setBar(int);
};

class FooBar : public Foo, public Bar {
	public:
    virtual void setFoo(int);
    virtual void setBar(int);
};

void initAPI(Foo*);