// global_mess2.cpp global variables produce a lot of troubles in
//                  big projects - see why.
//
// Global variables can be used before they are constructed and
// after they are destroyed.
// Singletons help to ensure that objects are initialized for sure but
// still they can produce troubles by using already destroyed objects.
// See the output of the program:
//	Bar::get_instance()
//	Bar::Bar()
//	Foo::get_instance()
//	Foo::Foo()
//	main() foo=0x... bar=0x...
//	Foo::~Foo()
//	Bar::get_instance()
//	Bar::do_something()
//	Bar::~Bar()
//	Foo::get_instance()
//	Foo::do_something()     // Foo::instance is already destructed
//
// build: cc global_mess.cpp -o global_mess
// use:   ./global_mess
/////////////////////////////////////////////////////////////////////

#include <stdio.h>


class Foo {
	Foo();
	~Foo();
public:
	static Foo& get_instance();
	void do_something();
};

class Bar {
	Bar();
	~Bar();
public:
	static Bar& get_instance();
	void do_something();
};


Foo::Foo() {
	printf("%s::%s()\n", "Foo", __func__);
}

Foo::~Foo() {
	printf("%s::%s()\n", "Foo", __func__);
	Bar::get_instance().do_something();
}

Foo& Foo::get_instance() {
	printf("%s::%s()\n", "Foo", __func__);
	static Foo instance;	// the single global Foo instance
	return instance;
}

void Foo::do_something() {
	printf("%s::%s()\n", "Foo", __func__);
}


Bar::Bar() {
	printf("%s::%s()\n", "Bar", __func__);
}

Bar::~Bar() {
	printf("%s::%s()\n", "Bar", __func__);
	Foo::get_instance().do_something();
}

Bar& Bar::get_instance() {
	printf("%s::%s()\n", "Bar", __func__);
	static Bar instance;	// the single global Bar instance
	return instance;
}

void Bar::do_something() {
	printf("%s::%s()\n", "Bar", __func__);
}


int main(const int argc, const char* argv[])
{
	printf("%s() foo=%p bar=%p\n", __func__, &Foo::get_instance(), &Bar::get_instance());
	return 0;
}
