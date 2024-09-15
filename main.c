/*Логика работы:
Всплывающее меню, предлагающее:
1) Добавление, удаление, редактирование элементов, получение информации(точки, нагрузки, сечения, балки).
2) Сохранение в файл, загрузка из файла.
2) Отрисовка заданых структур (точки, балки).
3) Отправка на расчет, получение информации.
4) Отрисовка полученных результатов.
*/

#include <stdio.h>
#include <stdlib.h>
// - - - - - - - - - - - - - - - - - - - - - - - - - //
#include "FEM_data_interface.h"
#include "FEM_solver_interface.h"
// - - - - - - - - - - - - - - - - - - - - - - - - - //
 // Система: м, Н, Па

int main()
{
    points_init();
    loads_init();
    sections_init();
    beams_init();

    bridge();

    read_points();
    solve_system();
    read_points();

    return 0;
}

void bridge(void)
{
    add_point(1, 1);
    add_point(3, 2);
    add_point(1, 3);
    add_point(3, 4);
    add_point(1, 5);
    add_beam(0, 1, 2);
    add_beam(0, 2, 3);
    add_beam(0, 3, 4);
    add_beam(0, 4, 5);
    add_beam(0, 1, 3);
    add_beam(0, 2, 4);
    add_beam(0, 3, 5);
    add_section(210000000000000, 0.7);
    add_load(0, 0.4, 0);
    assign_section(1, 1);
    assign_section(2, 1);
    assign_section(3, 1);
    assign_section(4, 1);
    assign_section(5, 1);
    assign_section(6, 1);
    assign_section(7, 1);
    assign_load(3, 1);
    assign_constraint(1, 3);
    assign_constraint(5, 3);
}

