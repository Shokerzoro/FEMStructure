#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#define POINT_TYPE 1
#define SECTION_TYPE 2
#define LOAD_TYPE 3
#define BEAM_TYPE 4

struct point{
double x;
double y;
unsigned load_id;
unsigned constraint;
double disp_x;
double disp_y;
};

struct section{
double elastity;
double area;
};

struct load{
double x_comp;
double y_comp;
double moment;
};

struct beam{
unsigned section_id;
unsigned start_point_id;
unsigned end_point_id;
};

union fem_data{
struct point* point_data;
struct section* section_data;
struct load* load_data;
struct beam* beam_data;
};

struct node{
unsigned node_id;
union fem_data node_data;
struct node *next_node;
};

struct simple_list{
struct node* first_node;
struct node* current_node;
unsigned num;
unsigned data_type;
};

#endif // STRUCTS_H_INCLUDED
