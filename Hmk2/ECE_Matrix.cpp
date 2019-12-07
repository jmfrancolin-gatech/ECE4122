/*
Author: Joao Matheus Francolin
Class: ECE 4122
Last date modified: September 25, 2019
Description: Implements ECE_Matrix class
*/

// Implemantation only works in C++ 11
// In order to compile in a Mac, run the following:
// clang++ -std=c++11 Hmk2_Test1.cpp ECE_Matrix.cpp -o Hmk2_Test1

#include "ECE_Matrix.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>

// Constructor chaining
ECE_Matrix::ECE_Matrix() : ECE_Matrix(0) {}
ECE_Matrix::ECE_Matrix(int num_rows) : ECE_Matrix(num_rows, 0.0) {}
ECE_Matrix::ECE_Matrix(int num_rows, double init_value) : ECE_Matrix(num_rows, num_rows, init_value) {}
// Main constructor
ECE_Matrix::ECE_Matrix(int num_rows, int num_columns, double init_value)
{
    this->num_rows = num_rows;
    this->num_columns = num_columns;
    this->vector = std::vector< std::vector<double> > (num_rows, \
        std::vector<double>(num_columns, init_value));
}
// Constructor for txt files
ECE_Matrix::ECE_Matrix(char const *file_name) : ECE_Matrix(0)
{
    // Open file
    std::ifstream my_file (file_name);
    if (!my_file.is_open())
    {
        return;
    }

    // Initialyze fields
    my_file >> this->num_rows;
    my_file >> this->num_columns;
    this->vector = std::vector< std::vector<double> > (num_rows, \
        std::vector<double>(num_columns, 0));

    // Populate matrix
    for (int ii = 0; ii < this->num_rows; ii++)
    {
        for (int jj = 0; jj < this->num_columns; jj++)
        {
            my_file >> this->vector[ii][jj];
        }
    }
    my_file.close();
}

// Override << operator
std::ostream &operator <<(std::ostream &stream, const ECE_Matrix &matrix)
{
    // loop over rows
    for (int ii = 0; ii < matrix.num_rows; ii++)
    {
        // loop over collumns
        for (int jj = 0; jj < matrix.num_columns; jj++)
        {
            // query and print matrix element
            stream << std::scientific << std::setw(12) << std::setprecision(3) << matrix.vector[ii][jj];
        }
        stream << std::endl;
    }
    return stream;
}

// Override + operator
ECE_Matrix operator +(const ECE_Matrix &matrix_1, double number)
{
    // declare local variables
    int num_rows = matrix_1.get_num_rows();
    int num_columns = matrix_1.get_num_columns();

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) +  number;
        }
    }
    return matrix_result;
}

// Override + operator
ECE_Matrix operator +(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // declare local variables
    int num_rows = std::max(matrix_1.get_num_rows(), matrix_2.get_num_rows());
    int num_columns = std::max(matrix_1.get_num_columns(), matrix_2.get_num_columns());

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) + matrix_2.get_value(ii, jj);
        }
    }
    return matrix_result;
}

// Override += operator
void operator +=(ECE_Matrix &matrix_1, double number)
{
    // call previously defined functions
    matrix_1 = matrix_1 + number;
}

// Override += operator
void operator +=(ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // call previously defined functions
    matrix_1 = matrix_1 + matrix_2;
}

// Override - operator
ECE_Matrix operator -(const ECE_Matrix &matrix_1, double number)
{
    // declare local variables
    int num_rows = matrix_1.get_num_rows();
    int num_columns = matrix_1.get_num_columns();

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) -  number;
        }
    }
    return matrix_result;
}

// Override - operator
ECE_Matrix operator -(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // declare local variables
    int num_rows = std::max(matrix_1.get_num_rows(), matrix_2.get_num_rows());
    int num_columns = std::max(matrix_1.get_num_columns(), matrix_2.get_num_columns());

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) - matrix_2.get_value(ii, jj);
        }
    }
    return matrix_result;
}

// Override -= operator
void operator -=(ECE_Matrix &matrix_1, double number)
{
    // call previously defined functions
    matrix_1 = matrix_1 - number;
}

// Override -= operator
void operator -=(ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // call previously defined functions
    matrix_1 = matrix_1 - matrix_2;
}

// Override * operator
ECE_Matrix operator *(const ECE_Matrix &matrix_1, double number)
{
    // declare local variables
    int num_rows = matrix_1.get_num_rows();
    int num_columns = matrix_1.get_num_columns();

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) *  number;
        }
    }
    return matrix_result;
}

// Override * operator
ECE_Matrix operator *(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // declare local variables
    int num_rows = std::max(matrix_1.get_num_rows(), matrix_2.get_num_rows());
    int num_columns = std::max(matrix_1.get_num_columns(), matrix_2.get_num_columns());

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) * matrix_2.get_value(ii, jj);
        }
    }
    return matrix_result;
}

// Override / operator
ECE_Matrix operator /(const ECE_Matrix &matrix_1, double number)
{
    // declare local variables
    int num_rows = matrix_1.get_num_rows();
    int num_columns = matrix_1.get_num_columns();

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) / number;
        }
    }
    return matrix_result;
}

// Override / operator
ECE_Matrix operator /(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2)
{
    // declare local variables
    int num_rows = std::max(matrix_1.get_num_rows(), matrix_2.get_num_rows());
    int num_columns = std::max(matrix_1.get_num_columns(), matrix_2.get_num_columns());

    // instanciate new matrix
    ECE_Matrix matrix_result(num_rows, num_columns, 0.0);

    // loop over rows
    for(int ii = 0; ii < num_rows; ii++)
    {
        // loop over collumns
        for(int jj = 0; jj < num_columns; jj++)
        {
            // update matrix value
            matrix_result.vector[ii][jj] = matrix_1.get_value(ii, jj) / matrix_2.get_value(ii, jj);
        }
    }
    return matrix_result;
}

// Transpose method
ECE_Matrix ECE_Matrix::transpose() const
{
    // declare local variables
    int num_rows = this->num_rows;
    int num_columns = this->num_columns;

    // instanciate new matrix
    ECE_Matrix matrix_result(num_columns, num_rows, 0.0);

    // loop over collumns
    for(int ii = 0; ii < num_columns; ii++)
    {
        // loop over rows
        for(int jj = 0; jj < num_rows; jj++)
        {
            // swap rows and collumns
            matrix_result.vector[ii][jj] = this->vector[jj][ii];
        }
    }
    return matrix_result;
}

// Getter methods
int ECE_Matrix::get_num_rows() const
{
    return this->num_rows;
}

int ECE_Matrix::get_num_columns() const
{
    return this->num_columns;
}

double ECE_Matrix::get_value(int ii, int jj) const
{
    // check ifm index is out of bounds
    if (ii >= this->num_rows or jj >= this->num_columns)
    {
        return 0;
    }
    // return value
    return this->vector[ii][jj];
}
