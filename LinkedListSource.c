//

#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"

#define TESTING
#undef TESTING

// - - - - - - - - Âíóòðåííèå ôóíêöèè - - - - - - - - - - - - - //
static struct node* malloc_node(void);
static void end_scroll(struct simple_list* list_name);
static unsigned free_node_data(struct simple_list* list_name, struct node* node_ptr); //0 - íåóäà÷à, 1 - óñïåõ
/*ÈÑÏÎËÜÇÓÅÒÑß ÊÀÊ ÂÑÏÎÌÎÃÀÒÅËÜÍÀß Â VOID DELETE NODE
    ÎÑÂÎÁÎÆÄÅÍÈÅ ÏÀÌßÒÈ DEPENDS ON ÒÈÏ ÕÐÀÍÈÌÛÕ ÄÀÍÍÛÕ
    ÒÐÅÁÓÅÒ ÈÇÌÅÍÅÍÈß ÏÐÈ ÄÎÁÀÂËÅÍÈÈ ÍÎÂÛÕ ÑÒÐÓÊÒÓÐ!!!*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

struct simple_list* list_init(void)
{
    struct simple_list* temp_list = NULL;
    if((temp_list = (struct simple_list*)malloc(sizeof(struct simple_list))) == NULL)
    {
        puts("ERROR STRUCT SIMPLE_LIST ALLOCATE");
        exit(9);
    }
    temp_list->first_node = NULL;
    temp_list->current_node = NULL;
    temp_list->num = 0;
    temp_list->data_type = 0;
    return temp_list;
};

struct node* get_node(struct simple_list* list_name, unsigned node_id)
{
    #ifdef TESTING
    printf("Node ptr request by id=%u\n", node_id);
    #endif // TESTING
    if(list_name->num >= node_id)
    {
        unsigned counter = 0;
        list_name->current_node = list_name->first_node;
        for(counter = 0; counter < (node_id - 1); counter++)
        {
            list_name->current_node = list_name->current_node->next_node;
        }
        #ifdef TESTING
        printf("Node ptr request by id=%u accomplished\n", counter);
        #endif // TESTING
        return list_name->current_node;
    }
    return NULL;
};

void add_node(struct simple_list* list_name)
{
    if(list_name->first_node)//Åñëè åñòü ïåðâûé ýëåìåíò
    {
        list_name->current_node = list_name->first_node;
        while(list_name->current_node->next_node)
        {
            list_name->current_node = list_name->current_node->next_node;
        }
        list_name->current_node->next_node = malloc_node();
        list_name->current_node = list_name->current_node->next_node;
        list_name->num ++;
        #ifdef TESTING
        printf("Added %u element\n", list_name->num);
        #endif // TESTING
    }
    else
    {
        list_name->first_node = malloc_node();
        list_name->current_node = list_name->first_node;
        list_name->num = 1;
        #ifdef TESTING
        printf("Added %u element\n", list_name->num);
        #endif // TESTING
    }
}

void delete_node(struct simple_list* list_name, unsigned node_id)
{
    #ifdef TESTING
    printf("Trying to delete point id=%u\n", node_id);
    #endif // TESTING
    if(list_name->num >= node_id)
    {
        if(node_id == 1) //Óäàëåíèå ïåðâîãî ýëåìåíòà
        {
             if(list_name->num == 1) //Ñóùåñòâóò òîëüêî 1 ýëåìåíò, îáíóëåíèå
             {
                 #ifdef TESTING
                 printf("Deleting the only first node\n");
                 #endif // TESTING
                 unsigned check = free_node_data(list_name, list_name->first_node);
                 if(check == 0) {puts("ERROR FREE NODE_DATA"); exit(3); };
                 free(list_name->first_node);
                 list_name->num = 0;
                 list_name->first_node = NULL;
                 list_name->current_node = NULL;
             }
             else
             {
                 #ifdef TESTING
                 printf("Deleting first node with total %u nodes\n", list_name->num);
                 #endif // TESTING
                 list_name->current_node = list_name->first_node->next_node;
                 unsigned check = free_node_data(list_name, list_name->first_node);
                 if(check == 0) {puts("ERROR FREE NODE_DATA"); exit(3); };
                 free(list_name->first_node);
                 list_name->first_node = list_name->current_node;
                 list_name->num --;
             }
        }
        else
        {
            #ifdef TESTING
            printf("Deleting %u node with %u total\n", node_id, list_name->num);
            #endif // TESTING
            list_name->current_node = list_name->first_node;
            for(unsigned counter = 0; counter < (node_id-2); counter++)
            {
                list_name->current_node = list_name->current_node->next_node;
            }
            struct node* temp_node = list_name->current_node->next_node;
            list_name->current_node->next_node = list_name->current_node->next_node->next_node;
            //free(temp_node->node_data);
            unsigned check = free_node_data(list_name, temp_node);
            if(check == 0) {puts("ERROR FREE NODE_DATA"); exit(3); };
            free(temp_node);
            list_name->num --;
        }
    }
}

void bypass_list(struct simple_list* list_name, void (*bypass_func)(struct node* node))
{
    if(list_name->num > 0)
    {
        list_name->current_node = list_name->first_node;
        while(list_name->current_node)
        {
            bypass_func(list_name->current_node);
            list_name->current_node = list_name->current_node->next_node;
        }
    }
}


void bypass_list_expanded(struct simple_list* list_name, unsigned num, void (*bypass_func)(struct node* node, unsigned num))
{
    if(list_name->num > 0)
    {
        list_name->current_node = list_name->first_node;
        while(list_name->current_node)
        {
            bypass_func(list_name->current_node, num);
            list_name->current_node = list_name->current_node->next_node;
        }
    }
}

void bypass_list_flag(struct simple_list* list_name, unsigned* flag_ptr, unsigned num, void (*bypass_func)(struct node* node, unsigned* flag_ptr, unsigned num))
{
    if(list_name->num > 0)
    {
        list_name->current_node = list_name->first_node;
        while(list_name->current_node)
        {
            bypass_func(list_name->current_node, flag_ptr, num);
            list_name->current_node = list_name->current_node->next_node;
        }
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static unsigned free_node_data(struct simple_list* list_name, struct node* node_ptr)
{
    unsigned flag = 0;
    if(list_name->data_type == POINT_TYPE)
    {
        free(node_ptr->node_data.point_data);
        flag = 1;
    }
    if(list_name->data_type == SECTION_TYPE)
    {
        free(node_ptr->node_data.section_data);
        flag = 1;
    }
    if(list_name->data_type == LOAD_TYPE)
    {
        free(node_ptr->node_data.load_data);
        flag = 1;
    }
    if(list_name->data_type == BEAM_TYPE)
    {
        free(node_ptr->node_data.beam_data);
        flag = 1;
    }
    return flag;
}

static struct node* malloc_node(void)
{
    struct node* temp_node = NULL;
    if((temp_node = (struct node*)malloc(sizeof(struct node))) == NULL)
    {
        puts("ERROR NODE ALLOCATE");
        exit(4);
    }
    temp_node->next_node = NULL;
    return temp_node;
}

static void end_scroll(struct simple_list* list_name)
{
    if(list_name->num != 0)
    {
        list_name->current_node = list_name->first_node;
        while(list_name->current_node->next_node)
        {
            list_name->current_node = list_name->current_node->next_node;
        }
    }
}
