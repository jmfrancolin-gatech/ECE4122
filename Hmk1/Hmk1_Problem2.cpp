/*
Author: Joao Matheus Nascimento Francolin / jfrancolin3
Class: ECE4122
Last Date Modified: September 08, 2019
Description: Numerically computes the integral of the function [4 / (1 + x^2)].
Lower bound = 0, upper bound is given by user input (ß)
*/

#include <iostream>
#include <cmath>
using namespace std;

/**
 * Math fanction / helper
 * Input: x value
 * Output: correnponding y value for the defind function
 */
double f(double x)
{
    return 4.0 / (1.0 + pow(x, 2));
}

/**
 * Main Function
 * Executes the program
 */
int main()
{
    // Local variables
    double betta, dx, x = 0.0, y = 0.0;
    unsigned long n;

    // Get user inputs
    cout << "Please enter a value for the upper limit (beta): ";
    cin >> betta;
    cout << "Please enter the number of subdivisions to use: ";
    cin >> n;

    // Compute dx
    dx = betta / n;

    // Perform integration
    for(int i = 0; i < n; i++)
    {
        y = y + f(x) * dx;
        x = x + dx;
    }

    // Print final value
    cout << "The integral evaluates to: " << y << endl;

    return 0;
}
