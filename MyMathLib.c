/*
�� ���������� ������� ������������ ������ ������� ������,
������� ��� row ���� ������� ������� (������ ����� �����).
��� �������� �������� ����������, ������� � ����� �������� � �������� ��������:
��� ������� ������ � ���� ������.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define TESTING
#undef TESTING
 // - - - - - - - - - - - - - - - - - - - ������������ - - - - - - - - - - - - - - - - - - - - - - - - - //
unsigned get_det(double** matrix, unsigned length, double* det);
static double get_det_rec(double** matrix, unsigned length, unsigned top_row, unsigned mdata[]);
static double get_simple_det(double** matrix, unsigned length, unsigned to_row, unsigned mdata[]);
static unsigned is_solvable(unsigned length, unsigned mdata[]);
static void fill_mdata(unsigned length, unsigned* mdata);
static void cpy_mdata(unsigned length, unsigned* mdata, unsigned* mdata_cpy);
 // - - - - - - - - - - - - - - - - - - - ��������� ������� - - - - - - - - - - - - - - - - - - - - - - - - - //
unsigned reverse_matrix(double*** matrix, unsigned length); // �������������� �� ��������� ������ ������� //
static unsigned get_minor(double** matrix, unsigned length, unsigned row, unsigned col, double* minor);
static unsigned cut_matrix(double** new_matrix, double** matrix, unsigned length, unsigned row, unsigned col);
 // - - - - - - - - - - - - - - - - - - - ������� ������� - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
 unsigned solve_matrix_eq(double** matrix, double* free_vec, unsigned length); // ������� �������
 static unsigned solve_pass_direct(double** matrix, double* free_vec, unsigned length, unsigned col); //������ ��������� ������
 static unsigned solve_pass_reverse(double** matrix, double* free_vec, unsigned length, int col); //������ ��������� ��������
 static unsigned normalize_matrix(double** matrix, double* free_vec, unsigned row); //������������� ����
 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
 void read_matrix(double** matrix, unsigned length);
 extern void read_system(unsigned length);

 unsigned solve_matrix_eq(double** matrix, double* free_vec, unsigned length) //�������� ������ � �������� � ���������� ���������� ������� ���������
 {
     #ifdef TESTING
     puts("SOLVER STARTED SYSTEM SOLVING");
     read_system(length);
     #endif // TESTING
     for(unsigned col = 0; col < length; col++) //������ ������
     {
        solve_pass_direct(matrix, free_vec, length, col);
     }
     #ifdef TESTING
     printf("DIRECT PASS COMPLETED\n");
     read_system(length);
     #endif // TESTING
    for(int col = length-1; col > -1; col--) //�������� ������
    {
        solve_pass_reverse(matrix, free_vec, length, col);
    }
    #ifdef TESTING
     printf("REVERSE PASS COMPLETED\n");
     read_system(length);
     #endif // TESTING
    for(unsigned row = 0; row < length; row++)
    {
        normalize_matrix(matrix, free_vec, row);
    }
    #ifdef TESTING
     printf("GAUSS METHOD COMPLETED\n");
     read_system(length);
     #endif // TESTING
 }

 static unsigned normalize_matrix(double** matrix, double* free_vec, unsigned row) //������������� ����
 {
     double relation = 1 / (*(*(matrix + row) + row));
     *(*(matrix + row) + row) = 1;
     *(free_vec + row) *= relation;
 }

static unsigned solve_pass_reverse(double** matrix, double* free_vec, unsigned length, int col) //������ ��������� ��������
{
    #ifdef TESTING
    printf("Making reverse pass\n");
    #endif // TESTING
    //col - ������ �� ������ � ������� ������������ ������� �������� ���������� ������

    for(unsigned mod_row = 0; mod_row < col; mod_row++)
    {
        //unsigned mod_row = col-1;
        double relation = (*(*(matrix + mod_row) + col)) / (*(*(matrix + col) + col));
        //��������� ��������
        *(*(matrix + mod_row) + col) = 0.0;
        //���������� �� free_vec
        *(free_vec + mod_row) -= (*(free_vec + col)) * relation;
    }


    return 1;
}

 static unsigned solve_pass_direct(double** matrix, double* free_vec, unsigned length, unsigned col)
 {
     //col - ������ �� ������ � ������� ������������ ������� �������� ���������� ������
     for(unsigned mod_row = col+1; mod_row < length; mod_row++) //������ �� ���� �������������� �����
     {
         // ������ ������� ���� mod_row ������ � ������� col
         // ������ ������� �� ������� ��������� col, col
         double relation = (*(*(matrix + mod_row) + col)) / *(*(matrix+col) + col);
         for(unsigned mod_col = col; mod_col < length; mod_col++) //������ �� ������ mod_row, �������� ������� mod_col
         {
                //�� �������� mod_row, mod_col �������� ������� ������ col, ������� mod_col ����������� �� relation
                *(*(matrix + mod_row) + mod_col) -= (*(*(matrix + col) + mod_col)) * relation;
         }
         // �� �������� mod_row ������� �������� ������� ������� col*relation
         *(free_vec + mod_row) -= (*(free_vec + col))*relation;
         // ��������� ��������
         *(*(matrix + mod_row) + col) = 0.0;
     }
     return 1;
 }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

unsigned reverse_matrix(double*** matrix, unsigned length)
{
    #ifdef TESTING
    printf("Reversing system\n");
    #endif // TESTING
    double** old_matrix = *matrix; //���������� ������
    double** new_matrix = (double**)malloc(sizeof(double*)*length); //��������� ������
    for(unsigned row = 0; row < length; row++)
    {
        *(new_matrix + row) = (double*)calloc(length, sizeof(double));
    }
    *matrix = new_matrix; //������� ����� ������� �� ����� �����

    double matrix_det = 0.0;
    get_det(old_matrix, length, &matrix_det); //������� ������������
    #ifdef TESTING
    printf("Matrix det: %f\n", matrix_det);
    #endif // TESTING

    double minor;
    for(unsigned row = 0; row < length; row++) //�������� �������������� ����������
    {
        for(unsigned col = 0; col < length; col++)
        {
            get_minor(old_matrix, length, row, col, &minor);
            if(abs(minor) > 0.0000001)
            {
                minor = minor/matrix_det;
                if((row+col)%2 == 0) //�������� �����
                    *(*(new_matrix + col) + row) = minor;
                else
                    *(*(new_matrix + col) + row) = minor * (-1);
            }
            else
            {
                *(*(new_matrix + col) + row) = 0;
            }
        }
    }
    free(old_matrix);
    return 0;
}

static unsigned get_minor(double** matrix, unsigned length, unsigned row, unsigned col, double* minor)
{
        double** temp_matrix = (double**)malloc(sizeof(double*)*(length-1)); //��������� ������
        for(unsigned row = 0; row < (length-1); row++)
        {
            *(temp_matrix + row) = (double*)malloc(sizeof(double)*(length-1));
        }

        cut_matrix(temp_matrix, matrix, length, row, col);
        get_det(temp_matrix, length-1, minor);

        #ifdef TESTING
        printf("Minor formed, det=%.18f\n", *minor);
        read_matrix(temp_matrix, length-1);
        #endif // TESTING
        free(temp_matrix);
        return 0;
}

static unsigned cut_matrix(double** temp_matrix, double** matrix, unsigned length, unsigned row, unsigned col)
{
    unsigned row_counter = 0;
    unsigned col_counter = 0;
    for(unsigned nm_row = 0; nm_row < row; nm_row++)
    {
        for(unsigned nm_col = 0; nm_col < col; nm_col++)
        {
            *(*(temp_matrix + row_counter) + col_counter) = *(*(matrix + nm_row) + nm_col);
            col_counter++;
        }
        for(unsigned nm_col = col+1; nm_col < length; nm_col++)
        {
            *(*(temp_matrix + row_counter) + col_counter) = *(*(matrix + nm_row) + nm_col);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
    for(unsigned nm_row = row+1; nm_row < length; nm_row++)
    {
        for(unsigned nm_col = 0; nm_col < col; nm_col++)
        {
            *(*(temp_matrix + row_counter) + col_counter) = *(*(matrix + nm_row) + nm_col);
            col_counter++;
        }
        for(unsigned nm_col = col+1; nm_col < length; nm_col++)
        {
            *(*(temp_matrix + row_counter) + col_counter) = *(*(matrix + nm_row) + nm_col);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
    return 0;
}

void read_matrix(double** matrix, unsigned length)
{
    for(unsigned row = 0; row < length; row++)
    {
        for(unsigned col = 0; col < length; col++)
        {
            printf("%f\t", *(*(matrix + row) + col));
        }
        printf("\n");
    }
}

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

unsigned get_det(double** matrix, unsigned length, double* det_reg) //��������� ���������� �� ������
{
    double det = 0.0;
    for(unsigned counter = 0; counter < length; counter++)
    {
        unsigned mdata[length];    //����������� ������ ��� ����������� �������
        fill_mdata(length, mdata);
        mdata[counter] = 0;

        if(((counter+1)%2) == 1) //����� ��������, ���������� ������� ������
            det += (*(*matrix + counter)) * get_det_rec(matrix, length, 1, mdata);
        else
            det += (*(*matrix + counter)) * get_det_rec(matrix, length, 1, mdata) * (-1);
    }

    *det_reg = det;
    return 0;
}

static double get_det_rec(double** matrix, unsigned length, unsigned top_row, unsigned mdata[]) //���������� �������
{
    double det = 0.0; //������������ ��������
    if(is_solvable(length, mdata)) //����� �� ��������
    {
        det += get_simple_det(matrix, length, top_row, mdata);
        return det;
    }

    unsigned index = 1; //������ ��� ������������ ��������� � ����������
    for(unsigned counter = 0; counter < length; counter++) //���������� �� ������...
    {
        if(mdata[counter] != 0) // ...���������� ������ ��� ��������� ����������
        {
            unsigned mdata_cpy[length];
            cpy_mdata(length, mdata, mdata_cpy);
            mdata_cpy[counter] = 0;

            if((index%2) == 1) //����� ��������, ���������� ������� ������
                det+= (*(*(matrix + top_row) + counter)) * get_det_rec(matrix, length, top_row+1, mdata_cpy);
            else
                det += (*(*(matrix + top_row) + counter)) * get_det_rec(matrix, length, top_row+1, mdata_cpy) * (-1);

            index++; //����������� � ����������
        }
    }

    return det;
}

static double get_simple_det(double** matrix, unsigned length, unsigned top_row, unsigned mdata[]) //������� "� ���" ���������� 2�2
{

    double det = 0.0;
    unsigned b1 = 0;
    unsigned b2 = 0;
    for(unsigned counter = 0; counter < length; counter++)
    {
        if(mdata[counter] != 0)
        {
            if(b1 == 0)
                b1 = mdata[counter];
            if(b1 != 0)
                b2 = mdata[counter];
        }
    }
    b1--; b2--; //��� ��������� ����������
    det += (*(*(matrix + top_row) + b1))*(*(*(matrix + top_row + 1) + b2)) - (*(*(matrix + top_row+1) + b1))*(*(*(matrix + top_row) + b2));
    return det;
}

static unsigned is_solvable(unsigned length, unsigned mdata[]) //�������� �� ������ "� ���", 1 - ��, 0 - ���
{
    unsigned reg = 0;
    for(unsigned counter = 0; counter < length; counter ++)
    {
        if(mdata[counter] != 0)
            reg++;
    }
    if(reg > 2)
        return 0;
    else
        return 1;
}

static void cpy_mdata(unsigned length, unsigned* mdata, unsigned* mdata_cpy) //�������� ������ ������ length �� mdata � mdata_cpy
{
    for(unsigned counter = 0; counter < length; counter ++)
    {
        *(mdata_cpy + counter) = *(mdata + counter);
    }
}

static void fill_mdata(unsigned length, unsigned* mdata) //���������� ������� � 0 �� (length-1)
{
    for(unsigned counter = 0; counter < length; counter++)
    {
        *(mdata + counter) = counter+1;
    }
}
