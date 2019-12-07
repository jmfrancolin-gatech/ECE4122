/*
Author: Joao Matheus Francolin
Class: ECE 4122
Last date modified: September 25, 2019
Description: Implements ECE_Matrix header
*/

#ifndef ECE_MATRIX_H
#define ECE_MATRIX_H

#include <iostream>
#include <vector>

class ECE_Matrix
{
private:

    // private fields
    int num_rows;
    int num_columns;
    std::vector<std::vector<double> > vector;

public:

    // constructors
    ECE_Matrix();
    ECE_Matrix(int num_rows);
    ECE_Matrix(int num_rows, double init_value);
    ECE_Matrix(int num_rows, int num_columns, double init_value);
    ECE_Matrix(char const *file_name);

    // class methods
    int get_num_rows() const;
    int get_num_columns() const;
    double get_value(int ii, int jj) const;
    void set_value(int ii, int jj, double value);
    ECE_Matrix transpose() const;

    // std library overrides

    // overrides << operator
    friend std::ostream & operator <<(std::ostream &stream, const ECE_Matrix &matrix);

    // overrides + operator
    friend ECE_Matrix operator +(const ECE_Matrix &matrix_1, double number);
    friend ECE_Matrix operator +(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);

    // overrides += operator
    friend void operator +=(ECE_Matrix &matrix_1, double number);
    friend void operator +=(ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);

    // overrides - operator
    friend ECE_Matrix operator -(const ECE_Matrix &matrix_1, double number);
    friend ECE_Matrix operator -(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);

    // overrides -= operator
    friend void operator -=(ECE_Matrix &matrix_1, double number);
    friend void operator -=(ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);

    // overrides * operator
    friend ECE_Matrix operator *(const ECE_Matrix &matrix_1, double number);
    friend ECE_Matrix operator *(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);

    // overrides / operator
    friend ECE_Matrix operator /(const ECE_Matrix &matrix_1, double number);
    friend ECE_Matrix operator /(const ECE_Matrix &matrix_1, const ECE_Matrix &matrix_2);
};

#endif
