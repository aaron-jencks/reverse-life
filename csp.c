#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z3.h>

#include "arraylist.h"
#include "csp.h"
#include "error.h"

bool COMBINATIONS_GENERATED = false;
// general syntax is COMBINATIONS[target alive now][alive previously] = list of combinations as uint8_t*
// in this case 0 = dead and 1 = alive
arraylist_t** COMBINATIONS = NULL;

void generate_combinations() {
    // allocate memory
    COMBINATIONS = malloc(sizeof(arraylist_t*) * 2);
    handle_memory_error(COMBINATIONS);
    COMBINATIONS[0] = malloc(sizeof(arraylist_t) * 2);
    handle_memory_error(COMBINATIONS[0]);
    COMBINATIONS[1] = malloc(sizeof(arraylist_t) * 2);
    handle_memory_error(COMBINATIONS[1]);
    COMBINATIONS[0][0] = create_arraylist(256);
    COMBINATIONS[0][1] = create_arraylist(256);
    COMBINATIONS[1][0] = create_arraylist(256);
    COMBINATIONS[1][1] = create_arraylist(256);

    for(uint16_t i = 0; i < 256; i++) {
        uint8_t* byte = malloc(sizeof(uint8_t));
        *byte = (uint8_t)i; // shave off the upper bit
        bool alive_previous = get_state(*byte, true);
        arraylist_append(&COMBINATIONS[alive_previous][1], byte);
        bool dead_previous = get_state(*byte, false);
        arraylist_append(&COMBINATIONS[dead_previous][0], byte);
    }

    COMBINATIONS_GENERATED = true;
}

char* make_cell_name(const char* prefix, size_t row, size_t column) {
    char* name = malloc(sizeof(char) * (strlen(prefix) + 23));
    sprintf(name, "%s_%zd_%zd", prefix, row, column);
    return name;
}

Z3_ast make_bool_variable(Z3_context ctx, const char* prefix, size_t row, size_t column) {
    Z3_sort ty = Z3_mk_bool_sort(ctx);
    char* name = make_cell_name(prefix, row, column);
    Z3_symbol s = Z3_mk_string_symbol(ctx, name);
    free(name);
    return Z3_mk_const(ctx, s, ty);
}

Z3_ast get_neighbors_products(Z3_context ctx, coord_t size, coord_t position, uint8_t state, Z3_ast** ast_grid) {
    Z3_ast args[8];
    icoord_t current_position;
    bool bit_value;
    for(int bit = 7; bit >= 0; bit--) {
        current_position = decode_neighbor_bit(state, bit, position);
        bit_value = (state & (0x01 << bit)) > 0;
        // makes assumption that neighbor_state_on_board was called before this
        // so no need to check if the decoded bit is on the grid
        args[bit] = ast_grid[current_position.y][current_position.x];
        if(!bit_value) args[bit] = Z3_mk_not(ctx, args[bit]);
    }
    return Z3_mk_and(ctx, 9, args);
}

bool neighbor_state_on_board(coord_t size, coord_t position, uint8_t state) {
    for(size_t bit = 0; bit < 8; bit++) {
        icoord_t current_position = decode_neighbor_bit(state, bit, position);
        if(current_position.y < 0 || current_position.y >= size.y || current_position.x < 0 || current_position.x >= size.x) return false;
    }
    return true;
}

Z3_ast make_previous_constraints(Z3_context ctx, coord_t size, coord_t position, bool target_state, Z3_ast** ast_grid) {
    Z3_ast result, result_args[2], *temp_args, temp_static[2];
    size_t index;
    uint8_t state;
    arraylist_t combinations;
    
    // if the previous state is dead
    combinations = COMBINATIONS[target_state][0];
    temp_static[0] = Z3_mk_not(ctx, ast_grid[position.y][position.x]);
    temp_args = malloc(sizeof(Z3_ast) * combinations.count);
    index = 0;
    for(size_t i = 0; i < combinations.count; i++) {
        state = *(uint8_t*)combinations.arr[i];
        if(!neighbor_state_on_board(size, position, state)) continue;
        temp_args[index++] = get_neighbors_products(ctx, size, position, state, ast_grid);
    }
    temp_static[1] = Z3_mk_or(ctx, index, temp_args);
    result_args[0] = Z3_mk_and(ctx, 2, temp_static);

    // if the previous state is alive
    combinations = COMBINATIONS[target_state][1];
    temp_static[0] = ast_grid[position.y][position.x];
    temp_args = malloc(sizeof(Z3_ast) * combinations.count);
    index = 0;
    for(size_t i = 0; i < combinations.count; i++) {
        state = *(uint8_t*)combinations.arr[i];
        if(!neighbor_state_on_board(size, position, state)) continue;
        temp_args[index++] = get_neighbors_products(ctx, size, position, state, ast_grid);
    }
    temp_static[1] = Z3_mk_or(ctx, index, temp_args);
    result_args[1] = Z3_mk_and(ctx, 2, temp_static);

    // final sum of products
    result = Z3_mk_or(ctx, 2, result_args);
    return result;
}

parent_result_t find_parent_grid(grid_t g) {
    if(!COMBINATIONS_GENERATED) generate_combinations();

    Z3_config cfg = Z3_mk_config();
    Z3_context ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    Z3_solver solver = Z3_mk_solver(ctx);
    Z3_solver_inc_ref(ctx, solver);

    // create constraint graph for g here
    // 1. create variables for each cell
    // Allocate symbol grid
    Z3_ast **prev_alive_grid; //, **current_alive_grid;
    prev_alive_grid = malloc(sizeof(Z3_ast*) * g.size.y);
    // current_alive_grid = malloc(sizeof(Z3_ast*) * g.size.y); // TODO not needed, maybe
    handle_memory_error(prev_alive_grid);
    // handle_memory_error(current_alive_grid);
    for(size_t i = 0; i < g.size.y; i++) {
        prev_alive_grid[i] = malloc(sizeof(Z3_ast) * g.size.x);
        // current_alive_grid = malloc(sizeof(Z3_ast) * g.size.x);
        handle_memory_error(prev_alive_grid[i]);
        // handle_memory_error(current_alive_grid[i]);

        for(size_t j = 0; j < g.size.x; j++) {
            // create Z3 variables
            prev_alive_grid[i][j] = make_bool_variable(ctx, "prev_alive", i, j);
            // current_alive_grid[i][j] = make_bool_variable(ctx, "current_alive", i, j);
        }
    }

    // 2. Create constraints for previous grid
    Z3_ast current;
    coord_t position;
    for(size_t i = 0; i < g.size.y; i++) {
        position.y = i;
        for(size_t j = 0; j < g.size.x; j++) {
            position.x = j;
            current = make_previous_constraints(ctx, g.size, position, g.grid[i][j], prev_alive_grid);
            Z3_solver_assert(ctx, solver, current);
        }
    }

    // 3. Solve the grid
    Z3_lbool result = Z3_solver_check(ctx, solver);

    parent_result_t total_result;
    total_result.found = false;

    if(result == Z3_L_TRUE) {
        // Model found
        Z3_model model = Z3_solver_get_model(ctx, solver);
        Z3_model_inc_ref(ctx, model);

        total_result.result = create_grid(g.size);
        total_result.found = true;

        // 4. Extract the grid
        Z3_ast cell_value;
        Z3_lbool bval;
        for(size_t i = 0; i < g.size.y; i++) {
            for(size_t j = 0; j < g.size.x; j++) {
                current = prev_alive_grid[i][j];
                Z3_model_eval(ctx, model, current, Z3_TRUE, &cell_value);
                bval = Z3_get_bool_value(ctx, cell_value);
                total_result.result.grid[i][j] = bval == Z3_L_TRUE;
            }
        }

        Z3_model_dec_ref(ctx, model);
    }

    // De-allocate symbol grids
    for(size_t i = 0; i < g.size.y; i++) {
        free(prev_alive_grid[i]);
        // free(current_alive_grid[i]);
    }
    free(prev_alive_grid);
    // free(current_alive_grid);

    Z3_solver_dec_ref(ctx, solver);
    Z3_del_context(ctx);

    return total_result;
}