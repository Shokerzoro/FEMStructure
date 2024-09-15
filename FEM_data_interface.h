#ifndef FEM_DATA_INTERFACE_H_INCLUDED
#define FEM_DATA_INTERFACE_H_INCLUDED

// - - - - - - - - - - - - - - - - - - - - - - - - - //
extern void get_points_list(struct simple_list** points_list_ptr, unsigned* flag);
extern void get_sections_list(struct simple_list** sections_list_ptr, unsigned* flag);
extern void get_loads_list(struct simple_list** loads_list_ptr, unsigned* flag);
extern void get_beams_list(struct simple_list** beams_list_ptr, unsigned* flag);
// - - - - - - - - - - - - - - - - - - - - - - - //
extern void points_init(void);
extern void add_point(double x, double y);
extern void delete_point(unsigned point_id);
extern void read_points(void);
extern void assign_load(unsigned point_id, unsigned load_id);
extern void assign_constraint(unsigned point_id, unsigned constr_type);
extern void get_coord(unsigned point_id, double* x, double* y);
// - - - - - - - - - - - - - - - - - - - - - - - //
extern void sections_init(void);
extern void add_section(double elastity, double area);
extern void delete_section(unsigned section_id);
extern void read_sections(void);
// - - - - - - - - - - - - - - - - - - - - - - - //
extern void loads_init(void);
extern void add_load(double x_comp, double y_comp, double moment);
extern void delete_load(unsigned load_id);
extern void read_loads(void);
// - - - - - - - - - - - - - - - - - - - - - - - //
extern void beams_init(void);
extern void add_beam(unsigned section_id, unsigned start_point_id, unsigned end_point_id);
extern void delete_beam(unsigned beam_id);
extern void read_beams(void);
extern void assign_section(unsigned beam_id, unsigned section_id);
// - - - - - - - - - - - - - - - - - - - - - - - //

#endif // FEM_DATA_INTERFACE_H_INCLUDED
