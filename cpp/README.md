# C++ trials

Trials for C++ programming related questions.

To build the targets start make in this directory.

    .../trials/cpp $ make


## Prerequisites

- `make` (I recommend gnu make)
- `c++` (I recommend gnu compiler collection)


## File list

|Source                |Target                |Description                                                                           |
|----------------------|----------------------|--------------------------------------------------------------------------------------|
|bypass_destructor.cpp |bin/bypass_destructor |Are destructors processed in case of a terminating signal or an exit call?            |
|global_mess.cpp       |bin/global_mess       |global variables produce a lot of troubles in big projects - see why.                 |
|global_mess2.cpp      |bin/global_mess2      |global variables produce a lot of troubles in big projects - see why.                 |
|static_side_effect.cpp|bin/static_side_effect|uses of the one time initialization of a static variable to print a message just once.|
