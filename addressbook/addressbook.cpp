#include "pch.h"
#include <iostream>
#include<cstdlib>

int main(int argv, char** argc) {
	if (argv < 2)
	{
		return 0;
	}
	const char *te = "address.exe input.txt 031702513.txt";
	system(te);
	return 0;
}

