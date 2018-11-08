#include <stdio.h>
#include <time.h>
 
// Check given year is leap year or not
int isLeapYear(int year, int mon){
    int flag = 0;
    if (year % 100 == 0){
            if (year % 400 == 0){
                    if (mon == 2){
                            flag = 1;
                    }
            }
    } 
    else if (year % 4 == 0){
            if (mon == 2){
                    flag = 1;
            }
    }
    return (flag);
}
 
int calculate_age_horoscope(int day, int month, int year){
 
    int DaysInMon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char dob[100];
    time_t ts;
    struct tm *ct;
 
    /* enter date of birth */
//    printf("Enter your date of birth (DD/MM/YYYY): ");
  //  scanf("%d/%d/%d",&day,&month, &year);
 
    /*get current date.*/
    ts = time(NULL);
    ct = localtime(&ts);
 
    printf("Current Date: %d/%d/%d\n",
            ct->tm_mday, ct->tm_mon + 1, ct->tm_year + 1900);
 
    day = DaysInMon[month - 1] - day + 1;
 
    /* leap year checking*/
    if (isLeapYear(year, month)){
            day = day + 1;
    }
 
    /* calculating age in no of day, years and months */
    day = day + ct->tm_mday;
    month = (12 - month) + (ct->tm_mon);
    year = (ct->tm_year + 1900) - year - 1;
 
    /* checking for leap year feb - 29 day */
    if (isLeapYear((ct->tm_year + 1900), (ct->tm_mon + 1))){
            if (day >= (DaysInMon[ct->tm_mon] + 1)){
                    day = day - (DaysInMon[ct->tm_mon] + 1);
                    month = month + 1;
            }
    } 
    else if (day >= DaysInMon[ct->tm_mon]){
            day = day - (DaysInMon[ct->tm_mon]);
            month = month + 1;
    }
 
    if (month >= 12){
            year = year + 1;
            month = month - 12;
    }
 
    /* print age */
    printf("\n## Ola, voce e %d anos %d meses e %d dias old. ##\n", year, month, day);
 
    return 0;
}