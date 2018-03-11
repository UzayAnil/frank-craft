#include "Winter.hpp"

#include <iostream>
using namespace std;

int main( int argc, char **argv ) 
{
	if (argc > 1) {
		std::string luaSceneFile(argv[1]);
		std::string title("Assignment 3 - [");
		title += luaSceneFile;
		title += "]";

		CS488Window::launch(argc, argv, new Winter(luaSceneFile), 1024, 768, title);

	} else {
		cout << "Must supply Lua file as first argument to program.\n";
        cout << "For example:\n";
        cout << "./Winter Assets/simpleScene.lua\n";
	}

	return 0;
}
