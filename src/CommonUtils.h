#ifndef OLED_CLOCK_COMMONUTILS_H
#define OLED_CLOCK_COMMONUTILS_H


class CommonUtils {
public:
    static bool isLeapYear(int year);

    static int getDaysInMonth(int year, int month);

    static int calculateWeekday(int year, int month, int day);
};


#endif //OLED_CLOCK_COMMONUTILS_H
