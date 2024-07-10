// boost-mutex.cpp shows the use of a mutex to synchonize threads.
//                 Create two threads and synchronize them.
//                 Additionally wait until they are done.
//
// Prerequisites: sudo apt-get install libboost-thread-dev
//
// build: c++ boost-mutex.cpp -lboost_thread -o boost-mutex
// use:   ./boost-mutex
//
// Without boost::mutex:
//	main() startup
//	main() waiting for the threads to end
//	wait_a_second() begin
//	wait_a_second() begin
//	wait_a_second() end
//	wait_a_second() end
//	main() done
// With boost::mutex:
//	main() startup
//	main() waiting for the threads to end
//	wait_a_second() begin
//	wait_a_second() end
//	wait_a_second() begin
//	wait_a_second() end
//	main() done
/////////////////////////////////////////////////////////////////////

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <iostream>


// Instead of std::cout << __func__ << "() ...
// I decided to use std::cout << "<function name>() ...
// for readability for this trial.


void wait_a_second() {
	static boost::mutex mutex;	// global lifetime but reduced scope

	// But the lock should just exist as long as the method is processed
	// so a local variable is the right choice for this lifetime
	const boost::mutex::scoped_lock lock(mutex);

	std::cout << "wait_a_second() begin" << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(1));
	std::cout << "wait_a_second() end" << std::endl;
}


// Create two threads and synchronize them. Wait until they are done.
int main(const int argc, const char* argv[]) {
	std::cout << "main() startup" << std::endl;
	boost::thread thread1(wait_a_second);
	boost::thread thread2(wait_a_second);
	std::cout << "main() waiting for the threads to end" << std::endl;
	thread1.join();
	thread2.join();
	std::cout << "main() done" << std::endl;
	return 0;
}
