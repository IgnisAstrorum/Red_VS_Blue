//Simulation loader implementations
//
//


#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "snap_reader.h"
#include "units_armies.h"
#include "utilities.h"

//Read units to a given army
int snp::read_army_units(std::map<char, std::vector<int>>& army_units, std::string unit_string)
{
    std::istringstream string_to_read{ unit_string }, info_to_read{};
    std::string unit_info{}, temp_str{};
    std::vector<int> unit_coords{};
    char unit_display{};
    int unit_x{}, unit_y{}, unit_hp{}, unit_buff{};
    char* end_of_info{};

    while (std::getline(string_to_read, unit_info, '&')) {
        info_to_read.str(unit_info);
        std::getline(info_to_read, temp_str, ',');
        unit_display = temp_str[0];

        std::getline(info_to_read, temp_str, ',');
        unit_x = (int)std::strtol(temp_str.c_str(), &end_of_info, 10);
        if (*end_of_info) {
            std::cout << "Error, invalid unit coordinate encountered" << std::endl;
            return -1;
        }
        else if (unit_x < 0) {
            std::cout << "Error, unit coordinates can't be negative" << std::endl;
            return -1;
        }

        std::getline(info_to_read, temp_str, ',');
        unit_y = (int)std::strtol(temp_str.c_str(), &end_of_info, 10);
        if (*end_of_info) {
            std::cout << "Error, invalid unit coordinate encountered" << std::endl;
            return -1;
        }
        else if (unit_y < 0) {
            std::cout << "Error, unit coordinates can't be negative" << std::endl;
            return -1;
        }

        std::getline(info_to_read, temp_str, ',');
        unit_hp = (int)std::strtol(temp_str.c_str(), &end_of_info, 10);
        if (*end_of_info) {
            std::cout << "Error, invalid unit hitpoints value encountered" << std::endl;
            return -1;
        }
        else if (unit_hp < 0) {
            std::cout << "Error, unit hitpoints can't be negative" << std::endl;
            return -1;
        }

        std::getline(info_to_read, temp_str);
        unit_buff = (int)std::strtol(temp_str.c_str(), &end_of_info, 10);
        if (*end_of_info) {
            std::cout << "Error, invalid unit buff value encountered" << std::endl;
            return -1;
        }
        else if (unit_hp < 0) {
            std::cout << "Error, unit buff can't be negative" << std::endl;
            return -1;
        }

        if (army_units.find(unit_display) == army_units.end()) {
            unit_coords.push_back(unit_x);
            unit_coords.push_back(unit_y);
            unit_coords.push_back(unit_hp);
            unit_coords.push_back(unit_buff);
            army_units.insert(std::make_pair(unit_display, unit_coords));
        }
        else {
            army_units[unit_display].push_back(unit_x);
            army_units[unit_display].push_back(unit_y);
            army_units[unit_display].push_back(unit_hp);
            army_units[unit_display].push_back(unit_buff);
        }

        info_to_read.clear();
        unit_coords.clear();
    }
    return 0;
}

//Function that reads the configuration file
int snp::read_config(std::map<std::string, int>& basic_params,
    std::map<std::string, std::string>& army_names, std::map<char, std::vector<int>>& army_1_units,
    std::map<char, std::vector<int>>& army_2_units, std::map<char, std::vector<int>>& tileset_tiles, int file_no)
{
    std::ifstream snap_file{ "snapshot_"+std::to_string(file_no)};
    std::string line{}, key{}, value{};
    int line_number{ 0 }, numeric_value{}, load_status{};
    std::string::iterator delim_loc{};
    char* end_of_numeric{};

    if (snap_file.fail()) {
        std::cout << "Error, snapshot file "<< 
            "snapshot_" + std::to_string(file_no) <<" unreadable" << std::endl;
        return -1;
    }

    //Extract keys and values to use for the simulation
    while (std::getline(snap_file, line)) {
        line_number++;
        if (line != "") utl::strip_whitespaces(line);
        if (line[0] == '#' && line != "") {
            line = line.substr(1, std::string::npos); //Strip the leading # from the snapshot file lines
            delim_loc = std::find(line.begin(), line.end(), '=');
            if (delim_loc == line.end()) {
                std::cout << "Error, line " << line_number << " is incorrectly formatted" << std::endl;
                return -1;
            }

            key = line.substr(0, (size_t)(delim_loc - line.begin()));
            value = line.substr((size_t)(delim_loc - line.begin() + 1), std::string::npos);
            numeric_value = (int)std::strtol(value.c_str(), &end_of_numeric, 10);
            if (*end_of_numeric || value == "") {
                if (key == "army_1_name" || key == "army_2_name") {
                    if (value == "tileset") {
                        std::cout << "Error, name 'tileset' is reserved" << std::endl;
                        return -1;
                    }
                    army_names.insert(std::make_pair(key, value));
                }
                //Unpack unit strings, if present
                else if (key == "army_1_units") {
                    load_status = snp::read_army_units(army_1_units, value);
                    if (load_status != 0) return -1;
                }
                else if (key == "army_2_units") {
                    load_status = snp::read_army_units(army_2_units, value);
                    if (load_status != 0) return -1;
                }
                else if (key == "tileset") {
                    load_status = snp::read_army_units(tileset_tiles, value);
                    if (load_status != 0) return -1;
                    army_names.insert(std::make_pair("tileset", "tileset"));
                }
                else {
                    std::cout << "Error, invalid value in line " << line_number << std::endl;
                    return -1;
                }
            }
            else if (numeric_value >= 0) basic_params.insert(std::make_pair(key, numeric_value));
            else {
                std::cout << "Error, negative value encountered in line " << line_number << std::endl;
                return -1;
            }
        }
    }
    return 0;
}


int snp::populate_army(std::shared_ptr<army>& empty_army, std::string army_name, std::map<char, std::vector<int>>& army_units)
{
    std::vector<int> unit_coords{};
    std::vector<int>::iterator coord_iter;
    std::shared_ptr<unit> empty_unit{};
    empty_army.reset(new army(army_name));
    for (auto pair : army_units) {
        unit_coords = pair.second;
        //Switch block to decide what unit to add
        switch (pair.first) {
        case('S'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new red_sw(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('A'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new red_arch(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('K'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new red_knight(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('s'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new blue_sw(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('a'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new blue_arch(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('k'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new blue_knight(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        case('+'):
            for (coord_iter = unit_coords.begin(); coord_iter < unit_coords.end(); coord_iter += 4) {
                empty_unit.reset(new caltrops(*coord_iter, *(coord_iter + 1),
                    *(coord_iter + 2), *(coord_iter + 3)));
                empty_army->add_unit(empty_unit);
            }
            break;
        default:
            std::cout << "Unrecognized unit type " << pair.first << std::endl;
            return -1;
        }
    }
    return 0;
}

int snp::load_simulation(std::unique_ptr<battlefield>& field, int& tick_length, int& max_iterations, int file_no)
{
    std::map<std::string, int> basic_params;
    std::map<std::string, std::string> army_names;
    std::map<char, std::vector<int>> army_1_units, army_2_units, tileset_tiles;
    std::shared_ptr<army> army_1{};
    std::shared_ptr<army> army_2{};
    std::shared_ptr<army> tileset{};
    int read_status{}, width{}, height{}, snapshot_rate{};

    read_status = snp::read_config(basic_params, army_names, army_1_units, army_2_units, tileset_tiles, file_no);
    if (read_status != 0) {
        std::cout << "Error reading snapshot" << std::endl;
        return -1;
    }
    //Read basic parameters, outputting error messages or using default values if they're missing
    if (basic_params.find("tick_length") == basic_params.end()) {
        tick_length = 250;
    }
    else tick_length = basic_params["tick_length"];

    if (basic_params.find("max_iterations") == basic_params.end()) {
        max_iterations = 100;
    }
    else max_iterations = basic_params["max_iterations"];

    if (basic_params.find("snapshot_rate") == basic_params.end()) {
        snapshot_rate = 10;
    }
    else if (basic_params["snapshot_rate"] == 0) {
        std::cout << "Error, snapshot_rate can't be 0" << std::endl;
        return -1;
    }
    else snapshot_rate = basic_params["snapshot_rate"];

    if (basic_params.find("width") == basic_params.end()) {
        std::cout << "Error, width missing from configuration" << std::endl;
        return -1;
    }
    width = basic_params["width"];

    if (basic_params.find("height") == basic_params.end()) {
        std::cout << "Error, height missing from configuration" << std::endl;
        return -1;
    }
    height = basic_params["height"];

    //Populate armies
    if (army_names.find("army_1_name") == army_names.end()) {
        std::cout << "Error, army_1_name missing from configuration" << std::endl;
        return -1;
    }
    read_status = snp::populate_army(army_1, army_names["army_1_name"], army_1_units);
    if (read_status != 0) {
        std::cout << "Error loading army 1" << std::endl;
        return -1;
    }

    if (army_names.find("army_2_name") == army_names.end()) {
        std::cout << "Error, army_2_name missing from configuration" << std::endl;
        return -1;
    }
    read_status = snp::populate_army(army_2, army_names["army_2_name"], army_2_units);
    if (read_status != 0) {
        std::cout << "Error loading army 2" << std::endl;
        return -1;
    }
    //Insert tileset
    if (army_names.find("tileset") == army_names.end()) {
        std::cout << "Error, missing tileset specification" << std::endl;
        return -1;
    }
    read_status = snp::populate_army(tileset, "tileset", tileset_tiles);
    if (read_status != 0) {
        std::cout << "Error loading tileset" << std::endl;
        return -1;
    }

    //Create battlefield
    try {
        field.reset(new battlefield(width, height, army_1, army_2, tileset, max_iterations, snapshot_rate));
    }
    catch (std::invalid_argument) {
        std::cout << "Unit out of bounds" << std::endl;
        return -1;
    }
    return 0;
}


