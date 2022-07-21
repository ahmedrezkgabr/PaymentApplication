#ifndef TERMINAL__H_
#define TERMINAL__H_

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

typedef struct ST_terminalData_t
{
    float transAmount;
    float maxTransAmount;
    uint8_t transactionDate[11];
} ST_terminalData_t;

typedef enum EN_teminalError_t
{
    //OK,
    WRONG_DATE = 1,
    EXPIRED_CARD,
    INVALID_CARD,
    INVALID_AMOUNT,
    EXCEED_MAX_AMOUNT,
    INVALID_MAX_AMOUNT
} EN_teminalError_t;

EN_teminalError_t getTransactionDate(ST_terminalData_t *);
EN_teminalError_t isCardExpired(ST_cardData_t, ST_terminalData_t);
EN_teminalError_t isValidCardPAN(ST_cardData_t *);
EN_teminalError_t getTransactionAmount(ST_terminalData_t *);
EN_teminalError_t isBelowMaxAmount(ST_terminalData_t *);
EN_teminalError_t setMaxAmount(ST_terminalData_t *);

#endif