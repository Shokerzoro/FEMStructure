#ifndef FEM_SOLVER_INTERFACE_H_INCLUDED
#define FEM_SOLVER_INTERFACE_H_INCLUDED

extern unsigned solve_system(void);
/* Отправка на решение
     Возврат 0 - неудача.
*/
extern unsigned read_point_displacement(void);
//
extern unsigned read_internal_forces(void);
//
extern unsigned read_reactions(void);
//
extern unsigned read_max_stress(void);
//

#endif // FEM_SOLVER_INTERFACE_H_INCLUDED
