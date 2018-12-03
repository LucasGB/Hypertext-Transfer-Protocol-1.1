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
 
int main(int argc, char** argv[]){

    int day = atoi(argv[1]);
    int month = atoi(argv[2]);
    int year = atoi(argv[3]);

    int DaysInMon[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char dob[100];
    time_t ts;
    struct tm *ct;
  
    /*get current date.*/
    ts = time(NULL);
    ct = localtime(&ts);
 
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
    printf("\nOla, voce tem %d anos %d meses e %d dias de vida.\n", year, month, day);
 
    return 0;
}