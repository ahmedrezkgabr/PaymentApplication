#ifndef SERVER__H_
#define SERVER__H_


#include "../Card/card.h"
#include "../Terminal/terminal.h"
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

typedef enum EN_transState_t
{
    APPROVED,
    DECLINED_EXPIRED_CARD,
    DECLINED_EXCEEDING_AMOUNT,
    DECLINED_INSUFFECIENT_FUND,
    DECLINED_STOLEN_CARD,
    INTERNAL_SERVER_ERROR
} EN_transState_t;

typedef struct ST_transaction_t
{
    ST_cardData_t cardData;
    ST_terminalData_t terminalData;
    EN_transState_t transState;
    uint32_t transactionSequenceNumber;
} ST_transaction_t;

typedef struct ST_accountDB_t
{
    float balance;
    uint8_t primaryAccountNumber[20];
} ST_accountDB_t;

typedef enum EN_serverError_t
{
    // OK,
    SAVING_FAILD = 1,
    TRANSACTION_NOT_FOUND,
    ACCOUNT_NOT_FOUND,
    LOW_BALANCE
} EN_serverError_t;

EN_transState_t recieveTransactionData(ST_transaction_t *);
EN_serverError_t isValidAccount(ST_cardData_t *);
EN_serverError_t isAmountAvailable(ST_transaction_t *);
EN_serverError_t saveTransaction(ST_transaction_t *);
EN_serverError_t getTransaction(uint32_t, ST_transaction_t *);
void reportStolenCard(void);
#endif