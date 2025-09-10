#include <stdio.h>
#include <stdlib.h>
void printarr(int *arr, int N, int M)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d ", *(arr + i * M + j));
        }
        printf("\n");
    }
}
int main()
{
    int N, M;
    printf("N = ");
    scanf("%d", &N);

    printf("M = ");
    scanf("%d", &M);

    int (*arr)[M] = malloc(sizeof(int[N][M]));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("Arr[%d][%d] = ", i, j);
            scanf("%d", &arr[i][j]);
        }
    }

    int maxOdd = arr[0][0];
    int maxEven = arr[0][0];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (arr[i][j] % 2 == 0)
            {
                if (arr[i][j] > maxEven)
                {
                    maxEven = arr[i][j];
                }
            }
            else
            {
                if (arr[i][j] > maxOdd)
                {
                    maxOdd = arr[i][j];
                }
            }
        }
    }

    if (maxEven % 2 == 1)
    {
        printf("No even number\n");
    }
    else
    {
        printf("max even number : %d\n", maxEven);
    }
    if (maxOdd % 2 == 0)
    {
        printf("No odd number\n");
    }
    else
    {
        printf("max odd number : %d\n", maxOdd);
    }

    return 0;
}