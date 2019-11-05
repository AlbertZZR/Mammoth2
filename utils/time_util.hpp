#pragma once

#include "std_header.h"

namespace mammoth{
    enum TIME_ITEM { YEAR, MONTH, DATE, HOUR, MINUTE, SECOND, DAY, MILLSECOND };
	class MTimeUtil {
	public:
		static std::string get_time_str();
		static int get_time(const TIME_ITEM&);
		static std::string get_time_code();
		static std::string get_time_code_millsecond();
	private:
		static SYSTEMTIME sys_time;

	};
};

SYSTEMTIME mammoth::MTimeUtil::sys_time;

std::string mammoth::MTimeUtil::get_time_str() {
	std::string time_str;
	char temp[40];
	memset(temp, 0, sizeof(char) * 40);
	GetLocalTime(&sys_time);
	sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d.%03d", sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
	time_str = temp;
	printf("%s\n", temp);
	return time_str;
}

int mammoth::MTimeUtil::get_time(const mammoth::TIME_ITEM& time_item) {
	GetLocalTime(&sys_time);
	switch (time_item) {
	case mammoth::TIME_ITEM::YEAR:{
		return sys_time.wYear;
	}
	case mammoth::TIME_ITEM::MONTH:{
		return sys_time.wMonth;
	}
	case mammoth::TIME_ITEM::DATE:{
		return sys_time.wDay;
	}
	case mammoth::TIME_ITEM::HOUR:{
		return sys_time.wHour;
	}
	case mammoth::TIME_ITEM::MINUTE:{
		return sys_time.wMinute;
	}
	case mammoth::TIME_ITEM::SECOND:{
		return sys_time.wSecond;
	}
	case mammoth::TIME_ITEM::MILLSECOND:{
		return sys_time.wMilliseconds;
	}
	case mammoth::TIME_ITEM::DAY:{
		return sys_time.wDayOfWeek;
	}
	}
	return 0;
}

std::string mammoth::MTimeUtil::get_time_code() {
	std::string time_str;
	char temp[40];
	memset(temp, 0, sizeof(char) * 40);
	GetLocalTime(&sys_time);
	sprintf(temp, "%04d%02d%02d%02d%02d%02d", sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
	time_str = temp;
	return time_str;
}

std::string mammoth::MTimeUtil::get_time_code_millsecond() {
	std::string time_str;
	char temp[40];
	memset(temp, 0, sizeof(char) * 40);
	GetLocalTime(&sys_time);
	sprintf(temp, "%04d%02d%02d%02d%02d%02d%03d", sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
	time_str = temp;
	return time_str;
}