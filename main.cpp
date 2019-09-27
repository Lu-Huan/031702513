#include <fstream>
#include <iostream>
#include<string>
#include <sstream>
#include <windows.h>
#include"include/json/json.h"
using namespace std;

#pragma comment(lib, "json_vc71_libmtd.lib")


string provinces[31];
const string Municipality[] = { "北京","上海","天津","重庆" };
const string AutonomousRegion[] = { "内蒙","广西","西藏","宁夏" ,"新疆" };
string list;
const char *analysis;
int index = 0;

//全国地图数据分割为两个JsonValue存储
//使用全局的Value，程序退出时会错
//原因：析构函数出错,修改源码重新编译没问题
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
	Json::Value region_list;
	Json::Value village_list;
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
	bool special = false;
	string special_city;
	if (IsMunicipality)
	{
		Address.append(name);
		Address.append(name + "市");
		provinces = name + "市";
		region_list = pcas0[provinces][provinces];
		if (list.substr(index, 2) == "市")
		{
			index += 2;
		}
	}
	else
	{
		if (list.substr(index, 2) == "江")
		{
			provinces = name + list.substr(index, 2) + "省";
			//Address.append(name + list.substr(index, 2) + "省");
			index += 2;
		}
		else
		{
			bool IsAutonomous = false;
			for (size_t i = 0; i < 5; i++)
			{
				if (name == AutonomousRegion[i])
				{
					IsAutonomous = true;
					int autonomous_end = index + 4;
					while (list.substr(autonomous_end, 2) != "区")
					{
						autonomous_end += 2;
					}
					autonomous_end += 2;
					provinces = list.substr(index - 4, autonomous_end - index + 4);
					index = autonomous_end;
					//cout << provinces << endl;
				}
			}
			if (!IsAutonomous)
			{
				provinces = name + "省";
				//Address.append(name + "省");
			}
		}
		if (list.substr(index, 2) == "省")
		{
			index += 2;
		}


		if (pcas0.isMember(provinces))
		{
			city_list = pcas0[provinces];
			Address.append(provinces);
		}
		else if (pcas1.isMember(provinces))
		{
			city_list = pcas1[provinces];
			Address.append(provinces);
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
			if (city == "东莞" || city == "中山")
			{
				special = true;
				special_city = citys;
			}
			if (city_list.isMember(citys))
			{
				if (list.substr(index + icity, 2) == "市")
				{
					index = index + icity + 2;
					//cout << list.substr(index, 2)<< endl;;
				}
				else
				{
					index = index + icity;
					//cout << list.substr(index, 2)<<endl;
				}
				region_list = city_list[citys];
				Address.append(citys);
				//cout << citys << endl;
				break;
			}
			else if (city_list.isMember(city))
			{
				index = index + icity;
				region_list = city_list[city];
				Address.append(city);
				//cout << city<<endl;
				break;
			}
		}
	}
	//区/县
	bool Find = false;
	int irigion = 0;
	while (index + irigion < list.length())
	{
		irigion += 2;
		string rigion = list.substr(index, irigion);
		if (region_list.isMember(rigion))
		{
			Find = true;
			index = index + irigion;
			village_list = region_list[rigion];
			Address.append(rigion);
			//cout << rigion << endl;
			break;
		}
	}
	//缺失
	if (!Find)
	{
		Address.append("");
		if (special)
		{
			/*Json::Value::Members member = region_list.getMemberNames();
			for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
			{
				cout << (*iter) << endl;
			}*/
			village_list = region_list[special_city];
		}
	}
	//乡/镇/街道
	int ivillage = 0;
	while (index + ivillage < list.length())
	{
		ivillage += 2;
		string village = list.substr(index, ivillage);
		bool find = false;
		for (size_t i = 0; i < village_list.size(); i++)
		{
			if (village_list[i] == village)
			{
				index = index + ivillage;
				Address.append(village);
				find = true;
				break;
			}
		}
		if (find)
		{
			break;
		}
	}

	if (mode == 1)
	{
		//5级地址分割已完成
		string ds = list.substr(index, list.length() - index - 1);//去掉句号
		//cout << ds << endl;
		Address.append(ds);
	}
	else if (mode == 2)
	{
		//7级地址分割
		int num = index;

		//寻找号码
		while (!IsNum(analysis + num))
		{
			num++;
			if (num == list.length())
			{
				Address.append(list.substr(index, list.length() - index - 1));
				return Address;
			}
		}
		//寻找号字
		int word_hao = num;
		while (IsNum(analysis + word_hao))
		{
			word_hao++;
		}
		word_hao += 2;
		string Lfive = list.substr(index, num - index);
		string Lsix = list.substr(num, word_hao - num);
		string Lseven = list.substr(word_hao, list.length() - word_hao - 1);
		Address.append(Lfive);
		Address.append(Lsix);
		Address.append(Lseven);
	}
	return Address;
}
void ReadMapOfChina()
{
	Json::Reader reader;// 解析json用Json::Reader
	stringstream  dastr0;
	stringstream  dastr1;
	string ds;
	ifstream is0("pcas0.json");
	if (!is0.is_open())		cout << "无法读取文件pcas0.json" << endl;
	dastr0 << is0.rdbuf();
	ds = dastr0.str();
	if (!reader.parse(ds, pcas0)) { cout << "无法读取json0" << endl; }
	is0.close();
	dastr0.clear();

	ifstream is1("pcas1.json");
	if (!is1.is_open())		cout << "无法读取文件pcas1.json" << endl;
	dastr1 << is1.rdbuf();
	ds = dastr1.str();
	if (!reader.parse(ds, pcas1)) { cout << "无法读取json1" << endl; }
	is1.close();
	dastr1.clear();
}




string GbkToUtf8(const char *src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

string Utf8ToGbk(const char *src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

string inf = "in.txt"; string outf = "out.json";
int main(int argc, char** argv)
{
	ReadMapOfChina();
	Json::Value root;
	//ifstream infile(argv[1]);
	ifstream infile(inf);

	bool frist_list = true;
	while (infile >> list)
	{
		list = Utf8ToGbk(list.data());
		if (frist_list)
		{
			list.erase(0, 1);
			frist_list = false;
		}

		//cout << list << endl;
		Json::Value a_list;

		analysis = list.data();
		index = 0;
		int mode = *analysis - 48;

		a_list["姓名"] = GetName();
		a_list["手机"] = GetPhone();
		a_list["地址"] = GetAddress(mode);
		root.append(a_list);
	}
	infile.close();
	//ofstream out(argv[2]);
	ofstream out(outf);
	string re = root.toStyledString();
	re = GbkToUtf8(re.data());
	out << re;
	out.close();
	return 0;
}
