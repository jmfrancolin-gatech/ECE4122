/*
Author: Joao Matheus Nascimento Francolin / jfrancolin3
Class: ECE4122
Last Date Modified: September 08, 2019
Description: Computes the number of 1's that must be added to the positive
integer n before the process of repeted divisions by 7 until n reaches 1
*/

#include <iostream>
using namespace std;

/**
 * Recursive function
 * Input: current sequence number
 * Output: number of times 1 was added to to sequence
 */
int process_counter(int n)
{
    // Base case
    if (n == 1)
    {
        return 0;
    }

    // Recursive call
    if (n % 7 == 0)
    {
        return process_counter(n / 7);
    }
    else
    {
        return 1 + process_counter(n + 1);
    }
}

/**
 * Main function
 *
 * Performs I/O and calls recursive function
 */
int main()
{
    // Local variables
    int n, counter;

    // Get user inputs
    cout << "Please enter the starting number n: ";
    cin >> n;

    // Compute process recursively
    counter = process_counter(n);

    // Output result
    cout << "The sequence had " << counter << \
    " instances of the number 1 being added" << endl;

    return 0;
}
