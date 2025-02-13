#include <mcs/cal.h>

BOOL CALIsLeapYear(unsigned int year)
{
	if((year % 400) == 0) {
		return TRUE;
	} else if((year % 100) == 0) {
		return FALSE;
	} else {
		return (year % 4) == 0;
	}
}

unsigned int CALGetDaysOfMonth(unsigned int year, unsigned int month)
{
	switch(month) {
		case 1: /* JAN */
			return 31;
		case 2: /* FEB */
			return 28 + (CALIsLeapYear(year) ? 1 : 0);
		case 3: /* MAR */
			return 31;
		case 4: /* APR */
			return 30;
		case 5: /* MAY */
			return 31;
		case 6: /* JUN */
			return 30;
		case 7: /* JUL */
			return 31;
		case 8: /* AUG */
			return 31;
		case 9: /* SEP */
			return 30;
		case 10: /* OCT */
			return 31;
		case 11: /* NOV */
			return 30;
		case 12: /* DEC */
			return 31;
		default:
			return 30;
	}
}

/* compute weekday (0=Monday, 6=Sunday) for day/month/year
 * (day=1..31, month=1..12) */

int CALGetWeekday(int year, int month, int day)
{
	int m = month < 3 ? month + 12 : month;
	int Y = month < 3 ? year - 1 : year;

	int w = day + (13 * (m + 1)) / 5 + Y + Y / 4 - Y / 100 + Y / 400;

	return (w + 5) % 7;
}
