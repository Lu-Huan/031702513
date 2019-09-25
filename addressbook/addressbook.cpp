#include <fstream>
#include <iostream>
#include<string>
using namespace std;



const string Municipality[] = { "北京","上海","天津","重庆" };
string result;
string list;
const char *analysis;
int index = 0;
//判断是否是数字 
bool IsNum(const char *te)
{
	if (*te >= 48 && *te <= 57)
		return true;
	return false;
}
/*int IncludeChinese(const char *str) {
	int nRet = 0;
	char c;
	while (c = *str++)
	{
		if ((c & 0x80) && (*str & 0x80)) {
			nRet = 1;
			break;
		}
	}   return nRet;
}*/
string GetName()
{

	int name_end = 0;
	while (*(analysis + index) != ',')
	{
		index++;
		name_end++;
	}
	return list.substr(2, name_end - 2);
}
string GetPhone()
{
	int num = 0;
	int phone_end = index;
	while (*(analysis + phone_end) != '\0')
	{
		phone_end++;
		if (IsNum(analysis + phone_end))
		{
			num++;
			if (num == 11)
			{
				string Phone = list.substr(phone_end - 10, 11);
				list.erase(phone_end - 10, 11);
				return Phone;
			}
		}
		else
		{
			num = 0;
		}
	}
	return nullptr;
}
void GetAddress(int mode, string result)
{
	index++;
	string name = list.substr(index, 4);
	index += 4;
	bool IsMunicipality = false;
	for (size_t i = 0; i <4; i++)
	{
		if (name == Municipality[i])
		{
			IsMunicipality = true;
			break;
		}
	}
	if (IsMunicipality)
	{
		result += (name+"'',''"+name+"市"+"'',");
		if (list.substr(index, 2) == "市")
		{
			index += 2;
		}
	}
	else
	{
		result += name;
		if (list.substr(index, 2) == "江" || list.substr(index, 2) == "古")
		{
			result += list.substr(index, 2);
			index += 2;
		}
		result += "省";
		if (list.substr(index, 2) == "省")
		{
			index += 2;
		}
	}
	cout << result << endl;
}
int main(int argc, char** argv)
{
	ifstream infile(argv[1]);
	
	while (infile >> list)
	{
		result += "[{''姓名'':''";
		cout << list << " " << list.length() << endl;
		analysis = list.data();
		index = 0;
		int mode = *analysis - 48;
		result += GetName();
		result += "'',''手机'':''";
		result += GetPhone();
		result += "'',''地址'':[''";
		GetAddress(mode, result);
		result.clear();
	}
	infile.close();
	return 0;
}
