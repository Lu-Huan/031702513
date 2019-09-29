#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include"include/json/json.h"
using namespace std;

#pragma comment(lib, "json_vc71_libmtd.lib")

string provinces[31];
//直辖市
const string Municipality[] = { "北京","上海","天津","重庆" };
//自治区
const string AutonomousRegion[] = { "内蒙","广西","西藏","宁夏" ,"新疆" };

string city_end[] = { "市","州","区" };
string region_end[] = { "县","区","市" };
string five_end[] = { "路","街","巷","同","道" ,"里" };
string six_end[] = { "号","弄" };
//全国地图数据分割为两个JsonValue存储
//使用全局的Value，程序退出时会错
//原因：析构函数出错
//修改源码重新编译没问题
Json::Value pcas0;
Json::Value pcas1;
Json::Value ans;
Json::Value test;
string list;
const char *analysis;
int index = 0;
//判断是否是数字 
int testnum = -1;
bool IsMunicipality(string city, bool addback = false)
{
	if (addback)
	{
		city = city.substr(0, 4);
	}
	for (size_t i = 0; i < 4; i++)
	{
		if (city == Municipality[i])
		{
			return true;
		}
	}
	return false;
}
//中间一级缺失搜寻 
string LostToSerch(int level, Json::Value root, string low_level_target)
{
	bool find = false;
	//缺了省份
	if (level == 1)
	{

		Json::Value::Members member = pcas0.getMemberNames();

		for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
		{
			Json::Value test = pcas0[*iter];
			if (IsMunicipality(*iter, true))
			{
				test = test[*iter];
			}
			if (test.isMember(low_level_target))
			{
				return  *iter;
			}
		}
		member = pcas1.getMemberNames();
		for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
		{
			Json::Value test = pcas1[*iter];
			if (test.isMember(low_level_target))
			{
				return  *iter;
			}
		}
	}
	//缺了市/州
	else if (level == 2)
	{
		Json::Value::Members member = root.getMemberNames();
		for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
		{
			Json::Value test = root[*iter];
			if (test.isMember(low_level_target))
			{
				return  *iter;
			}
		}
	}
	//缺了县/区
	else if (level == 3)
	{
		Json::Value::Members member = root.getMemberNames();
		for (Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
		{
			Json::Value test = root[*iter];
			for (size_t i = 0; i < test.size(); i++)
			{
				if (test[i] == low_level_target)
				{
					return *iter;
				}
			}
		}
	}
	return "";
}
bool IsNum(const char *te)
{
	if (*te >= 48 && *te <= 57)
		return true;
	return false;
}
bool IsChinese(int start)
{
	if (list.substr(start, 2) == "甲")
	{
		return false;
	}
	const char c = list[start];
	const char c_n = list[start + 1];
	if ((c & 0x80) && (c_n & 0x80))
		return true;
	return false;
}
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
	return "";
}

int FindAword(string target, int start, bool sigle = false)
{
	int test = start;
	while (test < list.length())
	{
		if (list.substr(test, 2) == target)
		{
			return test;
		}
		if (sigle)
		{
			test += 1;
		}
		else
		{
			test += 2;
		}
	}
	return 0;
}
int FindWord(string *target, int start, int target_Lenth)
{
	int test = start;
	while (test < list.length())
	{
		for (size_t i = 0; i < target_Lenth; i++)
		{
			if (list.substr(test, 2) == target[i])
			{
				return test;
			}
		}
		test += 2;
	}
	return 0;
}
Json::Value GetAddress(int mode)
{
	bool find[5] = { 0,0,0,0,0 };
	Json::Reader reader;// 解析json用Json::Reader
	Json::Value Address;
	Json::Value city_list;
	Json::Value region_list;
	Json::Value village_list;
	index++;
	string provinces;
	string testname = list.substr(index, 4);
	//index += 4;
	bool special = false;
	string special_city;
	//找省份
	if (IsMunicipality(testname))
	{
		//直辖市
		Address.append(testname);
		Address.append(testname + "市");
		provinces = testname + "市";
		city_list = pcas0[provinces];
		region_list = pcas0[provinces][provinces];
		index += 4;
		find[0] = true;
		find[1] = true;
		if (list.substr(index, 2) == "市")
		{
			index += 2;
		}
	}
	else
	{
		index += 4;
		if (list.substr(index - 4, 6) == "黑龙江")
		{
			provinces = list.substr(index - 4, 6) + "省";
			index += 2;
		}
		else
		{
			bool IsAutonomous = false;
			for (size_t i = 0; i < 5; i++)
			{
				if (testname == AutonomousRegion[i])
				{
					IsAutonomous = true;
					int autonomous_end = FindAword("区", index + 4);
					provinces = list.substr(index - 4, autonomous_end - index + 6);
					index = autonomous_end + 2;
				}
			}
			if (!IsAutonomous)
			{
				provinces = testname + "省";
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
			find[0] = true;
		}
		else if (pcas1.isMember(provinces))
		{
			city_list = pcas1[provinces];
			Address.append(provinces);
			find[0] = true;
		}
		else
		{
			cout << "省级缺失" << endl;
			bool findprovinces = true;
			provinces = provinces.substr(0, 4);
			string lowtest = provinces + "市";
			string test_provinces = LostToSerch(1, NULL, lowtest);
			if (test_provinces == "")
			{
				lowtest = provinces + "区";
				test_provinces = LostToSerch(1, NULL, lowtest);
				if (test_provinces == "")
				{
					index -= 4;
					int te = FindWord(city_end, index, 3);
					if (te != 0)
					{
						lowtest = list.substr(index, te - index + 2);
						test_provinces = LostToSerch(1, NULL, lowtest);
						if (test_provinces == "")
						{
							findprovinces = false;
						}
						else
						{
							index = te + 2;
						}
					}
				}
			}
			if (findprovinces)
			{
				if (list.substr(index, 2) == "区" || list.substr(index, 2) == "市")
				{
					index += 2;
				}
				if (IsMunicipality(test_provinces, true))
				{
					city_list = pcas0[test_provinces];
					region_list = city_list[test_provinces];
					village_list = region_list[lowtest];
					find[2] = true;
					Address.append(test_provinces.substr(0, 4));
				}
				if (pcas0.isMember(test_provinces))
				{
					city_list = pcas0[test_provinces];
				}
				if (pcas1.isMember(test_provinces))
				{
					city_list = pcas1[test_provinces];
				}
				region_list = city_list[lowtest];
				Address.append(test_provinces);
				Address.append(lowtest);
				find[0] = true;
				find[1] = true;
			}
			else
			{

				cout << "省份找不到" << provinces << endl;
			}
		}

	}

	if (find[0] && !find[1])
	{
		//市/区
		int icity = 0;
		while (icity <= 20)
		{
			icity += 2;
			string city_end = list.substr(index + icity, 2);
			string city = list.substr(index, icity);
			if (city_end == "县")
			{
				break;
			}
			string citys = city + "市";
			if (city == "东莞" || city == "中山")
			{
				special = true;
				special_city = citys;
				region_list = city_list[citys];
				village_list = region_list[citys];
				find[1] = true;
				find[2] = true;
				Address.append("");
				Address.append(citys);
			}
			else
			{
				if (city_list.isMember(citys))
				{
					if (list.substr(index + icity, 2) == "市")
					{


						if (list.substr(index + icity, 6) == "市中区")
						{
							index = index + icity;
						}
						else
						{
							index = index + icity + 2;
						}
					}
					else
					{
						index = index + icity;
					}
					region_list = city_list[citys];
					Address.append(citys);
					find[1] = true;
					break;
				}
				else if (city_list.isMember(city))
				{
					index = index + icity;
					region_list = city_list[city];
					Address.append(city);
					find[1] = true;
					break;
				}
			}
		}
		if (!find[1])
		{
			//未找到城市
			//寻找城市
			cout << "未找到城市,寻找城市" << endl;
			int cityfind = FindWord(region_end, index, 3);
			if (cityfind != 0)
			{
				string test_region = list.substr(index, cityfind - index + 2);
				cout << "尝试:" << test_region << endl;
				string test_result = LostToSerch(2, city_list, test_region);
				if (test_result != "")
				{
					cout << "成功找到城市" << test_result << endl;
					//Address.append(test_result);
					if (mode == 3)
					{
						Address.append(test_result);
					}
					else
					{
						Address.append("");
					}
					Address.append(test_region);
					region_list = city_list[test_result];
					village_list = region_list[test_region];
					find[2] = true;
					find[1] = true;
					index = cityfind + 2;
				}
				else
				{
					cout << "城市寻找失败" << endl;
				}
			}
			else
			{
				cout << "城市寻找失败" << endl;
			}
		}
	}

	if (find[0] && find[1] && !find[2])
	{
		int irigion = 0;
		while (index + irigion < list.length())
		{
			irigion += 2;
			string rigion = list.substr(index, irigion);
			if (region_list.isMember(rigion))
			{
				find[2] = true;
				index = index + irigion;
				village_list = region_list[rigion];
				Address.append(rigion);
				//cout << rigion << endl;
				break;
			}
		}
		if (!find[2])
		{
			cout << "未找到县/区" << endl;
			string testvillage;
			int testindex;
			int villagefind = index;
			villagefind += 4;

			while (villagefind < list.length())
			{
				string village_end = list.substr(villagefind, 2);
				if (village_end == "镇" || village_end == "乡" || village_end == "区" || village_end == "道" || village_end == "场" || village_end == "团" || village_end == "城")
				{
					testvillage = list.substr(index, villagefind - index + 2);
					testindex = villagefind;
					cout << "尝试:" << testvillage << endl;
					string test_result = LostToSerch(3, region_list, testvillage);
					if (test_result != "")
					{
						//Address.append(test_result);
						if (mode == 3)
						{
							Address.append(test_result);
						}
						else
						{
							Address.append("");
						}
						Address.append(testvillage);
						village_list = region_list[test_result];
						find[2] = true;
						find[3] = true;
						index = villagefind + 2;
						break;
					}
					else
					{
						villagefind += 2;
					}
				}
				else
				{
					villagefind += 2;
				}
			}
			/*index = testindex + 2;
			Address.append(testvillage);*/
		}
	}
	if (find[0] && find[1] && find[2] && !find[3])
	{
		//精确搜索
		//乡/镇/街道
		int ivillage = 0;
		while (index + ivillage < list.length())
		{
			ivillage += 2;
			string village = list.substr(index, ivillage);

			for (size_t i = 0; i < village_list.size(); i++)
			{
				if (village_list[i] == village)
				{
					index = index + ivillage;
					Address.append(village);
					find[3] = true;
					break;
				}
			}
			if (find[3])
			{
				break;
			}
		}
		if (!find[3])
		{
			Address.append("");
		}
	}
	if (mode == 1)
	{
		//5级地址分割已完成
		string ds = list.substr(index, list.length() - index - 1);//去掉句号
		//cout << ds << endl;
		Address.append(ds);
	}
	else
	{
		//7级地址分割
		bool find_num = true;
		//寻找路/街/巷
		int f_end = index;
		int s_end = f_end;
		int lastf_end = f_end;
		int tre = 0;
		while (true)
		{
			tre++;
			if (tre >= 100)
			{
				cout << 1;
			}
			f_end = FindWord(five_end, f_end, 6);
			if (f_end == 0)
			{
				find[4] = false;
				f_end = lastf_end;
				s_end = f_end;
				break;
			}
			f_end += 2;
			lastf_end = f_end;
			if (!IsChinese(f_end))
			{
				find[4] = true;
				break;
			}
		}
		if (find[4])
		{
			s_end = f_end;
			//寻找号/弄
			while (!IsChinese(s_end))
			{
				s_end++;
			}
			string test_six = list.substr(s_end, 2);
			if (test_six == "号" || test_six == "弄")
			{
				//满足条件
				s_end += 2;
			}
			else
			{
				//不满足条件
				s_end = f_end;
			}
		}

		string Lfive = list.substr(index, f_end - index);
		string Lsix = list.substr(f_end, s_end - f_end);
		string Lseven = list.substr(s_end, list.length() - s_end - 1);
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
void dsda(string op, int t)
{
	Json::Reader reader;
	stringstream  dastr0;
	stringstream  dastr1;
	string ds;
	ifstream is1(op);
	if (!is1.is_open())		cout << "answer1500.txt" << endl;
	dastr1 << is1.rdbuf();
	ds = dastr1.str();
	if (t == 1)
	{
		if (!reader.parse(ds, ans)) { cout << "无法读取json1" << endl; }
	}
	else
	{
		if (!reader.parse(ds, test)) { cout << "无法读取json1" << endl; }
	}
	is1.close();
	dastr1.clear();
}
int main(int argc, char** argv)
{

	if (argc < 2)
	{
		return 0;
	}
	ReadMapOfChina();
	Json::Value root;
	ifstream infile(argv[1]);

	bool frist_list = true;
	while (infile >> list)
	{
		testnum++;

		list = Utf8ToGbk(list.data());

		Json::Value a_list;

		analysis = list.data();
		index = 0;
		int mode = *analysis - 48;
		if (testnum == 25)
		{
			testnum = testnum;
		}
		a_list["姓名"] = GetName();
		a_list["手机"] = GetPhone();
		a_list["地址"] = GetAddress(mode);
		root.append(a_list);
	}
	infile.close();
	ofstream out(argv[2]);
	//ofstream out(outf);
	string re = root.toStyledString();
	re = GbkToUtf8(re.data());
	out << re;
	out.close();
	return 0;
}
