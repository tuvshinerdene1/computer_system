#include <stdio.h>
struct EngiinButarxai
{
    int d, n; // d - xurtver
    // n - xuviar
};
typedef struct EngiinButarxai EngiinButarxai;
int findGCD(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    return findGCD(b, a % b);
}

int findLCD(int a, int b)
{
    return (a / findGCD(a, b) * b);
}
void normalize(EngiinButarxai *a)
{
    int gcd = findGCD(a->n, a->d);
    a->n /= gcd;
    a->d /= gcd;
}
/*
Nemex uildel
*/
EngiinButarxai add(EngiinButarxai a, EngiinButarxai b)
{
    EngiinButarxai result;
    result.n = findLCD(a.n, b.n);
    result.d = a.d * (result.n / a.n) + b.d * (result.n / b.n);
    normalize(&result);
    return result;
}
/*
Xasax uildel
*/
EngiinButarxai sub(EngiinButarxai a, EngiinButarxai b)
{
    EngiinButarxai result;
    result.n = findLCD(a.n, b.n);
    result.d = a.d * (result.n / a.n) - b.d * (result.n / b.n);
    normalize(&result);
    return result;
}
/*
Urjix uildel
*/
EngiinButarxai mult(EngiinButarxai a, EngiinButarxai b)
{
    EngiinButarxai result;
    result.n = a.n * b.n;
    result.d = a.d * b.d;
    normalize(&result);
    return result;
}
/*
Xuvaax uildel
*/
EngiinButarxai div(EngiinButarxai a, EngiinButarxai b)
{
    EngiinButarxai result;
    result.n = a.n * b.d;
    result.d = a.d * b.n;
    normalize(&result);
    return result;
}
/*
Hewlex uildel
*/
void print(EngiinButarxai a)
{
    printf("%d/%d\n", a.d, a.n);
}
int main()
{
    EngiinButarxai a, b;
    printf("[?]/[?] = ");
    scanf("%d %d", &a.d, &a.n);
    printf("[?]/[?] = ");
    scanf("%d %d", &b.d, &b.n);

    int answer = 1;
    while (answer != 0)
    {
        printf("1.print a 2.print b 3.(a-b) 4.(a+b) 5.(a*b) 6.(a/b) 0.exit");
        scanf("%d", &answer);
        switch (answer)
        {
        case 1:
            print(a);
            break;
        case 2:
            print(b);
            break;
        case 3:
            printf("a-b = ");
            print(sub(a, b));
            break;
        case 4:
            printf("a+b = ");
            print(add(a, b));
            break;
        case 5:
            printf("a*b = ");
            print(mult(a, b));
            break;
        case 6:
            printf("a/b= ");
            print(div(a, b));
            break;
        case 0:
            return 0;
        default:
            printf("invalide answer\n");
        }
    }
    return 0;
}