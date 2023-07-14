//This file contains implementations of classes specified in "units_armies.h"
//
//

#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <map>
#include <string>
#include <fstream>
#include "utilities.h"
#include "units_armies.h"

//Base unit class functions
int unit::operator-(const std::shared_ptr<unit>& subtract)
{
    //use subtraction operator to get Manhattan distance between units
    return std::abs(pos_x - subtract->pos_x) + std::abs(pos_y - subtract->pos_y);
}

//Finds closest enemy unit
void unit::get_closest_unit_num(army& enemy_army)
{
    int enemy_units{ enemy_army.get_unit_number() };
    std::vector<int> enemy_distances{};
    for (int i = 0; i < enemy_units; i++) {
        enemy_distances.push_back(*this - enemy_army.get_unit(i));
    }

    auto closest_dist = std::min_element(enemy_distances.begin(), enemy_distances.end());
    closest_enemy_num = (int)(closest_dist - enemy_distances.begin());
    min_dist = *this - enemy_army.get_unit(closest_enemy_num);
}

//This function moves the unit on the closest path towards enemy unit
void unit::choose_closest_move(const battlefield& field, std::shared_ptr<army> enemy_army)
{
    closest_x = enemy_army->get_unit(closest_enemy_num)->get_posx();
    closest_y = enemy_army->get_unit(closest_enemy_num)->get_posy();
    for (int i = 0; i < 2; i++) {
        if (pos_x + x_moves[i] < field.get_width() && pos_x + x_moves[i] >= 0) {
            new_distances[i] = utl::manhattan_dist(closest_x, closest_y, pos_x + x_moves[i], pos_y);
        }
        else new_distances[i] = field.get_height() + 1 + field.get_width();
    }
    for (int i = 0; i < 2; i++) {
        if (pos_y + y_moves[i] < field.get_height() && pos_y + y_moves[i] >= 0) {
            new_distances[i + 2] = utl::manhattan_dist(closest_x, closest_y, pos_x, pos_y + y_moves[i]);
        }
        else new_distances[i + 2] = field.get_height() + 1 + field.get_width();
    }
    auto min_new_dist = std::min_element(new_distances, new_distances + 4);
    //Use brute force as only 4 options available
    chosen_move = (int)(min_new_dist - new_distances);
    switch (chosen_move) {
    case (0):
        pos_x += x_moves[0];
        break;
    case (1):
        pos_x += x_moves[1];
        break;
    case (2):
        pos_y += y_moves[0];
        break;
    case (3):
        pos_y += y_moves[1];
        break;
    }
}

//Function for retreating the maximum available distance
void unit::choose_furthest_move(const battlefield& field, std::shared_ptr<army> enemy_army)
{
    closest_x = enemy_army->get_unit(closest_enemy_num)->get_posx();
    closest_y = enemy_army->get_unit(closest_enemy_num)->get_posy();
    for (int i = 0; i < 2; i++) {
        if (pos_x + x_moves[i] < field.get_width() && pos_x + x_moves[i] >= 0) {
            new_distances[i] = utl::manhattan_dist(closest_x, closest_y, pos_x + x_moves[i], pos_y);
        }
        else new_distances[i] = -1;
    }
    for (int i = 0; i < 2; i++) {
        if (pos_y + y_moves[i] < field.get_height() && pos_y + y_moves[i] >= 0) {
            new_distances[i + 2] = utl::manhattan_dist(closest_x, closest_y, pos_x, pos_y + y_moves[i]);
        }
        else new_distances[i + 2] = -1;
    }
    auto max_new_dist = std::max_element(new_distances, new_distances + 4);
    chosen_move = (int)(max_new_dist - new_distances);
    switch (chosen_move) {
    case (0):
        pos_x += x_moves[0];
        break;
    case (1):
        pos_x += x_moves[1];
        break;
    case (2):
        pos_y += y_moves[0];
        break;
    case (3):
        pos_y += y_moves[1];
        break;
    }
}

//Red swordsman unit functions
red_sw::red_sw(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_red_sw;
    max_hitpoints = 30;
    hitpoints = 30;
    attack_die = 12;
    attack_range = 1;
    display = 'S';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_sw++;
    max_num_of_red_sw++;
}

red_sw::red_sw(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_red_sw;
    max_hitpoints = 30;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    attack_die = 12;
    attack_range = 1;
    display = 'S';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_sw++;
    max_num_of_red_sw++;
}

void red_sw::attack(std::shared_ptr<unit>& target)
{
    int damage{ std::rand() % attack_die + 1 };
    if (buffed) damage += 5;
    target->take_damage(damage);
}

void red_sw::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.5 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 2) stunned = true;
    }
}

void red_sw::take_action(const battlefield& field)
{
    if (number_of_red_sw / max_num_of_red_sw <= 0.5 && !buffed) buffed = true;
    if (stunned && !dead) stunned = false; //Recover if stunned, do nothing else

    else if (!dead) {
        get_closest_unit_num(*field.get_blue_army());
        //Attack if in range
        if (min_dist <= attack_range) attack((field.get_blue_army())->get_unit(closest_enemy_num));

        //Move closer along the shortest path if not
        else choose_closest_move(field, field.get_blue_army());
    }
}

//Red archer unit functions
red_arch::red_arch(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_red_arch;
    max_hitpoints = 15;
    hitpoints = 15;
    attack_die = 8;
    attack_range = 4;
    mov_speed = 1;
    display = 'A';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_arch++;
    max_num_of_red_arch++;
}

red_arch::red_arch(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_red_arch;
    max_hitpoints = 15;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    attack_die = 8;
    attack_range = 4;
    mov_speed = 1;
    display = 'A';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_arch++;
    max_num_of_red_arch++;
}

void red_arch::take_action(const battlefield& field)
{
    if (!dead && !stunned) {
        //Increase attack range if strength dropping
        if (number_of_red_arch <= 0.5 * max_num_of_red_arch && !buffed) {
            attack_range += 2;
            attack_die += 1;
            buffed = true;
        }
        get_closest_unit_num(*field.get_blue_army());
        //Attack if in range, retreat if too close (min_dist <= 1)
        if (min_dist <= attack_range && min_dist > 1) attack(field.get_blue_army()->get_unit(closest_enemy_num));
        else if (min_dist > 1) choose_closest_move(field, field.get_blue_army());
        else choose_furthest_move(field, field.get_blue_army());
    }
    else if (stunned) stunned = false;
}

void red_arch::attack(std::shared_ptr<unit>& target)
{
    int damage{std::rand()%attack_die+1};
    target->take_damage(damage);
}

void red_arch::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.3 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 3) stunned = true;
    }
}

//Red knight implementation
red_knight::red_knight(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_red_knight;
    max_hitpoints = 20;
    hitpoints = 20;
    attack_die = 10;
    attack_range = 2;
    mov_speed = 1;
    display = 'K';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_knight++;
    max_num_of_red_knight++;
}

red_knight::red_knight(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_red_knight;
    max_hitpoints = 20;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    attack_die = 10;
    attack_range = 2;
    mov_speed = 1;
    display = 'K';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_red_knight++;
    max_num_of_red_knight++;
}

void red_knight::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.5 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 1) stunned = true;
    }
}

void red_knight::attack(std::shared_ptr<unit>& target)
{
    int damage{ std::rand() % attack_die + 1 };
    if (buffed) damage += std::rand() % 6 + 1;
    target->take_damage(damage);
}

void red_knight::take_action(const battlefield& field)
{
    if (!dead && !stunned) {
        //Buff to increase attack damage if strength dropping
        if (number_of_red_knight <= 0.5 * max_num_of_red_knight) buffed = true;

        get_closest_unit_num(*field.get_blue_army());
        //Attack if in range, move closer if not
        if (min_dist <= attack_range && min_dist > 1) {
            attack(field.get_blue_army()->get_unit(closest_enemy_num));
        }
        else choose_closest_move(field, field.get_blue_army());
    }
    else if (stunned) stunned = false;
}

//Blue swordsman implementation
blue_sw::blue_sw(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_blue_sw;
    max_hitpoints = 20;
    attack_die = 10;
    attack_range = 1;
    mov_speed = 1;
    hitpoints = 20;
    display = 's';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_sw++;
    max_num_of_blue_sw++;
}

blue_sw::blue_sw(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_blue_sw;
    max_hitpoints = 20;
    attack_die = 10;
    attack_range = 1;
    mov_speed = 1;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    display = 's';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_sw++;
    max_num_of_blue_sw++;
}

void blue_sw::attack(std::shared_ptr<unit>& target)
{
    int damage{ std::rand() % attack_die + 1 };
    target->take_damage(damage);
}

void blue_sw::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.5 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 2) stunned = true;
    }
}

void blue_sw::take_action(const battlefield& field)
{

    if (number_of_blue_sw / max_num_of_blue_sw <= 0.5 && !buffed) {
        hitpoints += 5;
        attack_die += 2;
        buffed = true;
    }

    if (stunned && !dead) stunned = false; //Recover if stunned, do nothing else

    else if (!dead) {
        get_closest_unit_num(*field.get_red_army());

        if (min_dist <= attack_range) attack((field.get_red_army())->get_unit(closest_enemy_num));//Attack if in range

        //Move closer along the shortest path if not, find shortest path through brute force as only 4 options exist
        else choose_closest_move(field, field.get_red_army());
    }
}

//Blue archer implementation
blue_arch::blue_arch(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_blue_arch;
    max_hitpoints = 15;
    attack_die = 8;
    attack_range = 4;
    mov_speed = 1;
    hitpoints = 15;
    display = 'a';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_arch++;
    max_num_of_blue_arch++;
}

blue_arch::blue_arch(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_blue_arch;
    max_hitpoints = 15;
    attack_die = 8;
    attack_range = 4;
    mov_speed = 1;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    display = 'a';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_arch++;
    max_num_of_blue_arch++;
}

void blue_arch::attack(std::shared_ptr<unit>& target)
{
    int damage{ std::rand() % attack_die + 1 };
    target->take_damage(damage);
}

void blue_arch::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.3 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 3) stunned = true;
    }
}

void blue_arch::take_action(const battlefield& field)
{
    if (!dead && !stunned) {
        //Increase attack range if strength dropping
        if (number_of_blue_arch <= 0.5 * max_num_of_blue_arch && !buffed) {
            hitpoints += 8;
            attack_range++;
            buffed = true;
        }
        get_closest_unit_num(*field.get_red_army());
        //Attack if in range, retreat if too close (min_dist <= 1)
        if (min_dist <= attack_range && min_dist > 1) attack(field.get_red_army()->get_unit(closest_enemy_num));
        else if (min_dist > 1) choose_closest_move(field, field.get_red_army());
        else choose_furthest_move(field, field.get_red_army());
    }
    else if (stunned) stunned = false;
}

//Blue knight implementation
blue_knight::blue_knight(int in_pos_x, int in_pos_y)
{
    unit_id = number_of_blue_knight;
    max_hitpoints = 20;
    hitpoints = 20;
    attack_die = 10;
    attack_range = 2;
    mov_speed = 1;
    display = 'k';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_knight++;
    max_num_of_blue_knight++;
}

blue_knight::blue_knight(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = number_of_blue_knight;
    max_hitpoints = 20;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    attack_die = 10;
    attack_range = 2;
    mov_speed = 1;
    display = 'k';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
    number_of_blue_knight++;
    max_num_of_blue_knight++;
}

void blue_knight::take_damage(int damage)
{
    hitpoints -= damage;
    if (hitpoints <= 0) dead = true;
    else if (damage >= 0.5 * max_hitpoints) {
        if (std::rand() % 6 + 1 <= 1) stunned = true;
    }
}

void blue_knight::attack(std::shared_ptr<unit>& target)
{
    int damage{ std::rand() % attack_die + 1 };
    target->take_damage(damage);
}

void blue_knight::take_action(const battlefield& field)
{
    if (!dead && !stunned) {
        //Buff to increase attack damage if strength dropping
        if (number_of_blue_knight <= 0.5 * max_num_of_blue_knight && !buffed) {
            buffed = true;
            hitpoints += 8;
        }
        get_closest_unit_num(*field.get_red_army());
        //Attack if in range, move closer if not
        if (min_dist <= attack_range && min_dist > 1) attack(field.get_red_army()->get_unit(closest_enemy_num));
        else choose_closest_move(field, field.get_red_army());
    }
    else if (stunned) stunned = false;
}

//Battlefield tiles implementations
caltrops::caltrops(int in_pos_x, int in_pos_y)
{
    unit_id = 0;
    max_hitpoints = 99;
    hitpoints = 99;
    attack_die = 4;
    attack_range = 0;
    mov_speed = 0;
    display = '+';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
}

//Functions for the caltrops tile
caltrops::caltrops(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed)
{
    unit_id = 0;
    max_hitpoints = 99;
    hitpoints = in_hitpoints;
    buffed = in_buffed;
    attack_die = 4;
    attack_range = 0;
    mov_speed = 0;
    display = '+';
    pos_x = in_pos_x;
    pos_y = in_pos_y;
}


void caltrops::fill_targets(std::vector<std::shared_ptr<unit>>& targets, std::shared_ptr<army> target_army)
{
    for (int i = 0; i < target_army->get_unit_number(); i++) {
        if (*this - target_army->get_unit(i) == 0) {
            targets.push_back(target_army->get_unit(i));
        }

    }
}

void caltrops::take_action(const battlefield& field)
{
    std::vector<std::shared_ptr<unit>> target_list{};
    fill_targets(target_list, field.get_blue_army());
    fill_targets(target_list, field.get_red_army());
    //Simply damage anyone standing in the same spot
    for (auto i = target_list.begin(); i < target_list.end(); i++) {
        (*i)->take_damage(std::rand() % attack_die + 1);
    }
}

//Army class implementations
void army::add_unit(std::shared_ptr<unit>& unit_p)
{
    unit_list.push_back(unit_p);
    number_of_units = (int)unit_list.size();
}

std::ostream& operator<<(std::ostream& out, std::shared_ptr<army> printed_army)
{
    std::map<char, int> army_stats{};//Print generic army using a map
    for (int i = 0; i < printed_army->get_unit_number(); i++) {
        if (army_stats.find(printed_army->get_unit(i)->get_display()) == army_stats.end()) {
            army_stats.insert(std::pair<char, int>(printed_army->get_unit(i)->get_display(), 1));
        }
        else {
            army_stats[printed_army->get_unit(i)->get_display()]++;
        }
    }
    out << printed_army->name;
    for (auto pair : army_stats) {
        out << ": " << pair.first << ": " << pair.second << " ";
    }
    return out;
}

void army::erase_unit(std::shared_ptr<unit>& to_erase)
{
    auto target_p = std::find(unit_list.begin(), unit_list.end(), to_erase);
    unit_list.erase(target_p);
    number_of_units--;
}

//Function to generate config file strings to save army configuration
std::string army::get_unit_string()
{
    std::string unit_string{ "" };
    for (unit_it = unit_list.begin(); unit_it < unit_list.end(); unit_it++) {
        unit_string += (*unit_it)->get_display();
        unit_string += ",";
        unit_string += std::to_string((*unit_it)->get_posx());
        unit_string += ",";
        unit_string += std::to_string((*unit_it)->get_posy());
        unit_string += ",";
        unit_string += std::to_string((*unit_it)->get_hitpoints());
        unit_string += ",";
        unit_string += std::to_string((*unit_it)->is_buffed());
        unit_string += "&";
    }
    return unit_string;
}


//Battlefield class implementations
std::ostream& operator<<(std::ostream& out, battlefield& field)
{
    for (int i = 0; i < field.height; i++) {
        for (int j = 0; j < field.width; j++) {
            out <<std::setw(2)<< field.field_data.get()[j + i * field.width]; //setw to make the output nicer
        }
        out << '\n';
    }
    out << field.armies[0] << '\n';
    out << field.armies[1] << '\n';
    return out;
}

void battlefield::army_action(std::shared_ptr<army> acting_army, const battlefield& field)
{
    for (int i = 0; i < acting_army->get_unit_number(); i++) {
        (acting_army->get_unit(i))->take_action(field);
    }
}

void battlefield::deploy_army(std::shared_ptr<army> deployed_army)
{
    int unit_posx{}, unit_posy{};
    for (int i = 0; i < deployed_army->get_unit_number(); i++) {
        if ((deployed_army->get_unit(i))->is_dead()) {
            if (deployed_army->get_unit(i)->get_allegence() == 'r') {
                army_1_stats.at(0)++;
                army_2_stats.at(1)++;
            }
            else {
                army_2_stats.at(0)++;
                army_1_stats.at(1)++;
            }
            deployed_army->erase_unit(deployed_army->get_unit(i));
        }
    }
    for (int i = 0; i < deployed_army->get_unit_number(); i++) {
        unit_posx = (deployed_army->get_unit(i))->get_posx();
        unit_posy = (deployed_army->get_unit(i))->get_posy();
        if (unit_posx > width || unit_posy > height) {
            throw(std::invalid_argument("Attempting to place unit outside the battlefield"));
        }
        else field_data.get()[unit_posx + width * unit_posy] = (deployed_army->get_unit(i))->get_display();
    }
    if (deployed_army->get_unit_number() == 0 && deployed_army->get_name()!="tileset") stop_sim = true;
}

battlefield::battlefield(int in_width, int in_height, std::shared_ptr<army> in_army1, std::shared_ptr<army> in_army2, std::shared_ptr<army> in_tileset,
    int in_iterations, int in_rate)
{
    width = in_width;
    height = in_height;
    max_iterations = in_iterations;
    snapshot_rate = in_rate;
    tileset = in_tileset;
    armies.push_back(in_army1);
    armies.push_back(in_army2);
    field_data.reset(new char[width * height]);
    for (int i = 0; i < width * height; i++) { field_data.get()[i] = '*'; }
    deploy_army(tileset);
    for (size_t i = 0; i < armies.size(); i++) {
        deploy_army(armies.at(i));
    }
    army_1_stats.push_back(0);
    army_1_stats.push_back(0);
    army_2_stats.push_back(0);
    army_2_stats.push_back(0);
}

void battlefield::update_field()
{
    for (int i = 0; i < width * height; i++) { field_data.get()[i] = '*'; }
    deploy_army(tileset);
    for (size_t i = 0; i < armies.size(); i++) {
        deploy_army(armies.at(i));
    }
}

void battlefield::declare_winner()
{
    if (armies.at(0)->get_unit_number() != 0 && armies.at(1)->get_unit_number() == 0) {
        std::cout << armies.at(0)->get_name()<<" wins" << std::endl;
    }
    else if (armies.at(0)->get_unit_number() == 0 && armies.at(1)->get_unit_number() != 0) {
        std::cout << armies.at(1)->get_name() << " wins" << std::endl;
    }
    else std::cout << "Draw" << std::endl;
    std::cout << armies.at(0)->get_name()<<" Kills: "<<army_1_stats[1]<<" Losses: "<<army_1_stats[0]<<std::endl;
    std::cout << armies.at(1)->get_name() << " Kills: " << army_2_stats[1] << " Losses: " << army_2_stats[0] << std::endl;
}

void battlefield::save_snapshot()
{
    std::string file_name{"snapshot_"+std::to_string(curr_iteration/snapshot_rate)};
    std::ofstream snapshot_file{file_name};
    snapshot_file << "Snapshot number "<< curr_iteration / snapshot_rate <<
        ", lines starting with # are used to partially reconstruct the simulation:" << std::endl;
    snapshot_file << *this << std::endl;
    snapshot_file << armies.at(0)->get_name() << " Kills: " 
        << army_1_stats[1] << " Losses: " << army_1_stats[0] << std::endl;
    snapshot_file << armies.at(1)->get_name() << " Kills: " 
        << army_2_stats[1] << " Losses: " << army_2_stats[0] << std::endl;

    snapshot_file << std::endl;
    snapshot_file << "#tick_length = " << tick_length << std::endl;
    snapshot_file << "#max_iterations = " << max_iterations << std::endl;
    snapshot_file << "#snapshot_rate = " << snapshot_rate << std::endl;
    snapshot_file << "#width = " << width << std::endl;
    snapshot_file << "#height = " << height << std::endl;
    snapshot_file << "#tileset = " << tileset->get_unit_string() << std::endl;

    snapshot_file << "#army_1_name = " << armies.at(0)->get_name() << std::endl;
    snapshot_file << "#army_1_units = " << armies.at(0)->get_unit_string() << std::endl;
    //snapshot_file << "#army_1_stats = " << army_1_stats.at(0)<<"," << army_1_stats.at(1) << std::endl;
    
    snapshot_file << "#army_2_name = " << armies.at(1)->get_name() << std::endl;
    snapshot_file << "#army_2_units = " << armies.at(1)->get_unit_string() << std::endl;
    //snapshot_file << "#army_2_stats = " << army_2_stats.at(0) << "," << army_2_stats.at(1) << std::endl;
}


//Non-member function implementation
void run_tick(battlefield& field)
{
    if (!field.stop_sim) {
        if (field.curr_iteration % field.snapshot_rate == 0) field.save_snapshot();
        field.army_action(field.tileset, field); //Environmental damage resolves first
        field.army_action(field.armies[0], field); //Reds move after
        field.army_action(field.armies[1], field);
        field.update_field(); //Redeploy armies once all actions resolve
        field.curr_iteration++;
    }
    if (field.curr_iteration > field.max_iterations) field.stop_sim = true;
}


