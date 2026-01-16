#include "CommonUtils.h"

bool CommonUtils::isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    }
    return false;
}

int CommonUtils::getDaysInMonth(int year, int month) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    if (month == 1 || month == 3 || month == 5 || month == 7 ||
        month == 8 || month == 10 || month == 12) {
        return 31;
    }
    return 30;
}

// 函数：计算指定年月日对应的星期几
// 参数：year(年)、month(月1-12)、day(日1-31)
// 返回：0=周一，1=周二，...，6=周日（可自定义映射）
int CommonUtils::calculateWeekday(int year, int month, int day) {
    // 步骤1：修正1月和2月（转为上一年的13、14月）
    if (month < 3) {
        year -= 1;
        month += 12;
    }

    // 步骤2：代入基姆拉尔森计算公式
    return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
}
