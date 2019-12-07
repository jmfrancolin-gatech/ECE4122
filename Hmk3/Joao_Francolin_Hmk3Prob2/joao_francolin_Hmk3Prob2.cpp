/*
Author: Joao Matheus Francolin
Class: ECE 4122
Last date modified: October 1, 2019
Description: The implementation returns the maximum product of four consecutive
elements of an input matrix. Directions implicitly defined in "consecutive" are:
East - West / North - South / South-East - North-East / North-West - South-East
*/

// Implemantation only works in C++ 11
// clang++ -std=c++11 Problem2.cpp -o Problem2
// g++ Problem2.cpp -fopenmp -O3 -o Problem2

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <omp.h>

// #pragma omp declare reduction (merge : std::vector<int> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))

int main(int argc, char *argv[])
{

    // Open file
    char *file_name = argv[1];
    std::ifstream my_file (file_name);
    if (!my_file.is_open())
    {
        return -1;
    }

    // matrix properties
    int num_rows;
    int num_columns;
    std::vector<std::vector<int> > matrix;

    // Initialyze fields
    std::vector<int> product;

    my_file >> num_rows;
    my_file >> num_columns;
    matrix = std::vector< std::vector<int> > (num_rows, \
        std::vector<int>(num_columns, 0));

    // Populate matrix
    for (int ii = 0; ii < num_rows; ii++)
    {
        for (int jj = 0; jj < num_columns; jj++)
        {
            my_file >> matrix[ii][jj];
            // std::cout << std::setw(3) << matrix[ii][jj];
        }
        // std::cout << std::endl;
    }
    my_file.close();

    #pragma omp parallel
    {
        #pragma omp for
        // compute max product
        for (int ii = 0; ii < num_rows; ii++)
        {
            // #pragma omp reduction(merge: product)
            for (int jj = 0; jj < num_columns; jj++)
            {
                //North-East
                if (ii - 3 >= 0 and jj + 3 < num_columns)
                {
                    #pragma omp critical
                    product.push_back(matrix[ii][jj] * matrix[ii - 1][jj + 1] * \
                        matrix[ii - 2][jj + 2] * matrix[ii - 3][jj + 3]);
                }
                //East
                if (jj + 3 < num_columns)
                {
                    #pragma omp critical
                    product.push_back(matrix[ii][jj] * matrix[ii][jj + 1] * \
                        matrix[ii][jj + 2] * matrix[ii][jj + 3]);
                }
                //South-East
                if (ii + 3 < num_rows and jj + 3 < num_columns)
                {
                    #pragma omp critical
                    product.push_back(matrix[ii][jj] * matrix[ii + 1][jj + 1] * \
                        matrix[ii + 2][jj + 2] * matrix[ii + 3][jj + 3]);
                }
                //South
                if (ii + 3 < num_rows)
                {
                    #pragma omp critical
                    product.push_back(matrix[ii][jj] * matrix[ii + 1][jj] * \
                        matrix[ii + 2][jj] * matrix[ii + 3][jj]);
                }
            }
        }
    }

    int max_product = *std::max_element(product.begin(), product.end());
    std::cout << "Max element is " << max_product << std::endl;

    return 0;
}
