#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "../Card/card.h"
#include "terminal.h"

#define MAX_AMOUNT 3000
static void getCurrentDate(uint8_t *str)
{
    /*
    this function is to get the current date in wanted format
    this function is not allowed to be used outside this file
    */
    uint8_t *slash = "/";

    time_t date = time(NULL);
    struct tm tm = *localtime(&date);

    uint8_t *year = (uint8_t *)malloc(4);
    uint8_t *mon = (uint8_t *)malloc(2);
    uint8_t *day = (uint8_t *)malloc(2);

    sprintf(year, "%d", tm.tm_year + 1900);
    sprintf(mon, "%d", tm.tm_mon + 1);
    sprintf(day, "%d", tm.tm_mday);

    strcat(str, day);
    strcat(str, slash);
    strcat(str, mon);
    strcat(str, slash);
    strcat(str, year);
    
}

static uint8_t isLuhn(uint8_t *str)
{
    /*
    this function is to check if the number in str is a Luhn or not
    this function is not allowed to be used outside this file
    */
    uint32_t digitsNumber = strlen(str);
    uint32_t sum = 0, isSecond = false;

    for (int i = digitsNumber - 1; i >= 0; i--)
    {
        // getting the numerical value of the digit
        uint32_t d = str[i] - '0';

        if (isSecond == true)
            d *= 2;

        sum += d / 10; // tens
        sum += d % 10; // ones

        isSecond = !isSecond;
    }
    return (sum % 10 == 0);
}

EN_teminalError_t getTransactionDate(ST_terminalData_t *termData)
{

    getCurrentDate(termData->transactionDate);

    return OK;
}

EN_teminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
    EN_teminalError_t result = EXPIRED_CARD;

    int cardMonth, cardYear, termDay, termMonth, termYear;
    sscanf(cardData.cardExpirationDate, "%d/%d", &cardMonth, &cardYear);
    sscanf(termData.transactionDate, "%d/%d/%d", &termDay, &termMonth, &termYear);
    cardYear += 2000;
    if(cardYear > termYear)
        result = OK;
    else if(cardYear == termYear)
    {
        if(cardMonth > termMonth)
            result = OK;
    }

    return result;
}

EN_teminalError_t isValidCardPAN(ST_cardData_t *cardData)
{
    EN_teminalError_t result = INVALID_CARD;

    if (isLuhn(cardData->primaryAccountNumber))
        result = OK;

    return result;
}

EN_teminalError_t getTransactionAmount(ST_terminalData_t *termData)
{
    float amount;
    EN_teminalError_t result = INVALID_AMOUNT;

    while (true)
    {
        printf("Please Enter The Transaction Amount: ");
        scanf("%f", &amount);

        // check if it is a valid amount
        if (amount > 0)
        {
            result = OK;
            termData->transAmount = amount;
            break;
        }
        printf("WRONG AMOUNT!!!\n");
    }

    return result;
}

EN_teminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
    EN_teminalError_t result = EXCEED_MAX_AMOUNT;
    if (termData->transAmount <= termData->maxTransAmount)
        result = OK;

    return result;
}

EN_teminalError_t setMaxAmount(ST_terminalData_t *termData)
{

    EN_teminalError_t result = INVALID_AMOUNT;

    termData->maxTransAmount = MAX_AMOUNT;
    if (MAX_AMOUNT > 0)
        result = OK;

    return result;
}
