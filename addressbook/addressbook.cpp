#include <fstream>
#include <iostream>
#include<string>
#include <sstream>
#include"include/json/json.h"
using namespace std;

#pragma comment(lib, "json_vc71_libmtd.lib")


string provinces[31];
const string Municipality[] = { "北京","上海","天津","重庆" };
//string result;
string list;
const char *analysis;
int index = 0;
//全国地图数据分割为两个JsonValue存储
Json::Value pcas0;
Json::Value pcas1;
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
Json::Value GetAddress(int mode)
{
	Json::Reader reader;// 解析json用Json::Reader
	Json::Value Address;
	Json::Value city_list;
	Json::Value region;
	index++;
	string provinces;
	string name = list.substr(index, 4);
	index += 4;
	bool IsMunicipality = false;
	for (size_t i = 0; i < 4; i++)
	{
		if (name == Municipality[i])
		{
			IsMunicipality = true;
			break;
		}
	}
	if (IsMunicipality)
	{
		Address.append(name);
		Address.append(name + "市");
		provinces = name + "市";
		region = pcas0[provinces][provinces];
		if (list.substr(index, 2) == "市")
		{
			index += 2;
		}
	}
	else
	{
		if (list.substr(index, 2) == "江" || list.substr(index, 2) == "古")
		{
			provinces = name + list.substr(index, 2) + "省";
			Address.append(name + list.substr(index, 2) + "省");
			index += 2;
		}
		else
		{
			provinces = name + "省";
			Address.append(name + "省");
		}
		if (list.substr(index, 2) == "省")
		{
			index += 2;
		}

		
		if (pcas0.isMember(provinces))
		{
			city_list = pcas0[provinces];
		}
		else if(pcas1.isMember(provinces))
		{
			city_list = pcas1[provinces];
		}
		else
		{
			cout << "地图数据不存在:" << provinces;
		}
		
		int icity = 0;
		while (index + icity < list.length())
		{
			icity += 2;
			string city = list.substr(index, icity);
			string citys = list.substr(index, icity) + "市";
			if (city_list.isMember(citys))
			{
				if (list.substr(index+icity)=="市")
				{
					index = index + icity + 2;
					cout << list.substr(index, 2)<< endl;;
				}
				else
				{
					index = index + icity;
					cout << list.substr(index, 2)<<endl;
				}
				region = city_list[citys];
				Address.append(citys);
				cout << citys << endl;
				break;
			}
			else if(city_list.isMember(city))
			{
				index = index + icity;
				region = city_list[city];
				Address.append(city);
				cout << city<<endl;
				break;
			}
		}
		
		
	}
	return Address;
}
void ReadMapOfChina()
{
	Json::Reader reader;// 解析json用Json::Reader
	stringstream  dastr0;
	stringstream  dastr1;
	string ds;
	ifstream is0("C:\\Users\\Administrator\\Desktop\\addressbook\\addressbook\\pcas0.json");
	if (!is0.is_open())		cout << "无法读取文件pcas0.json" << endl;
	dastr0 << is0.rdbuf();
	ds = dastr0.str();
	if (!reader.parse(ds, pcas0)) { cout << "无法读取json0" << endl; }
	is0.close();
	dastr0.clear();

	ifstream is1("C:\\Users\\Administrator\\Desktop\\addressbook\\addressbook\\pcas1.json");
	if (!is1.is_open())		cout << "无法读取文件pcas1.json" << endl;
	dastr1 << is1.rdbuf();
	ds = dastr1.str();
	if (!reader.parse(ds, pcas1)) { cout << "无法读取json1" << endl; }
	is1.close();
	dastr1.clear();
}
int main(int argc, char** argv)
{
	ReadMapOfChina();
	Json::Value root;
	ifstream infile(argv[1]);
	while (infile >> list)
	{
		cout << list << endl;;
		Json::Value a_list;
		analysis = list.data();
		index = 0;
		int mode = *analysis - 48;
		string name = GetName();
		string phone = GetPhone();
		Json::Value address = GetAddress(mode);
		a_list["地址"] = address;
		a_list["手机"] = phone;
		a_list["姓名"] = name;
		root.append(a_list);
	}

	infile.close();
	ofstream out(argv[2]);
	out << root.toStyledString();
	out.close();
	return 0;
}
