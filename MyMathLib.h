#ifndef MYMATHLIB_H_INCLUDED
#define MYMATHLIB_H_INCLUDED

extern unsigned get_det(double** matrix, unsigned length, double* det);
/* Нахождение определителя */
extern unsigned reverse_matrix(double*** matrix, unsigned length);
/* Обращает матрицу в той же области памяти */
extern unsigned solve_matrix_eq(double** matrix, double* free_vec, unsigned length);
/* Решает систему уравнений
    Принимает только матрицы с ненулевыми элементами на главной диагонали
*/


#endif // MYMATHLIB_H_INCLUDED
