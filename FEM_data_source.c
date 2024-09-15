//

#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "LinkedListInterface.h"
#define TESTING
#undef TESTING

// - - - - - - - - - - - - - - - - - - - - - - - //
static struct simple_list *points_list;
static struct simple_list *sections_list;
static struct simple_list *loads_list;
static struct simple_list *beams_list;
// - - - - - - - - - - - - - - - - - - - - - - - //
void get_points_list(struct simple_list** points_list_ptr, unsigned* flag);
void get_sections_list(struct simple_list** sections_list_ptr, unsigned* flag);
void get_loads_list(struct simple_list** loads_list_ptr, unsigned* flag);
void get_beams_list(struct simple_list** beams_list_ptr, unsigned* flag);
// - - - - - - - - - - - - - - - - - - - - - - - //
static struct point* point_malloc(void);
static void load_deleting_support(unsigned load_id);
static void load_deleting_support_service(struct node* node, unsigned load_id);
static void read_point(struct node* node);
void points_init(void);
void add_point(double x, double y);
void delete_point(unsigned point_id);
void read_points(void);
void assign_load(unsigned point_id, unsigned load_id);
void assign_constraint(unsigned point_id, unsigned constr_type);
void get_coord(unsigned point_id, double* x, double* y);
// - - - - - - - - - - - - - - - - - - - - - - - //
static struct section* section_malloc(void);
static void read_section(struct node* node);
void sections_init(void);
void add_section(double elastity, double area);
void delete_section(unsigned section_id);
void read_sections(void);
// - - - - - - - - - - - - - - - - - - - - - - - //
static struct load* load_malloc(void);
static void read_load(struct node* node);
void loads_init(void);
void add_load(double x_comp, double y_comp, double moment);
void delete_load(unsigned load_id);
void read_loads(void);
// - - - - - - - - - - - - - - - - - - - - - - - //
static struct beam* beam_malloc(void);
static unsigned point_deleting_support(unsigned point_id); //Возврат можно/нельзя
static void section_deleting_support(unsigned section_id); //Удаление + чистка
static void section_deleting_support_service(struct node* node, unsigned section_id); //Вспомогательная функция
static void read_beam(struct node* node);
void beams_init(void);
void add_beam(unsigned section_id, unsigned start_point_id, unsigned end_point_id);
void delete_beam(unsigned beam_id);
void read_beams(void);
void assign_section(unsigned beam_id, unsigned section_id);
// - - - - - - - - - - - - - - - - - - - - - - - //

void get_points_list(struct simple_list** points_list_ptr, unsigned* flag) //0 - ошибка
{
    if(points_list)
    {
        *points_list_ptr = points_list;
    }
    else
    {
        *flag = 0;
    }
}
void get_sections_list(struct simple_list** sections_list_ptr, unsigned* flag)
{
    if(sections_list)
    {
        *sections_list_ptr = sections_list;
    }
    else
    {
        *flag = 0;
    }
}
void get_loads_list(struct simple_list** loads_list_ptr, unsigned* flag)
{
    if(loads_list)
    {
        *loads_list_ptr = loads_list;
    }
    else
    {
        *flag = 0;
    }
}
void get_beams_list(struct simple_list** beams_list_ptr, unsigned* flag)
{
    if(beams_list)
    {
        *beams_list_ptr = beams_list;
    }
    else
    {
        *flag = 0;
    }
}

void assign_section(unsigned beam_id, unsigned section_id)
{
    if(beams_list)
    {
        struct node* temp_node = get_node(beams_list, beam_id);
        if(temp_node)
        {
            temp_node->node_data.beam_data->section_id = section_id;
        }
    }
}

static struct beam* beam_malloc(void)
{
    struct beam* temp_beam = NULL;
    temp_beam = (struct beam*)malloc(sizeof(struct beam));
    if(temp_beam == NULL)
    {
        puts("ERROR STRUCT BEAM ALLOCATE");
        exit(88);
    }
    #ifdef TESTING
    puts("Struct beam allocated");
    #endif // TESTING
    return temp_beam;
}

static void point_deleting_support_service(struct node* node, unsigned* flag_ptr, unsigned point_id)
{
    if(node->node_data.beam_data->end_point_id == point_id)
    {
        *flag_ptr = 1;
        #ifdef TESTING
        puts("Error deleting point: included as end point in beam structure");
        #endif // TESTING
    }
    if(node->node_data.beam_data->start_point_id == point_id)
    {
        *flag_ptr = 1;
        #ifdef TESTING
        puts("Error deleting point: included as start point in beam structure");
        #endif // TESTING
    }
}

static unsigned point_deleting_support(unsigned point_id) //Возврат запрет - 1/разрешение - 0
{
    #ifdef TESTING
    printf("Checking possibility of deleting point id=%u\n", point_id);
    #endif // TESTING
    if(beams_list)
    {
        unsigned flag = 0;
        bypass_list_flag(beams_list, &flag, point_id, point_deleting_support_service);
        return flag;
    }
    else
    {
        #ifdef TESTING
        puts("No beams created: point deleting approved");
        #endif // TESTING
        return 0;
    }
}

static void section_deleting_support_service(struct node* node, unsigned section_id) //Вспомогательная функция
{
    if(node->node_data.beam_data->section_id == section_id)
    {
        node->node_data.beam_data->section_id = 0;
    }
    if(node->node_data.beam_data->section_id > section_id)
    {
        node->node_data.beam_data->section_id --;
    }
}

static void section_deleting_support(unsigned section_id) //Удаление + чистка
{
    #ifdef TESTING
    printf("Supporting section id=%u delete\n", section_id);
    #endif // TESTING
    if(beams_list)
    {
        bypass_list_expanded(beams_list, section_id, section_deleting_support_service);
    }
}

static void read_beam(struct node* node)
{
    printf("%u\t %u\t %u\t\n",
           node->node_data.beam_data->start_point_id,
           node->node_data.beam_data->end_point_id,
           node->node_data.beam_data->section_id);
}

void beams_init(void)
{
    beams_list = list_init();
    #ifdef TESTING
    puts("Beams list initialized");
    #endif // TESTING
    beams_list->data_type = BEAM_TYPE;
}

void add_beam(unsigned section_id, unsigned start_point_id, unsigned end_point_id)
{
    if(beams_list)
    {
        struct beam* temp_beam = beam_malloc();
        temp_beam->start_point_id = start_point_id;
        temp_beam->end_point_id = end_point_id;
        temp_beam->section_id = section_id;
        add_node(beams_list);
        beams_list->current_node->node_data.beam_data = temp_beam;
        #ifdef TESTING
        printf("Beam created: p1=%u, p2=%u, sec=%u\n", start_point_id, end_point_id, section_id);
        #endif // TESTING
    }
}

void delete_beam(unsigned beam_id)
{
    #ifdef TESTING
    printf("Trying to delete beam id=%u\n", beam_id);
    #endif // TESTING
    if(beams_list)
    {
        delete_node(beams_list, beam_id);
    }
}

void read_beams(void)
{
    if(beams_list)
    {
        printf("p1\t p2\t section\t\n");
        bypass_list(beams_list, read_beam);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - //
static struct load* load_malloc(void)
{
    struct load* temp_load = NULL;
    if((temp_load = (struct load*)malloc(sizeof(struct load))) == NULL)
    {
        puts("ERROR LOAD ALLOCATED");
        exit(83);
    }
    #ifdef TESTING
    puts("Struct load allocated");
    #endif // TESTING
    return temp_load;
}
static void read_load(struct node* node)
{
    printf("%.2f\t %.2f\t %.2f\t\n",
           node->node_data.load_data->x_comp,
           node->node_data.load_data->y_comp,
           node->node_data.load_data->moment);
}
void loads_init(void)
{
    loads_list = list_init();
    if(loads_list == NULL)
    {
        puts("ERROR SECTION LIST INIT");
        exit(15);
    }
    loads_list->data_type = LOAD_TYPE;
    #ifdef TESTING
    puts("Loads list initialized");
    #endif // TESTING
}
void add_load(double x_comp, double y_comp, double moment)
{
    #ifdef TESTING
    puts("Adding new load in list");
    #endif // TESTING
    struct load* temp_load = load_malloc();
    temp_load->x_comp = x_comp;
    temp_load->y_comp = y_comp;
    temp_load->moment = moment;
    add_node(loads_list);
    loads_list->current_node->node_data.load_data = temp_load;
}
void delete_load(unsigned load_id)
{
    #ifdef TESTING
    printf("Trying to delete node id=%u\n", load_id);
    #endif // TESTING
    delete_node(loads_list, load_id);
    load_deleting_support(load_id);
}
void read_loads(void)
{
    printf("x_comp\t y_comp\t moment\t\n");
    bypass_list(loads_list, read_load);
}

// - - - - - - - - - - - - - - - - - - - - - - - //
static struct section* section_malloc(void)
{
    struct section* temp_section = NULL;
    temp_section = (struct section*)malloc(sizeof(struct section));
    if(temp_section == NULL)
    {
        puts("ERROR SECTION ALLOCATION");
        exit(8);
    }
    #ifdef TESTING
    puts("Struct section malloc");
    #endif // TESTING
    return temp_section;
}
static void read_section(struct node* node)
{
    printf("%.2f \t\t %.2f\t\n", node->node_data.section_data->area,
        node->node_data.section_data->elastity);
}
void sections_init(void)
{
    sections_list = list_init();
    if(sections_list == NULL)
    {
        puts("ERROR SECTION INIT");
        exit(11);
    }
    sections_list->data_type = SECTION_TYPE;
    #ifdef TESTING
    puts("Sections list initialized");
    #endif // TESTING
}
void add_section(double elastity, double area)
{
    #ifdef TESTING
    printf("Adding a section with el=%.2f, area=%.2f\n", elastity, area);
    #endif // TESTING
    struct section* temp_section = section_malloc();
    temp_section->elastity = elastity;
    temp_section->area = area;
    #ifdef TESTING
    printf("New sections elast=%.2f, area=%.2f", temp_section->elastity, temp_section->area);
    #endif // TESTING
    add_node(sections_list);
    sections_list->current_node->node_data.section_data = temp_section;
    #ifdef TESTING
    printf("Section id=%u added in list\n", sections_list->num);
    #endif // TESTING
}
void delete_section(unsigned section_id)
{
    #ifdef TESTING
    printf("Trying to delete section id=%u\n", section_id);
    #endif // TESTING
    delete_node(sections_list, section_id);
    section_deleting_support(section_id);
}

void read_sections(void)
{
    printf("elastity\t area\t\n");
    bypass_list(sections_list, read_section);
}

// - - - - - - - - - - - - - - - - - - - - - - - //

void assign_constraint(unsigned point_id, unsigned constr_type)
{
    struct node* temp_node = get_node(points_list, point_id);
    temp_node->node_data.point_data->constraint = constr_type;
}

void assign_load(unsigned point_id, unsigned load_id)
{
    struct node* temp_node = get_node(points_list, point_id);
    temp_node->node_data.point_data->load_id = load_id;
}

void delete_point(unsigned point_id)
{
    if(points_list)
    {
        #ifdef TESTING
        printf("Trying to delete point id=%u\n", point_id);
        #endif // TESTING
        unsigned check = point_deleting_support(point_id);
        if(check == 0)
        {
            delete_node(points_list, point_id);
        }
        #ifdef TESTING
        if(check == 1)
        {
            puts("Error deleting point: included in beam");
        }
        #endif // TESTING
    }
}

void read_points(void)
{
    printf("x\t y\t\t load\t constr\t disp_x\t\t disp_y\n");
    bypass_list(points_list, read_point);
}

void add_point(double x, double y)
{
    struct point* temp_point = point_malloc();
    temp_point->x = x;
    temp_point->y = y;
    temp_point->load_id = 0;
    temp_point->disp_x = 0.0;
    temp_point->disp_y = 0.0;
    add_node(points_list);
    points_list->current_node->node_data.point_data = temp_point;
    #ifdef TESTING
    printf("New point data: x=%.2f, y=%.2f \n",
           points_list->current_node->node_data.point_data->x,
           points_list->current_node->node_data.point_data->x);
    #endif
}

void points_init(void)
{
    points_list = list_init();
    #ifdef TESTING
    if(points_list)
    {
        puts("Points list initialized");
    }
    #endif // TESTING
    points_list->data_type = POINT_TYPE;
}

static struct point* point_malloc(void)
{
    struct point* temp_point = NULL;
    if((temp_point = (struct point*)malloc(sizeof(struct point))) == NULL)
    {
        puts("ERROR POINT MALLOC");
        exit(5);
    }
    #ifdef TESTING
    puts("Point allocated");
    #endif
    temp_point->constraint = 0;
    temp_point->load_id = 0;
    temp_point->x = 0;
    temp_point->y = 0;
    return temp_point;
}

static void load_deleting_support(unsigned load_id)
{
    if(points_list)
    {
        bypass_list_expanded(points_list, load_id, load_deleting_support_service);
    }
}

static void load_deleting_support_service(struct node* node, unsigned load_id)
{
    if(node->node_data.point_data->load_id == load_id)
    {
        node->node_data.point_data->load_id = 0;
    }
    if(node->node_data.point_data->load_id > load_id)
    {
        node->node_data.point_data->load_id --;
    }
}

static void read_point(struct node* node)
{
    printf("%.2f\t %.2f\t %u\t %u\t %.5f\t %.5f\t\n",
           node->node_data.point_data->x,
           node->node_data.point_data->y,
           node->node_data.point_data->load_id,
           node->node_data.point_data->constraint,
           node->node_data.point_data->disp_x,
           node->node_data.point_data->disp_y);
}

void get_coord(unsigned point_id, double* x, double* y)
{
    if(points_list)
    {
        struct node* temp_node = get_node(points_list, point_id);
        if(temp_node)
        {
            *x = temp_node->node_data.point_data->x;
            *y = temp_node->node_data.point_data->y;
        }
    }
}
