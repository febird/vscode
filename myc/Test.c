#include <stdio.h>
int main(int argc, char *argv)
{
    int i = 0, j = 0, l = 20;
    for (i = 0; i < l; i++)
    {
        for (j = 0; j < l + i; j++)
            if (j < l - i)
                printf(" ");
            else
                printf("*");
        printf("\r\n");
    }
    return 0;
}