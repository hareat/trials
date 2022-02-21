// global_mess.cpp global variables produce a lot of troubles in
//                 big projects - see why.
//
// Global variables can be used before they are constructed and
// after they are destroyed. See the output of the program:
//	Foo::Foo()
//	Bar::do_something()     // g_bar is still uninitialized
//	Bar::Bar()
//	Foo::do_something()
//	main()
//	Bar::~Bar()
//	Foo::do_something()
//	Foo::~Foo()
//	Bar::do_something()     // g_bar is already destructed
//
// build: cc global_mess.cpp -o global_mess
// use:   ./global_mess
/////////////////////////////////////////////////////////////////////

#include <stdio.h>


struct Foo {
	Foo();
	~Foo();
	void do_something();
} g_foo;	// define the global variable

struct Bar {
	Bar();
	~Bar();
	void do_something();
} g_bar;	// define the global variable


Foo::Foo() {
	printf("%s::%s()\n", "Foo", __func__);
	g_bar.do_something();
}

Foo::~Foo() {
	printf("%s::%s()\n", "Foo", __func__);
	g_bar.do_something();
}

void Foo::do_something() {
	printf("%s::%s()\n", "Foo", __func__);
}


Bar::Bar() {
	printf("%s::%s()\n", "Bar", __func__);
	g_foo.do_something();
}

Bar::~Bar() {
	printf("%s::%s()\n", "Bar", __func__);
	g_foo.do_something();
}

void Bar::do_something() {
	printf("%s::%s()\n", "Bar", __func__);
}


int main(const int argc, const char* argv[])
{
	printf("%s()\n", __func__);
	return 0;
}
