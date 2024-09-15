#ifndef LINKEDLISTINTERFACE_H_INCLUDED
#define LINKEDLISTINTERFACE_H_INCLUDED

#include "Structs.h"

extern struct simple_list* list_init(void);
/*Инициализация списка
    Возвращает указатель на структуру simple list */
extern struct node* get_node(struct simple_list* list_name, unsigned node_id);
/*Возвращает указатель на узел по его id    */
extern void add_node(struct simple_list* list_name);
/* Добавляет узел (в конец списка)
    Принимает указатель на функцию вида void* (*struct_create)
    После выполенения остается current_node с неприведенным типом указателя*/
extern void delete_node(struct simple_list* list_name, unsigned node_id); //Удаляет узел и связанную с ним информация
/* Удаляет узел и связанную с ним информацию
    Принимает уникальный id узла */
extern void bypass_list(struct simple_list* list_name, void (*bypass_func)(struct node* node));
/* Применение функции к каждому элементу
    Принимает указатель на функцию обработки элемента*/
extern void bypass_list_expanded(struct simple_list* list_name, unsigned uns_num, void (*bypass_func)(struct node* node, unsigned num));
/* Применение функции к каждому элементу */
extern void bypass_list_flag(struct simple_list* list_name, unsigned* flag_ptr, unsigned num, void (*bypass_func)(struct node* node, unsigned* flag_ptr, unsigned num));
/* Применение функции к каждому элементу
    Доступ к флагу по указателю*/


#endif // LINKEDLISTINTERFACE_H_INCLUDED
