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

    printarr(arr, N, M);

    int sum;
    for (int i = 0; i < N; i++)
    {
        sum = 0;
        for (int j = 0; j < M; j++)
        {
            sum += arr[i][j];
        }
        printf("Sum of %dth row : %d\n", i, sum);
    }

    for (int i = 0; i < M; i++)
    {
        sum = 0;
        for (int j = 0; j < N; j++)
        {
            sum += arr[j][i];
        }
        printf("Sum of %dth column : %d\n", i + 1, sum);
    }

    return 0;
}