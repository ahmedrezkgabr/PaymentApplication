#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "card.h"

#define MAX_NAME_SIZE 30
#define EXP_DATE_SIZE 6
#define MAX_PAN_SIZE 20
static bool isRightFormat(char *str)
{
    // the right format is "number + number + '/' + number + number"
    if (!isalnum(str[0]) && !isalnum(str[1]) && !isalnum(str[3]) && !isalnum(str[4]) && !str[2] != '/')
        return false;

    return true;
}

static uint8_t isAlphanumeric(char *str)
{
    /*
    this function is to check if the PAN in the right format or not
    this function is not allowed to be used outside this file
    */

    // iterate over all digits and if one is not a number it will return 0 and if it is ended the loop without detecting any unnumic digit it will return 1
    for (int i = 0; str[i]; i++)
        if (!isalnum(str[i]))
            return 0;

    return 1;
}

EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{
    uint8_t *str = (uint8_t *)malloc(MAX_NAME_SIZE * sizeof(uint8_t));
    uint8_t len = 0;
    EN_cardError_t result = WRONG_NAME;

    while (1)
    {
        printf("Please Enter The Card Holder's Name: ");
        fgets(str, MAX_NAME_SIZE, stdin);
        len = strlen(str);

        // check if it is a valid holder name
        if (str && len >= 20 && len <= 24)
        {
            result = OK;
            strcpy(cardData->cardHolderName, str);
            break;
        }
        printf("WRONG HOLDER'S NAME!!!\n");
    }
    free(str);
    return result;
}

EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
    uint8_t *str = (uint8_t *)malloc(EXP_DATE_SIZE * sizeof(uint8_t));
    uint8_t len = 0;
    EN_cardError_t result = WRONG_EXP_DATE;

    while (1)
    {
        printf("Please Enter The Card's Expiry Date 'MM/YY': ");
        scanf("%s", str);
        len = strlen(str);

        // check if it is a valid ex date
        if (str && len == 5 && isRightFormat(str))
        {
            result = OK;
            strcpy(cardData->cardExpirationDate, str);
            break;
        }
        printf("WRONG Expiry Date!!!\n");
    }
    free(str);
    return result;
}

EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
    uint8_t *str = (uint8_t *)malloc(MAX_PAN_SIZE * sizeof(uint8_t));
    uint8_t len = 0;
    EN_cardError_t result = WRONG_PAN;

    while (true)
    {
        printf("Please Enter The Card's Primary Account Number (PAN): ");
        scanf("%s", str);
        len = strlen(str);

        // check if it is a valid pan
        if (str && len >= 16 && len <= 19 && isAlphanumeric(str))
        {
            result = OK;
            strcpy(cardData->primaryAccountNumber, str);
            break;
        }
        printf("WRONG PAN!!!\n");
    }
    free(str);
    return result;
}