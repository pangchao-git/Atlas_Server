/*
@author		fyu
@date		2015/12/26
@function	sqlite×Ö·û´®£¬Ê±¼ä×Ö·û´®µÄ×ª»»º¯Êý
*/

#pragma once
#include <string>

std::string ConvertStdStringToSqliteText(std::string std_string);
std::string ConverIsoTimeStringToSqliteTimeString(std::string iso_string);
std::string ConverSqliteTimeStringToIsoTimeString(std::string sqllite_datetime);