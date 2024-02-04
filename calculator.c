/**
 * \file            calculator.c
 * \brief           This file contains the implementation of a calculator program
 */

/*
 * Copyright (c) 2024 Daniil VERES
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author:          Daniil VERES <daniaveres@gmail.com>
 * Version:         v1.0.0
 */

                    /* Functions used: */
#include <ctype.h>  /* isalpha, isdigit */
#include <math.h>   /* sqrt, pow, sin, cos, tan, asin, acos, atan, sinh, cosh, tanh, asinh, acosh, atanh, fabs, ceil, floor, round, trunc, fmod, log, log10 */
#include <stdint.h> /* int8_t, uint8_t, int16_t, int32_t */
#include <stdio.h>  /* printf, fgets, stdin */
#include <stdlib.h> /* system, malloc, free, exit, strtod */
#include <string.h> /* strlen, strstr, strpbrk, _stricmp */

                                    /* Constants used: */
#define M_PI 3.14159265358979323846 /*!< Pi number */
#define MAX_ALLOCATED_BLOCKS 1000   /*!< Maximum number of allocated blocks */
#define MAX_INPUT_LENGTH 100        /*!< Maximum length of input string */
#define MAX_FUNCTION_COUNT 68       /*!< Maximum number of math functions */
#define MAX_FUNCTION_LENGTH 10      /*!< Maximum length of a math function */

/**
 * \brief           Enumeration representing error codes in the calculator program
 */
typedef enum {
    ERROR_FAILED_TO_ALLOCATE_MEMORY = 1,   /*!< Failed to allocate memory error code */
    ERROR_INVALID_INPUT,                   /*!< Invalid input error code */
    ERROR_UNDEFINED_FUNCTION,              /*!< Undefined function error code */
    ERROR_UNKNOWN                          /*!< Unknown error code */
} error_code_t;

static void** allocated_memory;         /*!< An array of allocated memory, used to keep track of dynamically allocated memory and free all at once */
static size_t allocated_memory_count;   /*!< A number of actually allocated blocks */
static char** math_functions;           /*!< An array of math functions, used to store math functions and their keywords */

static void error_handler(error_code_t error_code, const char* function, int32_t line);  /* A function used to handle errors based on the passed error code */

static void add_allocated_memory(void* ptr); /* A function used to add a pointer to the allocated memory array */
static void free_all(void);                  /* A function used to free all allocated memory */

static void init_math_functions(void);   /* A function used to allocate and initialize math functions */

                                                        /* A set of functions to validate input */
static uint8_t is_valid_input(const char* str);         /* A function used to check if the input is valid */
static uint8_t is_valid_parenthesis(const char* str);   /* A function used to check if parentheses are valid */
static uint8_t is_valid_point(const char* str);         /* A function used to check if decimal points are valid */
static uint8_t is_valid_space(const char* str);         /* A function used to check if spaces are valid */
static uint8_t has_random_letters(const char* str);     /* A function used to check if there are random letters in the input */

static void get_token(const char** str, char* token);    /* A function used to get a token from the input string */

                                                            /* A set of functions used to parse the input string */
static double factor(const char** str, char* token);        /* A function used to parse a factor (looking for a '(' to clarify the order and '-' to change sign) */
static double expression(const char** str, char* token);    /* A function used to parse and handle an addition or a subtraction */
static double term(const char** str, char* token);          /* A function used to parse and handle such terms as: '*', '/', ':', '%', '^' */

static double call_function(const char** str, char* token, const char* func); /* A function used to call an appropriate math function from the list below */

                                                        /* A set of math functions */
static double sqrt_s(const char** str, char* token);    /* A function used to calculate a square root */
static double sin_s(const char** str, char* token);     /* A function used to calculate a sine */
static double cos_s(const char** str, char* token);     /* A function used to calculate a cosine */
static double tan_s(const char** str, char* token);     /* A function used to calculate a tangent */
static double ctan_s(const char** str, char* token);    /* A function used to calculate a cotangent */
static double asin_s(const char** str, char* token);    /* A function used to calculate an arcsine */
static double acos_s(const char** str, char* token);    /* A function used to calculate an arccosine */
static double atan_s(const char** str, char* token);    /* A function used to calculate an arctangent */
static double actan_s(const char** str, char* token);   /* A function used to calculate an arccotangent */
static double sinh_s(const char** str, char* token);    /* A function used to calculate a hyperbolic sine */
static double cosh_s(const char** str, char* token);    /* A function used to calculate a hyperbolic cosine */
static double tanh_s(const char** str, char* token);    /* A function used to calculate a hyperbolic tangent */
static double ctanh_s(const char** str, char* token);   /* A function used to calculate a hyperbolic cotangent */
static double asinh_s(const char** str, char* token);   /* A function used to calculate a hyperbolic arcsine */
static double acosh_s(const char** str, char* token);   /* A function used to calculate a hyperbolic arccosine */
static double atanh_s(const char** str, char* token);   /* A function used to calculate a hyperbolic arctangent */
static double actanh_s(const char** str, char* token);  /* A function used to calculate a hyperbolic arccotangent */
static double exp_s(const char** str, char* token);     /* A function used to calculate an exponential function */
static double fabs_s(const char** str, char* token);    /* A function used to calculate an absolute value */
static double ceil_s(const char** str, char* token);    /* A function used to calculate a ceiling value */
static double floor_s(const char** str, char* token);   /* A function used to calculate a floor value */
static double round_s(const char** str, char* token);   /* A function used to calculate a rounded value */
static double trunc_s(const char** str, char* token);   /* A function used to calculate a truncated value */
static double sign_s(const char** str, char* token);    /* A function used to calculate a sign */
static double rad_s(const char** str, char* token);     /* A function used to convert degrees to radians */
static double deg_s(const char** str, char* token);     /* A function used to convert radians to degrees */
static double fact_s(const char** str, char* token);    /* A function used to calculate a factorial */
static double log_s(const char** str, char* token);     /* A function used to calculate a logarithm */
static double log10_s(const char** str, char* token);   /* A function used to calculate a decimal logarithm */
static double ln_s(const char** str, char* token);      /* A function used to calculate a natural logarithm */
static double min_s(const char** str, char* token);     /* A function used to calculate a minimum value */
static double max_s(const char** str, char* token);     /* A function used to calculate a maximum value */

/**
 * \brief           Main function
 * \note            The program is a simple calculator that can handle arithmetic expressions with the following operations:
 *                  addition, subtraction, multiplication, division, modulo, power, square root, natural logarithm, exponential function,
 *                  sine, cosine, tangent, cotangent, arcsine, arccosine, arctangent, arccotangent, hyperbolic sine, hyperbolic cosine, hyperbolic tangent,
 *                  hyperbolic cotangent, hyperbolic arcsine, hyperbolic arccosine, hyperbolic arctangent, hyperbolic arccotangent, absolute value, ceiling value,
 *                  floor value, rounded value, truncated value, sign, degrees to radians conversion, radians to degrees conversion, factorial, logarithm, decimal logarithm, minimum value, maximum value
 * \return          0 in case of successful finish
 */
int
main(void) {
    allocated_memory = (void**)malloc(MAX_ALLOCATED_BLOCKS * sizeof(void*));    /* Allocate memory for the allocated memory array */
    if (allocated_memory == NULL) {                                             /* Check if the memory has been allocated */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__);     /* Handle the error if the memory has not been allocated */
    }
    char* input = (char*)malloc(MAX_INPUT_LENGTH * sizeof(char));               /* Allocate memory for the input string */
    if (input == NULL) {                                                        /* Check if the memory has been allocated */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__);     /* Handle the error if the memory has not been allocated */
    }
    add_allocated_memory(input);                                                /* Add the allocated memory to the allocated memory array */
    printf("Enter an arithmetic expression: ");                                 /* Ask the user to enter an arithmetic expression */
    fgets(input, MAX_INPUT_LENGTH, stdin);                                      /* Get the input string */
    init_math_functions();                                                      /* Initialize math functions */
    if (!is_valid_input(input)) {                                               /* Check if the input is valid */
        error_handler(ERROR_INVALID_INPUT, __func__, __LINE__);                 /* Handle the error if the input is not valid */
    }
                                                                                /* Loop for multiple execution */
    while (*input != '\n') {                                                    /* While the input is not a new line (input nothing and press Enter) */
        const char* str = input;                                                /* Create a pointer to the input string */
        char token;                                                             /* Create a variable to store a token */
        get_token(&str, &token);                                                /* Get a token from the input string */
        double result;                                                          /* Create a variable to store the result */
        result = expression(&str, &token);                                      /* Calculate the result */
        if (result == floor(result)) {                                          /* Check if there is no fractional part */
            printf("Result: %.0lf\n", result);                                  /* Print the result without the fractional part */
        } else {                                                                /* Else if there is a fractional part */
            printf("Result: %.10lf\n", result);                                 /* Print the result with the fractional part */
        }
        printf("Enter an arithmetic expression: ");                             /* Ask the user to enter an arithmetic expression */
        fgets(input, MAX_INPUT_LENGTH, stdin);                                  /* Get the input string */
        if (!is_valid_input(input)) {                                           /* Check if the input is valid */
            error_handler(ERROR_INVALID_INPUT, __func__, __LINE__);             /* Handle the error if the input is not valid */
        }
    }
    free_all();         /* Free all allocated memory */
    system("pause");    /* Pause the program */
    return 0;           /* Return 0 as a sign of successful finish */
}

/**
 * \brief           A function used to handle errors based on the passed error code
 * \param[in]       error_code: An error code to handle
 * \param[in]       function: A function name where the error occurred
 * \param[in]       line: A line number where the error occurred
 * \note            The function freeing allocated memory, prints an error message based on the passed error code and exits the program with the appropriate error code
 */
static void
error_handler(const error_code_t error_code, const char* function, const int32_t line) {
    free_all();                                         /* Free all allocated memory */
    printf("Function %s, line %d, ", function, line);   /* Print the function name and the line number where the error occurred */
    switch (error_code) {                               /* Print an error message based on the passed error code */
        case ERROR_FAILED_TO_ALLOCATE_MEMORY:
            printf("\033[31merror: failed to allocate memory\033[0m\n");
            break;
        case ERROR_INVALID_INPUT:
            printf("\033[31merror: invalid input\033[0m\n");
            break;
        case ERROR_UNDEFINED_FUNCTION:
            printf("\033[31merror: undefined math function\033[0m\n");
            break;
        case ERROR_UNKNOWN:
            printf("\033[31merror: unknown error\033[0m\n");
            break;
        default:
            break;
    }
    system("pause");                                    /* Pause the program */
    exit(error_code);                                   /* Exit the program with appropriate error code */
}

/**
 * \brief           A function used to add a pointer to the allocated memory array
 * \param[in]       ptr: A pointer to add
 */
static void
add_allocated_memory(void* ptr) {
    if (allocated_memory_count + 1 >= MAX_ALLOCATED_BLOCKS) {               /* Check if the allocated memory array is full */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the allocated memory array is full */
    } else {                                                                /* Else if the allocated memory array is not full */
        allocated_memory[allocated_memory_count] = ptr;                     /* Add the pointer to the allocated memory array */
        ++allocated_memory_count;                                           /* Increment the number of actually allocated blocks */
    }
}

/**
 * \brief           A function used to free all allocated memory
 */
static void
free_all(void) {
    for (; allocated_memory_count > 0; --allocated_memory_count) {  /* Loop through all allocated memory */
        free(allocated_memory[allocated_memory_count - 1]);         /* Free the allocated memory */
        allocated_memory[allocated_memory_count - 1] = NULL;
    }
    free(allocated_memory);                                         /* Free the allocated memory array */
    allocated_memory = NULL;
}

/**
 * \brief           A function used to allocate and initialize math functions
 * \note            The function allocates memory for the math functions array and initializes it, some of the functions are repeated with different names, e.g. tg and tan
 */
static void
init_math_functions(void) {
    math_functions = (char**)malloc(MAX_FUNCTION_COUNT * sizeof(char*));        /* Allocate memory for the math functions array */
    if (math_functions == NULL) {                                               /* Check if the memory has been allocated */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__);     /* Handle the error if the memory has not been allocated */
    }
    add_allocated_memory(math_functions);                                       /* Add the allocated memory to the allocated memory array */
    for (size_t i = 0; i < MAX_FUNCTION_COUNT; ++i) {                           /* Loop through all math functions */
        math_functions[i] = (char*)malloc(MAX_FUNCTION_LENGTH * sizeof(char));  /* Allocate memory for a math function */
        if (math_functions[i] == NULL) {                                        /* Check if the memory has been allocated */
            error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the memory has not been allocated */
        }
        add_allocated_memory(math_functions[i]);                                /* Add the allocated memory to the allocated memory array */
    }
    math_functions[0] = "sqrt";
    math_functions[1] = "ln";
    math_functions[2] = "exp";
    math_functions[3] = "sin";
    math_functions[4] = "cos";
    math_functions[5] = "tan";
    math_functions[6] = "tg";
    math_functions[7] = "ctan";
    math_functions[8] = "ctg";
    math_functions[9] = "cotan";
    math_functions[10] = "cot";
    math_functions[11] = "cotg";
    math_functions[12] = "arcsin";
    math_functions[13] = "asin";
    math_functions[14] = "arccos";
    math_functions[15] = "acos";
    math_functions[16] = "arctan";
    math_functions[17] = "arctg";
    math_functions[18] = "atan";
    math_functions[19] = "atg";
    math_functions[20] = "arcctan";
    math_functions[21] = "arcctg";
    math_functions[22] = "arccotan";
    math_functions[23] = "arccot";
    math_functions[24] = "arccotg";
    math_functions[25] = "acotan";
    math_functions[26] = "acot";
    math_functions[27] = "acotg";
    math_functions[28] = "sinh";
    math_functions[29] = "sh";
    math_functions[30] = "cosh";
    math_functions[31] = "ch";
    math_functions[32] = "tanh";
    math_functions[33] = "tgh";
    math_functions[34] = "th";
    math_functions[35] = "ctanh";
    math_functions[36] = "ctgh";
    math_functions[37] = "coth";
    math_functions[38] = "cth";
    math_functions[39] = "arcsinh";
    math_functions[40] = "arsinh";
    math_functions[41] = "asinh";
    math_functions[42] = "arcsh";
    math_functions[43] = "arccosh";
    math_functions[44] = "arcosh";
    math_functions[45] = "acosh";
    math_functions[46] = "arcch";
    math_functions[47] = "arctanh";
    math_functions[48] = "arctgh";
    math_functions[49] = "arcth";
    math_functions[50] = "artgh";
    math_functions[51] = "atanh";
    math_functions[52] = "arccoth";
    math_functions[53] = "arccth";
    math_functions[54] = "arcoth";
    math_functions[55] = "abs";
    math_functions[56] = "ceil";
    math_functions[57] = "floor";
    math_functions[58] = "round";
    math_functions[59] = "trunc";
    math_functions[60] = "sign";
    math_functions[61] = "rad";
    math_functions[62] = "deg";
    math_functions[63] = "fact";
    math_functions[64] = "log";
    math_functions[65] = "lg";
    math_functions[66] = "min";
    math_functions[67] = "max";
}

/**
 * \brief           A function used to check if the input is valid
 * \param[in]       str: A string to check
 * \return          1 if the input is valid, 0 otherwise
 */
static uint8_t
is_valid_input(const char* str) {
    size_t length;          /* A variable to store the length of the input string */
    length = strlen(str);   /* Get the length of the input string */
    if (str[0] == '\n') {   /* Check if the input is empty */
        return 1;           /* If so, it can be considered as valid */
    } else if (length == MAX_INPUT_LENGTH - 1 && str[MAX_INPUT_LENGTH - 2] != '\n') {   /* Check if the input is too long */
        return 0;                                                                       /* If so, it is invalid */
    } else if (str[0] == ' ') { /* Check if the input starts with a space */
        return 0;               /* If so, it is invalid */
    } else if (str[0] == '.' || str[0] == '*' || str[0] == '/' || str[0] == ':' || str[0] == '%' || str[0] == '^' || str[0] == ')'
            || str[length - 2] == '.' || str[length - 2] == '+' || str[length - 2] == '*' || str[length - 2] == '/'
            || str[length - 2] == ':' || str[length - 2] == '%' || str[length - 2] == '^' || str[length - 2] == '(') {  /* Check if the input starts or ends with an operator */
        return 0;                                                                                                       /* If so, it is invalid */
    } else if (strpbrk(str, "!\"#$%&'`~\\|<>?_@;=[]{}\t\v\f\r") != NULL) {  /* Check if the input contains invalid characters */
        return 0;                                                           /* If so, it is invalid */
    } else if (strpbrk(str, "0123456789") != NULL && strpbrk(str, "+-*/:%^") == NULL && strpbrk(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == NULL) { /* Check if the input contains only numbers */
        return 0;                                                                                                                                                       /* If so, it is invalid */
    } else if (strpbrk(str, "+-*/:%^") != NULL && strpbrk(str, "0123456789") == NULL) { /* Check if the input contains only operators */
        return 0;                                                                       /* If so, it is invalid */
    } else if (strpbrk(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != NULL && strpbrk(str, "0123456789") == NULL) {    /* Check if the input contains only letters */
        return 0;                                                                                                                       /* If so, it is invalid */
    } else if (strpbrk(str, "()") != NULL && strpbrk(str, "0123456789") == NULL && strpbrk(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == NULL) {  /* Check if the input contains only parentheses */
        return 0;                                                                                                                                                   /* If so, it is invalid */
    } else if (strstr(str, "..") != NULL || strstr(str, ".+") != NULL || strstr(str, ".-") != NULL || strstr(str, ".*") != NULL || strstr(str, "./") != NULL
            || strstr(str, ".:") != NULL || strstr(str, ".%") != NULL || strstr(str, ".^") != NULL || strstr(str, ".(") != NULL || strstr(str, ".)") != NULL
            || strstr(str, ".,") != NULL || strstr(str, ". ") != NULL || strstr(str, "+.") != NULL || strstr(str, "+*") != NULL || strstr(str, "+/") != NULL
            || strstr(str, "+:") != NULL || strstr(str, "+%") != NULL || strstr(str, "+^") != NULL || strstr(str, "+)") != NULL || strstr(str, "+,") != NULL
            || strstr(str, "-.") != NULL || strstr(str, "-*") != NULL || strstr(str, "-/") != NULL || strstr(str, "-:") != NULL || strstr(str, "-%") != NULL
            || strstr(str, "-^") != NULL || strstr(str, "-)") != NULL || strstr(str, "-,") != NULL || strstr(str, "*.") != NULL || strstr(str, "*+") != NULL
            || strstr(str, "*/") != NULL || strstr(str, "*:") != NULL || strstr(str, "*%") != NULL || strstr(str, "*^") != NULL || strstr(str, "*)") != NULL
            || strstr(str, "*,") != NULL || strstr(str, "/.") != NULL || strstr(str, "/+") != NULL || strstr(str, "/*") != NULL || strstr(str, "/:") != NULL
            || strstr(str, "/%") != NULL || strstr(str, "/^") != NULL || strstr(str, "/)") != NULL || strstr(str, "/,") != NULL || strstr(str, ":.") != NULL
            || strstr(str, ":+") != NULL || strstr(str, ":*") != NULL || strstr(str, ":/") != NULL || strstr(str, ":%") != NULL || strstr(str, ":^") != NULL
            || strstr(str, ":)") != NULL || strstr(str, ":,") != NULL || strstr(str, "%.") != NULL || strstr(str, "%+") != NULL || strstr(str, "%-") != NULL
            || strstr(str, "%*") != NULL || strstr(str, "%/") != NULL || strstr(str, "%:") != NULL || strstr(str, "%^") != NULL || strstr(str, "%)") != NULL
            || strstr(str, "%,") != NULL || strstr(str, "^.") != NULL || strstr(str, "^+") != NULL || strstr(str, "^-") != NULL || strstr(str, "^*") != NULL
            || strstr(str, "^/") != NULL || strstr(str, "^:") != NULL || strstr(str, "^%") != NULL || strstr(str, "^)") != NULL || strstr(str, "^,") != NULL
            || strstr(str, "(.") != NULL || strstr(str, "(+") != NULL || strstr(str, "(*") != NULL || strstr(str, "(/") != NULL || strstr(str, "(:") != NULL
            || strstr(str, "(%") != NULL || strstr(str, "(^") != NULL || strstr(str, "(,") != NULL || strstr(str, "()") != NULL || strstr(str, "  ") != NULL) { /* Check if the input contains invalid combinations of characters */
        return 0;                                                                                                                                               /* If so, it is invalid */
    } else if (!is_valid_parenthesis(str)) {    /* Check if parentheses are valid */
        return 0;                               /* If not, the input is invalid */
    } else if (!is_valid_point(str)) {          /* Check if decimal points are valid */
        return 0;                               /* If not, the input is invalid */
    } else if (!is_valid_space(str)) {          /* Check if spaces are valid */
        return 0;                               /* If not, the input is invalid */
    } else if (has_random_letters(str)) {       /* Check if there are random letters in the input */
        return 0;                               /* If so, the input is invalid */
    } else {                                    /* Else if the input is valid */
        return 1;                               /* Return 1 as sign of valid input */
    }
}

/**
 * \brief           A function used to check if parentheses are valid
 * \param[in]       str: A string to check
 * \return          1 if parentheses are valid, 0 otherwise
 */
static uint8_t
is_valid_parenthesis(const char* str) {
    size_t length, top = -1;    /* A variable to store the length of the input string and a variable to store the top of the stack */
    length = strlen(str);       /* Get the length of the input string */
    char* stack = (char*)malloc(length * sizeof(char));                     /* Allocate memory for a stack */
    if (stack == NULL) {                                                    /* Check if the memory has been allocated */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the memory has not been allocated */
    }
    for (size_t i = 0; i < length; ++i) {   /* Loop through all characters in the input string */
        if (str[i] == '(') {                /* If the character is a left parenthesis */
            if (isdigit(str[i - 1]) && isdigit(str[i + 1])) {   /* Check if the previous and the next characters are digits */
                free(stack);
                stack = NULL;
                return 0;                                       /* If so, parentheses are invalid */
            }
            stack[++top] = str[i];          /* Push the character to the stack */
        } else if (str[i] == ')') {         /* Else if the character is a right parenthesis */
            if (top == -1) {                /* Check if the stack is empty */
                free(stack);
                stack = NULL;
                return 0;                   /* If so, parentheses are invalid */
            }
            char last = stack[top];         /* Get the last character from the stack */
            --top;                          /* Decrement the top of the stack */
            if (last != '(') {              /* Check if the last character from the stack is not a left parenthesis */
                free(stack);
                stack = NULL;
                return 0;                   /* If so, parentheses are invalid */
            }
        }
    }
    if (top != -1) {    /* Check if the stack is not empty */
        free(stack);
        stack = NULL;
        return 0;       /* If so, parentheses are invalid */
    } else {            /* Else if the stack is empty */
        free(stack);
        stack = NULL;
        return 1;       /* Then parentheses are valid or absent */
    }
}

/**
 * \brief           A function used to check if decimal points are valid
 * \param[in]       str: A string to check
 * \return          1 if decimal points are valid, 0 otherwise
 */
static uint8_t
is_valid_point(const char* str) {
    size_t length;              /* A variable to store the length of the input string */
    uint8_t active_point = 0;   /* A variable to store if a decimal point is active */
    length = strlen(str);       /* Get the length of the input string */
    for (size_t i = 1; i < length; ++i) {                       /* Loop through all characters in the input string */
        if (str[i] == '.' && active_point) {                    /* If the character is a decimal point and a decimal point is active */
            return 0;                                           /* Then it is not valid */
        } else if (str[i] == '.' && !active_point) {            /* Else if the character is a decimal point and a decimal point is not active */
            if (!isdigit(str[i - 1]) || !isdigit(str[i + 1])) { /* Check if the previous and the next characters are not digits */
                return 0;                                       /* Then it is not valid */
            } else {                                            /* Else if the previous and the next characters are digits */
                active_point = 1;                               /* Set the decimal point active */
            }
        } else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == ':' || str[i] == '%' || str[i] == '^') {   /* Else if the character is an operator */
            active_point = 0;                                                                                                               /* Set the decimal point inactive */
        }
    }
    return 1; /* If no errors were found during the check before that, then all the points are correct or absent */
}

/**
 * \brief           A function used to check if spaces are valid
 * \param[in]       str: A string to check
 * \return          1 if spaces are valid, 0 otherwise
 */
static uint8_t
is_valid_space(const char* str) {
    size_t length;          /* A variable to store the length of the input string */
    length = strlen(str);   /* Get the length of the input string */
    for (size_t i = 1; i < length; ++i) {                                   /* Loop through all characters in the input string */
        if (str[i] == ' ' && isdigit(str[i - 1]) && isdigit(str[i + 1])) {  /* If the character is a space and the previous and the next characters are digits */
            return 0;                                                       /* Then it is not valid */
        }
    }
    return 1;                                                               /* Otherwise it is valid */
}

/**
 * \brief           A function used to check if there are random letters in the input
 * \param[in]       str: A string to check
 * \return          1 if there are random letters in the input, 0 otherwise
 */
static uint8_t
has_random_letters(const char* str) {
    size_t length, sub_string_length = -1;            /* A variable to store the length of the input string and a variable to store the length of a substring */
    uint8_t is_valid_function, has_been_compared = 0; /* A variable to store if a function is valid and a variable to store if a function has been compared */
    char* sub_string = (char*)malloc(MAX_INPUT_LENGTH * sizeof(char));      /* Allocate memory for a substring */
    if (sub_string == NULL) {                                               /* Check if the memory has been allocated */
        error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the memory has not been allocated */
    }
    sub_string[0] = '\0';
    length = strlen(str);  /* Get the length of the input string */
    for (size_t i = 0; i < length; ++i) {                           /* Loop through all characters in the input string */
        if (isalpha(str[i])) {                                      /* If the character is a letter */
            sub_string[++sub_string_length] = str[i];               /* Add the character to the substring */
            has_been_compared = 0;                                  /* Set the variable to store if a function has been compared to 0 */
        } else if (!has_been_compared && *sub_string != '\0') {     /* Else if the character is not a letter and a function has not been compared and the substring is not empty */
            sub_string[++sub_string_length] = '\0';                 /* Add the null terminator to the end of substring */
            is_valid_function = 0;                                  /* Set the variable to store if a function is valid to 0 */
            for (size_t j = 0; j < MAX_FUNCTION_COUNT; ++j) {       /* Loop through all math functions to find match */
                if (_stricmp(sub_string, math_functions[j]) == 0) { /* If the substring matches a math function */
                    is_valid_function = 1;                          /* Set the variable to store if a function is valid to 1 */
                    break;                                          /* Break the loop */
                }
            }
            if (!is_valid_function || str[i] != '(') {              /* If the function is not valid or the next character is not a left parenthesis */
                free(sub_string);
                sub_string = NULL;
                return 1;                                           /* Then there are random letters in the input */
            } else {                                                /* Else if the function is valid and the next character is a left parenthesis */
                has_been_compared = 1;                              /* Set the variable to store if a function has been compared to 1 */
                sub_string_length = -1;                             /* Set the length of the substring to -1 */
            }
        }
    }
    free(sub_string);
    sub_string = NULL;
    return 0;  /* If no errors were found during the check before that, then there are no random letters in the input */
}


/**
 * \brief           Retrieves the next token from a string
 * \param           str: The input string pointer
 * \param           token: The variable to store the retrieved token
 * \note            The function skips all spaces and retrieves the next token from the input string
 */
static void
get_token(const char** str, char* token) {
    while (**str == ' ') {  /* While a current pointer is space */
        ++(*str);           /* Increment the pointer */
    }
    *token = **str;         /* Get the current character */
    ++(*str);               /* Increment the pointer */
}

/**
 * \brief           A function used to break the input string into tokens and calculate the result
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
factor(const char** str, char* token) {
    int8_t sign = 1;            /* A variable to store a sign */
    double result = 0;          /* A variable to store the result */
    if (*token == '-') {        /* Check if the token is a minus sign */
        sign = -1;              /* Set the sign to -1 if so */
        get_token(str, token);  /* Get the next token */
    }
    if (*token == '(') {                    /* Check if the token is a left parenthesis */
        get_token(str, token);              /* Get the next token */
        result = expression(str, token);    /* Calculate the result */
        get_token(str, token);              /* Get the next token */
    } else if (isdigit(*token) || *token == '.') {                              /* Else if the token is a digit or a decimal point */
        int8_t i = -1, has_decimal_point = 0;                                   /* A variable to store the length of a number and a variable to store if a number has a decimal point */
        char* number = (char*)malloc(MAX_INPUT_LENGTH * sizeof(char));          /* Allocate memory for a number */
        if (number == NULL) {                                                   /* Check if the memory has been allocated */
            error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the memory has not been allocated */
        }
        while (isdigit(*token) || (!has_decimal_point && *token == '.')) {      /* While the token is a digit or a decimal point */
            if (*token == '.') {                                                /* Check if the token is a decimal point */
                has_decimal_point = 1;                                          /* Set the variable to store if a number has a decimal point to 1 */
            }
            number[++i] = *token;                                               /* Add the token to the number */
            get_token(str, token);                                              /* Get the next token */
        }
        number[++i] = '\0';                                                     /* Add the null terminator to the end of the number */
        result = strtod(number, NULL);                                          /* Convert the number to a double */
        free(number);                                                           /* Free the allocated memory */
        number = NULL;
    } else if (isalpha(*token)) {                                               /* Else if the token is a letter */
        int8_t i = -1;                                                          /* A variable to store the length of a function */
        char* func = (char*)malloc(MAX_INPUT_LENGTH * sizeof(char));            /* Allocate memory for a function */
        if (func == NULL) {                                                     /* Check if the memory has been allocated */
            error_handler(ERROR_FAILED_TO_ALLOCATE_MEMORY, __func__, __LINE__); /* Handle the error if the memory has not been allocated */
        }
        add_allocated_memory(func);                                             /* Add the allocated memory to the allocated memory array (in case of a crash in call_function or the appropriate math function) */
        while (isalpha(*token)) {                                               /* While the token is a letter */
            func[++i] = *token;                                                 /* Add the token to the function */
            get_token(str, token);                                              /* Get the next token */
        }
        func[++i] = '\0';                                                       /* Add the null terminator to the end of the function */
        get_token(str, token);                                                  /* Get the next token */
        result = call_function(str, token, func);                               /* Calculate the result */
        free(func);                                                             /* Free the allocated memory */
        func = NULL;
        --allocated_memory_count;                                               /* Decrement the number of allocated blocks, because it was incremented in add_allocated_memory */
        get_token(str, token);                                                  /* Get the next token */
    }
    return sign * result;  /* Return the result with the appropriate sign */
}

/**
 * \brief           A function used to calculate an expression
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
expression(const char** str, char* token) {
    double result;              /* A variable to store the result */
    result = term(str, token);  /* Calculate the result */
    while (*token == '+' || *token == '-') {    /* While the token is a plus or a minus sign */
        char operator = *token;                 /* A variable to store an operator */
        double right;                           /* A variable to store the right part of the expression */
        get_token(str, token);                  /* Get the next token */
        right = term(str, token);               /* Calculate the right part of the expression */
        switch (operator) {                     /* Calculate the result based on the operator */
            case '+':
                result += right;
                break;
            case '-':
                result -= right;
                break;
            default:
                break;
        }
    }
    return result;
}

/**
 * \brief           A function used to calculate a term
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
term(const char** str, char* token) {
    double result;                  /* A variable to store the result */
    result = factor(str, token);    /* Calculate the result */
    while (*token == '*' || *token == '/' || *token == ':' || *token == '%' || *token == '^') { /* While the token is a multiplication, a division, a modulo, a power */
        char operator = *token;     /* A variable to store an operator */
        double right;               /* A variable to store the right part of the expression */
        get_token(str, token);      /* Get the next token */
        right = factor(str, token); /* Calculate the right part of the expression */
        switch (operator) {         /* Calculate the result based on the operator */
            case '*':
                result *= right;
                break;
            case '/':
            case ':':
                result /= right;
                break;
            case '%':
                result = fmod(result, right);
                break;
            case '^':
                result = pow(result, right);
                break;
            default:
                break;
        }
    }
    return result;
}

/**
 * \brief           A function used to call a math function
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \param[in]       func: A variable to store a function
 * \return          The result of the calculation
 */
static double
call_function(const char** str, char* token, const char* func) {
    size_t i; /* A variable to store the index of a math function */
    for (i = 0; i < MAX_FUNCTION_COUNT; ++i) {          /* Loop through all math functions */
        if (_stricmp(func, math_functions[i]) == 0) {   /* If the function matches a math function */
            break;                                      /* Break the loop */
        }
    }
    switch (i) {                                        /* Calculate the result based on the index of the math function */
        case 0:
            return sqrt_s(str, token);
        case 1:
            return ln_s(str, token);
        case 2:
            return exp_s(str, token);
        case 3:
            return sin_s(str, token);
        case 4:
            return cos_s(str, token);
        case 5:
            return tan_s(str, token);
        case 6:
            return ctan_s(str, token);
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return asin_s(str, token);
        case 12:
        case 13:
        case 14:
        case 15:
            return acos_s(str, token);
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            return atan_s(str, token);
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            return actan_s(str, token);
        case 28:
        case 29:
            return sinh_s(str, token);
        case 30:
        case 31:
            return cosh_s(str, token);
        case 32:
        case 33:
        case 34:
            return tanh_s(str, token);
        case 35:
        case 36:
        case 37:
        case 38:
            return ctanh_s(str, token);
        case 39:
        case 40:
        case 41:
            return asinh_s(str, token);
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
            return acosh_s(str, token);
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
            return atanh_s(str, token);
        case 52:
        case 53:
        case 54:
            return actanh_s(str, token);
        case 55:
            return fabs_s(str, token);
        case 56:
            return ceil_s(str, token);
        case 57:
            return floor_s(str, token);
        case 58:
            return round_s(str, token);
        case 59:
            return trunc_s(str, token);
        case 60:
            return sign_s(str, token);
        case 61:
            return rad_s(str, token);
        case 62:
            return deg_s(str, token);
        case 63:
            return fact_s(str, token);
        case 64:
            return log_s(str, token);
        case 65:
            return log10_s(str, token);
        case 66:
            return min_s(str, token);
        case 67:
            return max_s(str, token);
        default:
            error_handler(ERROR_UNKNOWN, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the square root of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below 0 is considered invalid
 */
static double
sqrt_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result >= 0) {
        return sqrt(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the sine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
sin_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return sin(result);
}

/**
 * \brief           A function used to calculate the cosine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
cos_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return cos(result);
}

/**
 * \brief           A function used to calculate the tangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            The function is calculated as sin(x) / cos(x), so if cos(x) is 0, the function is undefined
 */
static double
tan_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (cos(result) != 0) {
        return tan(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the cotangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            The function is calculated as 1 / tan(x), so if tan(x) is 0, the function is undefined
 */
static double
ctan_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (sin(result)) {
        return 1 / tan(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the arc sine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below -1 or above 1 is considered as invalid
 */
static double
asin_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result >= -1 && result <= 1) {
        return asin(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the arc cosine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below -1 or above 1 is considered as invalid
 */
static double
acos_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result >= -1 && result <= 1) {
        return acos(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the arc tangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
atan_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return atan(result);
}

/**
 * \brief           A function used to calculate the arc cotangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
actan_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return M_PI / 2 - atan(result);
}

/**
 * \brief           A function used to calculate the hyperbolic sine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
sinh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return sinh(result);
}

/**
 * \brief           A function used to calculate the hyperbolic cosine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
cosh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return cosh(result);
}

/**
 * \brief           A function used to calculate the hyperbolic tangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
tanh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return tanh(result);
}

/**
 * \brief           A function used to calculate the hyperbolic cotangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            The function is calculated as 1 / tanh(x), so if tanh(x) is 0, the function is undefined
 */
static double
ctanh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (tanh(result) != 0) {
        return 1 / tanh(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the hyperbolic arc sine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
asinh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return asinh(result);
}

/**
 * \brief           A function used to calculate the hyperbolic arc cosine of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below 1 is considered as invalid
 */
static double
acosh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result >= 1) {
        return acosh(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the hyperbolic arc tangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below -1 or above 1 is considered as invalid
 */
static double
atanh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result > -1 && result < 1) {
        return atanh(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the exponential function of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 */
static double
exp_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return exp(result);
}

/**
 * \brief           A function used to calculate the hyperbolic arc cotangent of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below -1 or above 1 is considered as invalid
 */
static double
actanh_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result > -1 && result < 1) {
        return log((1 + result) / (1 - result)) / 2;
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the absolute value of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The absolute value of a number
*/
static double
fabs_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return fabs(result);
}

/**
 * \brief           A function used to calculate the ceiling of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The ceiling of a number
 */
static double
ceil_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return ceil(result);
}

/**
 * \brief           A function used to calculate the floor of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The floor of a number
 */
static double
floor_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return floor(result);
}

/**
 * \brief           A function used to calculate the round of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The round of a number
 */
static double
round_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return round(result);
}

/**
 * \brief           A function used to calculate the truncation of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The truncation of a number
 */
static double
trunc_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return trunc(result);
}

/**
 * \brief           A function used to calculate the sign of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          1 if the number is positive, -1 if the number is negative, 0 otherwise
 */
static double
sign_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result > 0) {
        return 1;
    } else if (result < 0) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * \brief           A function used to calculate the radian of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The radian of a number
 */
static double
rad_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return result * M_PI / 180;
}

/**
 * \brief           A function used to calculate the degree of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The degree of a number
 */
static double
deg_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    return result * 180 / M_PI;
}

/**
 * \brief           A function used to calculate the factorial of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The factorial of a number
 * \note            Any number with an essential fractional part is considered invalid
 */
static double
fact_s(const char** str, char* token) {
    double result = 1, fact;
    fact = expression(str, token);
    if (fact >= 0 && fact == floor(fact)) {
        for (int16_t i = 1; i <= fact; ++i) {
            result *= i;
        }
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return result;
}

/**
 * \brief           A function used to calculate the logarithm of a number with a base
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The logarithm of a number with a base
 * \note            More than one comma will lead to an error
 */
static double
log_s(const char** str, char* token) {
    size_t comma_count = 0;
    double result = 0, base, value;
    for (size_t i = 0; i < strlen(*str); ++i) {
        if ((*str)[i] == ',') {
            ++comma_count;
        }
    }
    if (comma_count > 1) {
        error_handler(ERROR_INVALID_INPUT, __func__, __LINE__);
    }
    base = expression(str, token);
    get_token(str, token);
    value = expression(str, token);
    if (base > 0 && base != 1 && value > 0) {
        result = log(value) / log(base);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return result;
}

/**
 * \brief           A function used to calculate the logarithm of a number with a base 10
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The logarithm of a number with a base 10
 * \note            Any number below 0 is considered invalid
 */
static double
log10_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result > 0) {
        return log10(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the natural logarithm of a number
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The result of the calculation
 * \note            Any number below 0 is considered invalid
 */
static double
ln_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    if (result > 0) {
        return log(result);
    } else {
        error_handler(ERROR_UNDEFINED_FUNCTION, __func__, __LINE__);
    }
    return nan("");
}

/**
 * \brief           A function used to calculate the minimum of a set of numbers
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The minimum of a set of numbers
 */
static double
min_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    while (*token == ',') {
        get_token(str, token);
        double next = expression(str, token);
        if (next < result) {
            result = next;
        }
    }
    return result;
}

/**
 * \brief           A function used to calculate the maximum of a set of numbers
 * \param[in]       str: A string to calculate
 * \param[in]       token: A variable to store a token
 * \return          The maximum of a set of numbers
 */
static double
max_s(const char** str, char* token) {
    double result;
    result = expression(str, token);
    while (*token == ',') {
        get_token(str, token);
        double next = expression(str, token);
        if (next > result) {
            result = next;
        }
    }
    return result;
}
