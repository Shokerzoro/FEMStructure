#ifndef LINKEDLISTINTERFACE_H_INCLUDED
#define LINKEDLISTINTERFACE_H_INCLUDED

#include "Structs.h"

extern struct simple_list* list_init(void);
/*������������� ������
    ���������� ��������� �� ��������� simple list */
extern struct node* get_node(struct simple_list* list_name, unsigned node_id);
/*���������� ��������� �� ���� �� ��� id    */
extern void add_node(struct simple_list* list_name);
/* ��������� ���� (� ����� ������)
    ��������� ��������� �� ������� ���� void* (*struct_create)
    ����� ����������� �������� current_node � ������������� ����� ���������*/
extern void delete_node(struct simple_list* list_name, unsigned node_id); //������� ���� � ��������� � ��� ����������
/* ������� ���� � ��������� � ��� ����������
    ��������� ���������� id ���� */
extern void bypass_list(struct simple_list* list_name, void (*bypass_func)(struct node* node));
/* ���������� ������� � ������� ��������
    ��������� ��������� �� ������� ��������� ��������*/
extern void bypass_list_expanded(struct simple_list* list_name, unsigned uns_num, void (*bypass_func)(struct node* node, unsigned num));
/* ���������� ������� � ������� �������� */
extern void bypass_list_flag(struct simple_list* list_name, unsigned* flag_ptr, unsigned num, void (*bypass_func)(struct node* node, unsigned* flag_ptr, unsigned num));
/* ���������� ������� � ������� ��������
    ������ � ����� �� ���������*/


#endif // LINKEDLISTINTERFACE_H_INCLUDED
