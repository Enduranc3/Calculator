#include <stdio.h>
#include <math.h>

int main() {
    long double num1, num2, result;
    char operator;

    printf("Enter an arithmetic expression: ");
    scanf("%lf %c %lf", &num1, &operator, &num2);

    switch(operator) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
        case ':':
            result = num1 / num2;
            break;
        case '%':
            result = fmod(num1, num2);
            break;
        case '^':
            result = pow(num1, num2);
            break;
        case 's':
            result = sqrt(num1);
            break;
        case 'e':
            result = exp(num1);
            break;
        case 'l':
            result = log(num1);
            break;
        case '!':
            result = 1;
            for(int i = 1; i <= num1; ++i) {
                result *= i;
            }
            break;
        default:
            printf("Invalid operator\n");
            return 1;
    }

    printf("Result: %.2lf\n", result);

    return 0;
}
