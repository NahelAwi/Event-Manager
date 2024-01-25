#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include <stdbool.h>
#include <string.h>

static bool dayIsValid(int day);
static bool monthIsValid(int month);
static int dateToDays(Date date);
static int monthToInt (char* month);

#define MONTH_LEN 4
#define MIN_DAYS 1
#define MAX_DAYS 30
#define DAYS_IN_YEAR 365
#define NUMBER_OF_MONTHS 12
#define INVALID_MONTH -1

struct Date_t{
    int day;
    char month[MONTH_LEN];
    int year;
};

static const char* const months[NUMBER_OF_MONTHS] = {
		"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
		"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

Date dateCreate(int day, int month, int year){
    if(!dayIsValid(day) || !monthIsValid(month)){
        return NULL;
    }
    Date date = malloc(sizeof(*date));
    if(date == NULL){
        return NULL;
    }

    date->day = day;
    strcpy(date->month, months[month-1]);
    date->year = year;
    return date;
}

static bool dayIsValid(int day){
    return (day >= MIN_DAYS && day <= MAX_DAYS);
}

static bool monthIsValid(int month){
    return (month >= 1 && month <= NUMBER_OF_MONTHS);
}


static int dateToDays(Date date){
    return (date->year*DAYS_IN_YEAR + monthToInt(date->month)*MAX_DAYS + date->day);
}

void dateDestroy(Date date){
    free(date);
}

Date dateCopy(Date date){    
    if(date ==NULL){
        return NULL;
    }

    Date new_date = dateCreate(date->day, monthToInt(date->month), date->year);
    if (new_date == NULL){
        return NULL;
    }
    return new_date;
}

static int monthToInt (char* month){
    for (int i = 0; i < NUMBER_OF_MONTHS; i++) {
		if (strcmp(month, months[i]) == 0){
			return i+1;
        }
    }
	return INVALID_MONTH;
}

bool dateGet(Date date, int* day, int* month, int* year){
    if( date == NULL || day == NULL || month == NULL || year == NULL){
        return false;
    }
    
    *day = date->day;
    *month = monthToInt(date->month);
    *year = date->year;
    return true;
}

int dateCompare(Date date1, Date date2){
    if( date1 == NULL || date2 == NULL){
        return 0;
    }

    return(dateToDays(date1) - dateToDays(date2));
}

void dateTick(Date date){
    if(date == NULL){
        return;
    }
    if(date->day != MAX_DAYS){
        ++date->day;
        return;
    }
    date->day = MIN_DAYS;
    if(monthToInt(date->month) != NUMBER_OF_MONTHS){
        strcpy(date->month, months[monthToInt(date->month)]);
        return;
    }
    ++date->year; 
    strcpy(date->month,months[0]);
    return;
    }

