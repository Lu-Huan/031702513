#include<stdlib.h>
#include<string>
int main(int argv, char** argc) {
	if (argv<2)
	{
		return 0;
	}
	std::string t0 = "address.exe ";
	std::string t1 = argc[0];
	//std::string t1 = "in.txt ";
	t1 += " ";
	std::string t2 = argc[1];
	//std::string t2 = "out.json";
	system((t0 + t1 + t2).data());
	return 0;
}
