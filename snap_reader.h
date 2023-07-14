//This file contains the declarations of functions that load the simulation from a numbered snapshot
//
//
#ifndef SNAP_H
#define SNAP_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "units_armies.h"

//Namespace to more clearly differentiate the purpose of these functions
namespace snp {
    int read_army_units(std::map<char, std::vector<int>>& army_units, std::string unit_string);
    int read_config(std::map<std::string, int>& basic_params,
        std::map<std::string, std::string>& army_names, std::map<char, std::vector<int>>& army_1_units,
        std::map<char, std::vector<int>>& army_2_units, std::map<char, std::vector<int>>& tileset_tiles, int file_no);

    int populate_army(std::shared_ptr<army>& empty_army, std::string army_name, std::map<char, std::vector<int>>& army_units);
    int load_simulation(std::unique_ptr<battlefield>& field, int& tick_length, int& max_iterations, int file_no);
}

#endif
