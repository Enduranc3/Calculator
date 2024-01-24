# Calculator
This is a simple console app for calculating written in C.

## Functions
This app allows such arithmetic operations as:
- adding (+);
- subtracting (-);
- multiplying (*);
- dividing (/ or :);
- dividing remainder (%);
- raising to the power (^);

and a bunch of math functions, such as:
- square root;
- sine, cosine, tangent, cotangent (same for hyperbolic);
- arc sine, arc cosine, arc tangent, arc cotangent (same for hyperbolic);
- exponentiation;
- absolute value;
- round, ceil, floor, truncation;
- sign;
- rad, deg;
- factorial;
- log, lg, ln;
- min or max of numbers set.

## Building
To build the calculator, use your preferred C compiler to compile `calculator.c`.

## Usage
To use the calculator, just run `calculator.exe`, or execute from the command line with no arguments. 

Input your example and press Enter to calculate it. Input nothing and press Enter to exit the app.

The procedure is like in math. You can also use parentheses to clarify the order.

## Error Codes
The calculator uses the following error codes:

- `ERROR_FAILED_TO_ALLOCATE_MEMORY`(1): Failed to allocate memory;
- `ERROR_INVALID_FUNCTION`(2): Invalid function;
- `ERROR_UNDEFINED_FUNCTION`(3): Undefined function;
- `ERROR_INVALID_FUNCTION`(4): Function not found in a list;
- `ERROR_UNKNOWN`(5): For all other unexpected errors.