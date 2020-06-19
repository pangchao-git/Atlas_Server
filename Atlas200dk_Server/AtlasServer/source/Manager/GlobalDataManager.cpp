/*
 * @Description  : global data manager
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-08 15:29:13
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-09 16:24:34
 */
#include "GlobalDataManager.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
using namespace rapidxml;

GlobalDataManager *GlobalDataManager::GetInstance()
{
    static GlobalDataManager info;
    return &info;
}

string GlobalDataManager::GetParameter(const string &stKey, const string &stSubKey, const string &stDefValue)
{
    string stRet;
    string stMapKey = stSubKey.empty()?stKey:(stKey + "/" + stSubKey);
    std::transform(stMapKey.begin(), stMapKey.end(), stMapKey.begin(), ::toupper); //将小写的都转换成大写
    MAP_PARAMETER::iterator it = mapParameter.find(MAP_PARAMETER::key_type(stMapKey));
    if (it != mapParameter.end()) {
        stRet = it->second;
    } else {
        stRet = stDefValue;
    }
    return stRet;
}

bool GlobalDataManager::ReadFromXml(const char *stXml)
{
    bool bRet = false;
    if (!stXml)
        return false;

    mapParameter.clear();
    xml_document<> doc;
    try
	{
        rapidxml::file<> fdoc(stXml);   //读取数据
        doc.parse<0>(fdoc.data());
		xml_node<>* rootnode = doc.first_node();
		xml_node<>* node = rootnode->first_node();
		while(node)
		{
            xml_node<>* subnode = node->first_node();
            if (!subnode) {
                Insert2ParameterMap((const char *)node->name(), "",
                                    (const char *)node->value());
                node = node->next_sibling();
                continue;
            }

            // element
            while(subnode) {
                if (subnode->type() == node_element)
                    Insert2ParameterMap((const char *)node->name(), (const char *)subnode->name(),
                                (const char *)subnode->value());
                subnode = subnode->next_sibling();
            }
            node = node->next_sibling();
        }
        bRet = true;
    } catch(parse_error & e) {
        std::cout << "rapidxml Parse error: " << e.what() << std::endl << "At: " << e.where<char>() << std::endl;
    } catch(runtime_error & e) {
        std::cout << "rapidxml runtime_error: " << e.what() << std::endl;
    } catch(exception & e) {
        std::cout << "rapidxml exception: " << e.what() << std::endl;
    }
    return bRet;
}

void GlobalDataManager::WriteToXml(const char *stXml)
{
    if (!stXml)
        return;
}

void GlobalDataManager::Insert2ParameterMap(const char *szKey, const char *szSubKey, const char *szValue)
{
    if (szKey) {
        std::string _strkey = szKey?szKey:"";
        std::string _strSubkey = szSubKey?szSubKey:"";	    
        if (szSubKey && !_strSubkey.empty()) {
            _strkey.append("/");
            _strkey.append(_strSubkey);
        }
        std::transform(_strkey.begin(), _strkey.end(), _strkey.begin(), ::toupper); //将小写的都转换成大写
        std::string _strValue = szValue?szValue:"";
        mapParameter.insert(MAP_PARAMETER::value_type(_strkey, _strValue));
        //fprintf(stdout, "%s: %s.\n", _strkey.c_str(), _strValue.c_str());
    }
}



