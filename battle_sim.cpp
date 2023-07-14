// battle_sim.cpp : This file runs the battle simulation.
//
//


#include <iostream>
#include <fstream>
#include <sstream>

#include <random>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>

#include <cmath>
#include <algorithm>
#include <string>
#include <map>
#include "utilities.h"
#include "units_armies.h"
#include "config.h"
#include "snap_reader.h"


//Prepare the static variables
int red_sw::number_of_red_sw{ 0 };
int red_sw::max_num_of_red_sw{ 0 };

int blue_sw::number_of_blue_sw{ 0 };
int blue_sw::max_num_of_blue_sw{ 0 };

int red_arch::number_of_red_arch{ 0 };
int red_arch::max_num_of_red_arch{ 0 };

int blue_arch::number_of_blue_arch{ 0 };
int blue_arch::max_num_of_blue_arch{ 0 };

int red_knight::number_of_red_knight{ 0 };
int red_knight::max_num_of_red_knight{ 0 };

int blue_knight::number_of_blue_knight{ 0 };
int blue_knight::max_num_of_blue_knight{ 0 };
///////////////////////////////////////////


int main()
{
    std::srand((unsigned int)std::time(0)); //Set random seed
    std::unique_ptr<battlefield> field{};
    char* end_num{};
    int loading_stat{};
    int tick_length{ 250 }, max_iterations{}, snapshot_num{};
    std::string choice{}, snap_no{};

    do {
        std::cout << "Enter c to load from config file, s to load a snapshot: ";
        std::cout << std::flush;
        std::getline(std::cin, choice);
    } while (choice != "s" && choice != "c");

    if (choice == "c") {
        loading_stat = cfg::load_simulation(field, tick_length, max_iterations);
        if (loading_stat != 0) {
            std::cout << "Fatal error when loading simulation, stopping" << std::endl;
            return -1;
        }
    }

    else if (choice == "s") {
        std::cout << "Enter snapshot number: ";
        std::getline(std::cin, snap_no);
        std::cout << std::flush;
        snapshot_num = std::strtol(snap_no.c_str(), &end_num, 10);
        while ((*end_num || !std::strtol(snap_no.c_str(), &end_num, 10)) && snap_no != "0") {
            std::cout << "Snapshot number must be a valid integer" << std::endl;;
            std::cout << "Enter snapshot number: ";
            std::getline(std::cin, snap_no);
            std::cout << std::flush;
            snapshot_num = std::strtol(snap_no.c_str(), &end_num, 10);
        }

        loading_stat = snp::load_simulation(field, tick_length, max_iterations, snapshot_num);
        if (loading_stat != 0) {
            std::cout << "Fatal error when loading simulation, stopping" << std::endl;
            return -1;
        }

    }


    field->set_tick_length(tick_length); //Manually provide tick_length
    std::cout << *field << std::endl;

    utl::enter_continue();
    utl::clear_screen();

    //Loop that runs the simulation
    while (!field->is_stopped()) {
        std::cout << *field << std::endl;
        run_tick(*field);
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_length));
        if (!field->is_stopped()) utl::clear_screen();
    }
    //Output the final state
    utl::clear_screen();
    run_tick(*field);
    std::cout << *field << std::endl;
    field->declare_winner();
}
