#ifndef MYMATHLIB_H_INCLUDED
#define MYMATHLIB_H_INCLUDED

extern unsigned get_det(double** matrix, unsigned length, double* det);
/* ���������� ������������ */
extern unsigned reverse_matrix(double*** matrix, unsigned length);
/* �������� ������� � ��� �� ������� ������ */
extern unsigned solve_matrix_eq(double** matrix, double* free_vec, unsigned length);
/* ������ ������� ���������
    ��������� ������ ������� � ���������� ���������� �� ������� ���������
*/


#endif // MYMATHLIB_H_INCLUDED
