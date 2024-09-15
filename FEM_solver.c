/* Файл интегрирует решение задачи
1) Предварительная проверка данных(наличие хотябы двух балок, одной нагрузки);
2) Формирование матрицы жесткости;
3) Проверка разрешимости системы;
4) Дальнейшее формирование данных;
5) Разрешение системы;
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Structs.h"
#include "FEM_data_interface.h"
#include "MyMathLib.h"

#define TESTING
#undef TESTING
#define VERIF

unsigned solve_system(void); // Запускает алгоритм решения, возврат 0 - неудача.
void read_system(unsigned length); //Чтение системы

unsigned read_point_displacement(void); //
unsigned read_internal_forces(void); //
unsigned read_reactions(void); //
unsigned read_max_stress(void); //

// - - - - - - Внутренние функции препроцессора - - - - - - - - //
static void K_local_malloc(void); // Выделение памяти под локальную матрицу
static void K_global_malloc(void); //Выделение памяти под глобальную матрицу
static void R_matrix_malloc(void);
static void R_matrix_form(void);


static unsigned get_data(void); // Получает доступ к созданным данным, возврат 0 - неудача.
static unsigned pre_check(void); // Предварительная проверка, возврат 0 - неудача.
static void K_global_form(unsigned pre_form); //Формирование глобальной матрицы
static void K_local_push(struct beam* current_beam); //Запись локальной матрицы в локальную
static void K_local_form(double alpha, double EAL_coeff); //Формирование локальной матрицы
static double EAL_coeff_find(struct beam* current_beam); //Нахождение коэффициента жесткости
static double alpha_find(struct beam* current_beam); //Нахождение угла наклона
static unsigned apply_constraints(void); // Подготовка системы к решению, возврат размерности
static void cut_system(unsigned id, unsigned length); // Подготовка системы к решению

// - - - - - - Внутренние функции постпроцессора - - - - - - - - //
static void post_pro_displacements(void); //Первая функция постпроцессора, формирует вектор перемещений

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
static struct simple_list *points_list;
static struct simple_list *sections_list;
static struct simple_list *loads_list;
static struct simple_list *beams_list;
static double** K_local;
static double** K_global;
static double* R_matrix;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

unsigned solve_system(void) // Запускает алгоритм решения, возврат 0 - неудача.
{
    #ifdef TESTING
    printf("Begining solving system\n");
    #endif // TESTING
    unsigned flag = 0;
    if((flag = get_data()) == 0)
    {
        return flag;
        #ifdef TESTING
        printf("DATA ERROR\n");
        #endif // TESTING
    }
    if((flag = pre_check()) == 0)
    {
        return flag;
        #ifdef TESTING
        printf("PRE CHECK ERROR\n");
        #endif // TESTING
    }

    K_local_malloc();
    K_global_malloc();
    R_matrix_malloc();

    K_global_form(0);
    R_matrix_form();
    #ifdef TESTING
    read_system(points_list->num*2);
    #endif // TESTING
    unsigned m_len = apply_constraints();
    #ifdef TESTING
    read_system(m_len);
    #endif // TESTING
    solve_matrix_eq(K_global, R_matrix, m_len);
    #ifdef TESTING
    read_system(m_len);
    #endif // TESTING
    post_pro_displacements();

    return flag;
}

static void post_pro_displacements(void)
{
    #ifdef TESTING
    printf("POSTPROCESSING\n");
    #endif // TESTING
    struct node* temp_node = NULL;
    struct point* temp_point = NULL;
    unsigned points_num = points_list->num;
    unsigned offset = 0;
    for(unsigned counter = 0; counter < points_num; counter++)
    {
        temp_node = get_node(points_list, counter+1);
        temp_point = temp_node->node_data.point_data;
        if(temp_point->constraint == 0)
        {
            temp_point->disp_x = *(R_matrix + 2*counter  - offset);
            temp_point->disp_y = *(R_matrix + 2*counter + 1 - offset);
        }
        else
        {
            if(temp_point->constraint == 1)
            {
                offset++;
                temp_point->disp_x = 0.0;
                temp_point->disp_y = *(R_matrix + 2*counter + 1 - offset);
            }
            if(temp_point->constraint == 2)
            {
                offset++;
                temp_point->disp_x = *(R_matrix + 2*counter - offset);
                temp_point->disp_y = *(R_matrix + 2*counter + 1 - offset);
            }
            if(temp_point->constraint == 3)
            {
                offset += 2;
                temp_point->disp_x = 0.0;
                temp_point->disp_y = 0.0;
            }
        }
    }
}
static unsigned apply_constraints(void) //возвращает количество удаленных элементов
{
    unsigned constr_applied = 0;
    unsigned points_num = points_list->num;
    unsigned m_len = points_num*2;
    struct node* temp_node = NULL;
    struct point* temp_point = NULL;
    for(unsigned counter = 0; counter < points_num; counter++)
    {
        temp_node = get_node(points_list, counter+1);
        temp_point = temp_node->node_data.point_data;
        if(temp_point->constraint == 1)
        {
            #ifdef TESTING
            printf("Appling constraint p_id=%u, dof=%u\n", counter+1, counter*2+1);
            #endif // TESTING
            cut_system(counter*2+1, m_len);
            m_len--;
        }
        if(temp_point->constraint == 2)
        {
            #ifdef TESTING
            printf("Appling constraint p_id=%u, dof=%u\n", counter+1, counter*2+2);
            #endif // TESTING
            cut_system(counter*2+2, m_len);
            m_len--;
        }
        if(temp_point->constraint == 3)
        {
            #ifdef TESTING
            printf("Appling constraint p_id=%u, dof=%u, %u\n", counter+1, counter*2+1, counter*2+2);
            #endif // TESTING
            cut_system(counter*2+1, m_len);
            m_len--;
            cut_system(counter*2+1, m_len);
            m_len--;
        }
    }


    return m_len;
}
static void cut_system(unsigned id, unsigned length)
{
    //должна удаляться строка id и столбец id
    double* temp_row = *(K_global + id -1);
    //Потеря указателя, утечка памяти
    for(unsigned row = id-1; row < (length-1); row++)
    {
        *(K_global + row) = *(K_global + row + 1); //Манипулирование указателями
        *(R_matrix + row) = *(R_matrix + row + 1); //Манипулирование данными
    }
    for(unsigned row = 0; row < (length-1); row++) //Для всех рядов
    {
        for(unsigned col = (id-1); col < length; col++)
        {
            if(col == (length-1))
            {
                *(*(K_global + row) + col) = 0.0;
            }
            else
            {
                *(*(K_global + row) + col) = *(*(K_global + row) + col + 1);
            }
        }
    }
    free(temp_row);
}
void read_system(unsigned length)
{
    for(unsigned row = 0; row < length; row ++)
    {
        for(unsigned col = 0; col < length; col++)
        {
            printf("%.2f\t", *(*(K_global + row) + col));
        }
        printf("\t%.18f\n", *(R_matrix + row));
    }
}
static void R_matrix_form(void)
{
    unsigned nodes_num = points_list->num;
    struct node* temp_node = NULL;
    struct point* temp_point = NULL;
    struct load* temp_load = NULL;
    unsigned load_id = 0;
    for(unsigned counter = 0; counter < nodes_num; counter++)
    {
        temp_node = get_node(points_list, counter+1);
        temp_point = temp_node->node_data.point_data;
        load_id = temp_point->load_id;
        if(load_id != 0)
        {
            temp_node = get_node(loads_list, load_id);
            temp_load = temp_node->node_data.load_data;
            *(R_matrix + 2*counter) = temp_load->x_comp;
            *(R_matrix + 2*counter+ 1) = temp_load->y_comp;
        }
    }
}
static void R_matrix_malloc(void)
{
    unsigned r_len = points_list->num*2;
    R_matrix = (double*)calloc(r_len, sizeof(double));
}
static void K_global_form(unsigned pre_form) //Формирование глобальной матрицы
{
    unsigned FE_num = beams_list->num;
    struct node* temp_node = NULL;
    struct beam* temp_beam = NULL;
    double EAL_coeff = 0.0;
    double alpha = 0.0;
    for(unsigned counter = 0; counter < FE_num; counter++)
    {
        temp_node = get_node(beams_list, counter+1);
        temp_beam = temp_node->node_data.beam_data;
        if(pre_form == 1)
        {
            EAL_coeff = 1;
        }
        else
        {
            EAL_coeff = EAL_coeff_find(temp_beam);
        }
        alpha = alpha_find(temp_beam);
        K_local_form(alpha, EAL_coeff);
        K_local_push(temp_beam);
    }
}
static void K_local_push(struct beam* current_beam) //Запись локальной матрицы в глобальную
{
    unsigned sp_id = current_beam->start_point_id;
    unsigned ep_id = current_beam->end_point_id;
    unsigned row_counter = 0;
    unsigned col_counter = 0;
    #ifdef TESTING
    printf("K_local pushing to local matrix, nodes are: %u, %u\n", sp_id, ep_id);
    #endif // TESTING
    for(unsigned row = 0; row < 2; row++)
    {
        for(unsigned col = 0; col < 2; col++)
        {
            *(*(K_global + 2*(sp_id-1) + row_counter) + 2*(sp_id-1) + col_counter) += *(*(K_local+row_counter) + col_counter);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
    row_counter = 0;
    col_counter = 0;
    for(unsigned row = 0; row < 2; row++)
    {
        for(unsigned col = 2; col < 4; col++)
        {
            *(*(K_global + 2*(sp_id-1) + row_counter) + 2*(ep_id-1) + col_counter) += *(*(K_local+row_counter) + col_counter);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
    row_counter = 0;
    col_counter = 0;
    for(unsigned row = 2; row < 4; row++)
    {
        for(unsigned col = 0; col < 2; col++)
        {
            *(*(K_global + 2*(ep_id-1) + row_counter) + 2*(sp_id-1) + col_counter) += *(*(K_local+row_counter) + col_counter);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
    row_counter = 0;
    col_counter = 0;
    for(unsigned row = 2; row < 4; row++)
    {
        for(unsigned col = 2; col < 4; col++)
        {
            *(*(K_global + 2*(ep_id-1) + row_counter) + 2*(ep_id-1) + col_counter) += *(*(K_local+row_counter) + col_counter);
            col_counter++;
        }
        row_counter++;
        col_counter = 0;
    }
}
static void K_global_malloc(void) //Выделение памяти под глобальную матрицу
{
    #ifdef TESTING
    printf("K_global allocating\n");
    #endif // TESTING
    unsigned K_matrix_len = points_list->num;
    K_global = (double**)malloc(sizeof(double*)*K_matrix_len*2);
    for(unsigned row = 0; row < 2*K_matrix_len; row++)
    {
        *(K_global + row) = (double*)calloc(2*K_matrix_len, sizeof(double));
    }
}
static void K_local_form(double alpha, double EAL_coeff) //Формирование локальной матрицы]
{
    #ifdef TESTING
    printf("K_local forming\n");
    #endif // TESTING
    double sap_matrix[] = {sin(alpha), cos(alpha), -sin(alpha), -cos(alpha)};
    for(unsigned row = 0; row < 4; row++)
    {
        for(unsigned col = 0; col < 4; col++)
        {
            *(*(K_local + row) + col) = EAL_coeff*sap_matrix[row]*sap_matrix[col];
        }
    }
}
static double EAL_coeff_find(struct beam* current_beam) //Нахождение коэффициента жесткости
{
    #ifdef TESTING
    printf("Calculation beam rigidity\n");
    #endif // TESTING
    unsigned section_id = current_beam->section_id;
    unsigned start_point_id = current_beam->start_point_id; //temp_beam>start_point_id;
    unsigned end_point_id = current_beam->end_point_id; //temp_beam->end_point_id;
    struct node* temp_section_node = get_node(sections_list, section_id);
    struct section* temp_section = temp_section_node->node_data.section_data;

    double E_mod = temp_section->elastity; //temp_section->elastity;
    double area = temp_section->area; //temp_section->area;
    double sp_x, sp_y, ep_x, ep_y;
    get_coord(start_point_id, &sp_x, &sp_y);
    get_coord(end_point_id, &ep_x, &ep_y);
    double beam_x = ep_x - sp_x;
    double beam_y = ep_y - sp_y;
    double beam_length = sqrt(beam_x*beam_x + beam_y*beam_y);
    double EAL_coeff = E_mod*area/beam_length;
    #ifdef TESTING
    printf("Beam length is %.2f, rigidity is %.2f\n", beam_length, EAL_coeff);
    #endif // TESTING
    return EAL_coeff;
}
static double alpha_find(struct beam* current_beam) //Нахождение угла наклона
{
    #ifdef TESTING
    printf("Trying to find beam angle\n");
    #endif // TESTING
    unsigned sp_id = current_beam->start_point_id;
    unsigned ep_id = current_beam->end_point_id;
    double sp_x, sp_y, ep_x, ep_y;
    get_coord(sp_id, &sp_x, &sp_y);
    get_coord(ep_id, &ep_x, &ep_y);
    double beam_x = ep_x - sp_x;
    double beam_y = ep_y - sp_y;
    double alpha = atan(beam_y/beam_x);
    #ifdef TESTING
    printf("For beam angle is %f\n", alpha);
    #endif // TESTING
    return alpha;
}
static void K_local_malloc(void) // Выделение памяти под локальную матрицу
{
    #ifdef TESTING
    printf("K local allocating\n");
    #endif // TESTING
    K_local= (double**)malloc(sizeof(double**)*4);
    for(unsigned row = 0; row < 4; row ++)
    {
        *(K_local + row) = (double*)malloc(sizeof(double)*4);
    }
    #ifdef TESTING
    for(unsigned row = 0; row < 4; row++)
    {
        printf("%p: ", K_local + row);
        for(unsigned col = 0; col < 4; col++)
        {
            printf("%p %zu,", *(K_local + row) + col, sizeof(*(*(K_local + row) + col)) );
        }
        puts("");
    }
    #endif // TESTING
}
static unsigned pre_check(void) // Предварительная проверка, возврат 0 - неудача.
{
    #ifdef TESTING
    printf("Making pre-check\n");
    #endif // TESTING
    unsigned flag = 1;
    unsigned loads_applied = 0;
    struct node* temp_node = NULL;
    if(beams_list->num < 2)
    {
        flag = 0;
        #ifdef TESTING
        puts("Not enough beams");
        #endif // TESTING
    }
    if(loads_list->num == 0)
    {
        flag = 0;
        #ifdef TESTING
        puts("No loads");
        #endif // TESTING
    }
    if(sections_list->num == 0)
    {
        flag = 0;
        #ifdef TESTING
        puts("No sections");
        #endif // TESTING
    }
    for(unsigned counter = 0; counter < beams_list->num; counter++) //Beams sections checking
    {
        temp_node = get_node(beams_list, counter+1);
        if(temp_node)
        {
            if(temp_node->node_data.beam_data->section_id == 0)
            {
                flag = 0;
                #ifdef TESTING
                printf("Beam id=%u has no section applied\n", counter+1);
                #endif // TESTING
            }
        }
    }
    unsigned counter = 0;
    while((loads_applied == 0) && (counter < points_list->num)) //Any load apllied checking;
    {
        temp_node = get_node(points_list, counter+1);
        if(temp_node->node_data.point_data->load_id != 0)
        {
            loads_applied++;
        }
        counter++;
    }
    if(loads_applied == 0)
    {
        flag = 0;
        #ifdef TESTING
        printf("No loads applied\n");
        #endif // TESTING
    }
    #ifdef TESTING
    printf("Pre-check result: %u\n", flag);
    #endif // TESTING
    return flag;
}
static unsigned get_data(void) // Получает доступ к созданным данным, возврат 0 - неудача.
{
    #ifdef TESTING
    printf("Getting data\n");
    #endif // TESTING
    unsigned flag = 1;
    get_points_list(&points_list, &flag);
    get_sections_list(&sections_list, &flag);
    get_beams_list(&beams_list, &flag);
    get_loads_list(&loads_list, &flag);
    #ifdef TESTING
    printf("Getting data result: %u\n", flag);
    #endif // TESTING
    return flag;
}
