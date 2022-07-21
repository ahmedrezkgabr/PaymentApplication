#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "../Card/card.h"
#include "../Terminal/terminal.h"
#include "server.h"

#define MAX_ACCOUNTS_SIZE 255
#define MAX_TRANSACTIONS_SIZE 255

/*represent the database with some initial values*/
ST_accountDB_t accounts[MAX_ACCOUNTS_SIZE] = {{76342164, "4265149190086014"},
                                              {1000, "4010007434380361"},
                                              {324134, "4036493292234835"},
                                              {132455, "4001310725317428"},
                                              {12345, "4101012665149063"}};

ST_accountDB_t stolen[MAX_ACCOUNTS_SIZE] = {{76342164, "4009589055120286"},
                                            {1000, "4479650940427673"}};

uint32_t nextSeq = 2; /*this indecates the next sequnce number*/
uint32_t stolenIndex = 0;

/*represent the database with some initial values*/
ST_transaction_t transactions[MAX_TRANSACTIONS_SIZE] = {{{"ahmed rezk gabr almogh", "4265149190086014", "09/23"}, {100.0f, 3000.0f, "15/07/2022"}, APPROVED, 0},
                                                        {{"mariam awaadalla younn", "4010007434380361", "10/24"}, {500.0f, 3000.0f, "17/07/2022"}, APPROVED, 1}};

void reportStolenCard()
{
    ST_cardData_t card;
    getCardPAN(&card);
    strcpy(stolen[stolenIndex].primaryAccountNumber, card.primaryAccountNumber);
}
static void setAccount(uint8_t *PAN, ST_accountDB_t *account)
{
    for (uint32_t i = 0; i < MAX_ACCOUNTS_SIZE; i++)
    {
        /*find the element through its PAN*/
        if (strcmp(PAN, accounts[i].primaryAccountNumber) == 0)
        {
            /*update the array element with the data at the buffer*/
            accounts[i].balance = account->balance;
            break;
        }
    }
    /*no error of not finding the element as i found it before and this fuction is static used only in this porpus*/
}

static void getAccount(uint8_t *PAN, ST_accountDB_t *account)
{
    /*stop when PAN is null*/
    for (uint32_t i = 0; i < MAX_ACCOUNTS_SIZE; i++)
    {
        /*find the element through its PAN*/
        if (strcmp(PAN, accounts[i].primaryAccountNumber) == 0)
        {
            /*update the buffer with the data at the array element*/
            account->balance = accounts[i].balance;
            break;
        }
    }
    /*no error of not finding the element as i found it before and this fuction is static used only in this porpus*/
}

static void updateAccountBalance(ST_transaction_t *transData)
{
    /*buffer to store data*/
    ST_accountDB_t *account = (ST_accountDB_t *)malloc(sizeof(ST_accountDB_t));

    /*file the buffer depending on the PAN*/
    getAccount(transData->cardData.primaryAccountNumber, account);

    /*update the palance of the buffer*/
    account->balance -= transData->terminalData.transAmount;

    printf("Your New Balance Is: %.2f\n", account->balance);

    /*update the element in the dynamic array with the new buffer data*/
    setAccount(transData->cardData.primaryAccountNumber, account);
    free(account);
}

EN_transState_t recieveTransactionData(ST_transaction_t *transData)
{

    /*getting card's info*/
    getCardHolderName(&(transData->cardData));
    getCardExpiryDate(&(transData->cardData));
    getCardPAN(&(transData->cardData));

    /*initialise the max amount and getting the terminal's information*/
    setMaxAmount(&(transData->terminalData));
    getTransactionDate(&(transData->terminalData));

    /*check if the card is expired or not*/
    if (isCardExpired(transData->cardData, transData->terminalData) == EXPIRED_CARD)
    {
        transData->transState = DECLINED_EXPIRED_CARD;
        return DECLINED_EXPIRED_CARD;
    }

    /*get the amount wanted*/
    getTransactionAmount(&(transData->terminalData));

    /*check if the wanted amount is allowed to be taken or not*/
    if (isBelowMaxAmount(&(transData->terminalData)) == EXCEED_MAX_AMOUNT)
    {
        transData->transState = DECLINED_EXCEEDING_AMOUNT;
        return DECLINED_EXCEEDING_AMOUNT;
    }

    transData->transState = APPROVED;

    /*check if the account is in the database (is a valid) or not*/
    if (isValidAccount(&(transData->cardData)) == ACCOUNT_NOT_FOUND)
    {
        transData->transState = DECLINED_STOLEN_CARD;
        return DECLINED_STOLEN_CARD;
    }

    /*check if the amount is available or not*/
    if (isAmountAvailable(transData) == LOW_BALANCE)
    {
        transData->transState = DECLINED_INSUFFECIENT_FUND;
        return DECLINED_INSUFFECIENT_FUND;
    }

    /*this update is in the datastructure*/
    updateAccountBalance(transData);

    /*saving the transaction in the database*/
    if (saveTransaction(transData) == SAVING_FAILD)
    {
        transData->transState = INTERNAL_SERVER_ERROR;
        return INTERNAL_SERVER_ERROR;
    }

    return APPROVED;
}

EN_serverError_t isValidAccount(ST_cardData_t *cardData)
{
    /*search if it is in the stolen cards*/
    for (uint32_t i = 0; i < MAX_ACCOUNTS_SIZE; i++)
    {
        if (strcmp(stolen[i].primaryAccountNumber, cardData->primaryAccountNumber) == 0)
            return ACCOUNT_NOT_FOUND;
    }

    /*search for a giving PAN in the datastructure (database copy)*/
    EN_serverError_t result = ACCOUNT_NOT_FOUND;
    for (uint32_t i = 0; i < MAX_ACCOUNTS_SIZE; i++)
    {
        if (strcmp(cardData->primaryAccountNumber, accounts[i].primaryAccountNumber) == 0)
        {
            result = OK;
            break;
        }
    }
    return result;
}

EN_serverError_t isAmountAvailable(ST_transaction_t *transData)
{

    EN_serverError_t result = LOW_BALANCE;

    /*it is a buffer to hold the data*/
    ST_accountDB_t *account = (ST_accountDB_t *)malloc(sizeof(ST_accountDB_t));

    /*getting the information of the account*/
    getAccount(transData->cardData.primaryAccountNumber, account);

    /*check if the balance cover the amount or not*/
    if (account->balance >= transData->terminalData.transAmount)
        result = OK;

    free(account);
    return result;
}

EN_serverError_t saveTransaction(ST_transaction_t *transData)
{
    /*so the transactions is full*/
    if (nextSeq > MAX_TRANSACTIONS_SIZE)
    {
        return SAVING_FAILD;
    }

    /*savjing card's info*/
    strcpy(transactions[nextSeq].cardData.cardHolderName, transData->cardData.cardHolderName);
    strcpy(transactions[nextSeq].cardData.primaryAccountNumber, transData->cardData.primaryAccountNumber);
    strcpy(transactions[nextSeq].cardData.cardExpirationDate, transData->cardData.cardExpirationDate);

    /*saving terminal's info*/
    strcpy(transactions[nextSeq].terminalData.transactionDate, transData->terminalData.transactionDate);
    transactions[nextSeq].terminalData.transAmount = transData->terminalData.transAmount;
    transactions[nextSeq].terminalData.maxTransAmount = transData->terminalData.maxTransAmount;

    /*saving transaction's info*/
    transactions[nextSeq].transactionSequenceNumber = nextSeq;
    transactions[nextSeq].transState = transData->transState;

    /*for the next transaction*/
    nextSeq++;

    return OK;
}

EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t *transData)
{
    /*so this sequnce number is not been used*/
    if (transactionSequenceNumber >= nextSeq)
        return TRANSACTION_NOT_FOUND;

    /*getting card's info*/
    strcpy(transData->cardData.cardHolderName, transactions[transactionSequenceNumber].cardData.cardHolderName);
    strcpy(transData->cardData.primaryAccountNumber, transactions[transactionSequenceNumber].cardData.primaryAccountNumber);
    strcpy(transData->cardData.cardExpirationDate, transactions[transactionSequenceNumber].cardData.cardExpirationDate);

    /*getting terminal's info*/
    strcpy(transData->terminalData.transactionDate, transactions[transactionSequenceNumber].terminalData.transactionDate);
    transData->terminalData.maxTransAmount = transactions[transactionSequenceNumber].terminalData.maxTransAmount;
    transData->terminalData.transAmount = transactions[transactionSequenceNumber].terminalData.transAmount;

    /*getting transaction's info*/
    transData->transactionSequenceNumber = transactionSequenceNumber;
    transData->transState = transactions[transactionSequenceNumber].transState;

    return OK;
}