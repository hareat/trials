// static_side_effect.cpp uses of the one time initialization of a
//                        static variable to print a message just once.
//
// Sometimes it is part of the runtime data if a specific content
// should be printed never, once or often. But maybe a header or
// what ever should be printed only once.
// A static variable could be a simple solution for that requirement.
//
// build: cc static_side_effect.cpp -o static_side_effect
// use:   ./static_side_effect
/////////////////////////////////////////////////////////////////////

#include <stdio.h>


// Tedious but obvious use of a static variable to ensure a header line
// is only printed once.
void multiline()
{
	static bool is_printed = false;
	if (is_printed == false)
	{
		printf("%s() this line should be printed only once\n", __func__);
		is_printed = true;
	}
	printf("%s() is_printed=%d\n", __func__, is_printed);
}


// Oneline but not so obvious use of a static variable to ensure a header line
// is only printed once.
void oneliner()
{
	static const int only_once = printf("%s() this line should be printed only once\n", __func__);
	printf("%s() only_once=%d (but nobody will care about it)\n", __func__, only_once);
}


int main(const int argc, const char* argv[])
{
	puts("calling multiline() twice");
	multiline();
	multiline();

	puts("\ncalling onliner() twice");
	oneliner();
	oneliner();
}
