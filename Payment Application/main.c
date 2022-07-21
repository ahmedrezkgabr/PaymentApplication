#include <stdio.h>
#include <stdbool.h>
#include "Application/app.h"

int main()
{
    bool flag = true;
    char choice;
    while (flag)
    {
        appStart();
        printf("\nAnother Transaction?(y/n): ");

        scanf(" %c", &choice);

        if (choice == 'n' || choice == 'N')
        {
            flag = false;
        }
    }
    printf("Thank you\n");
    return 0;
}