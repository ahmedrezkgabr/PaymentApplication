#include <stdio.h>
#include <stdlib.h>
#include "app.h"
#include "../Server/server.h"

void appStart(void)
{
    uint32_t c = 1;
    printf("1 for doing a transaction\n2 for reporting stolen card\nyour choice: ");
    scanf("%d", &c);
    if (c == 1)
    {
        /*initialise the struct instance with defualt values*/
        ST_transaction_t transaction = {{"", "", ""}, {0.0f, 0.0f, ""}, 0, 0};
        EN_serverError_t flag = recieveTransactionData(&transaction);
        switch (flag)
        {
        case APPROVED:
            printf("APPROVED\n");
            break;
        case DECLINED_EXPIRED_CARD:
            printf("EXPIRED CARD\n");
            break;
        case DECLINED_EXCEEDING_AMOUNT:
            printf("EXCEEDING THE LIMIT\n");
            break;
        case DECLINED_INSUFFECIENT_FUND:
            printf("INSUFFECIENT FUND\n");
            break;
        case DECLINED_STOLEN_CARD:
            printf("STOLEN CARD\n");
            break;
        case INTERNAL_SERVER_ERROR:
            printf("ERROR\n");
            break;

        default:
            printf("error!!!\n");
            break;
        }
    }
    else if(c == 2)
    {
        reportStolenCard();
    }
}
