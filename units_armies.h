//This file contains the declarations of classes describing actors of the simulation
//Simple one-line functions are also implemented here
//
//

#ifndef ARMY_H
#define ARMY_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <iostream>


class battlefield; //Prototyping to avoid undeclared type issues
class army;


//Base unit class
class unit
{
protected:
    int max_hitpoints{}, hitpoints{}, pos_x{}, pos_y{};
    int attack_range{}, attack_die{}, mov_speed{ 1 };
    int unit_id{};
    char display{};
    bool dead{ false };
    bool stunned{ false };
    bool buffed{ false };
    //Parameters for detecting enemy units
    int closest_enemy_num{}, closest_x{}, closest_y{};
    int chosen_move{}, min_dist{};
    int new_distances[4] = { 0, 0, 0, 0 };
    int x_moves[2] = { mov_speed, -mov_speed };
    int y_moves[2] = { mov_speed, -mov_speed };
public:
    virtual ~unit() {};
    virtual char get_allegence() = 0;
    virtual void take_damage(int damage) = 0;
    virtual void attack(std::shared_ptr<unit>& target) = 0;
    virtual void take_action(const battlefield& field) = 0;
    virtual int return_current_number() = 0;
    //Functions shared by all derived classes
    void get_closest_unit_num(army& enemy_army);
    void choose_closest_move(const battlefield& field, std::shared_ptr<army> enemy_army);
    void choose_furthest_move(const battlefield& field, std::shared_ptr<army> enemy_army);
    bool is_dead() const { return dead; }
    bool is_buffed() const { return buffed; }
    bool is_stunned()const { return stunned; }
    char get_display()const { return display; }
    int get_posx() const { return pos_x; }
    int get_posy() const { return pos_y; }
    int get_id() { return unit_id; }
    int get_hitpoints() { return hitpoints; }
    int operator-(const std::shared_ptr<unit>& subtract);
    bool operator==(std::shared_ptr<unit>& compare) { return unit_id == compare->unit_id && display == compare->display; }
};

//Red units
class red_unit : public unit
{
public:
    char get_allegence() { return 'r'; }

};

class red_sw : public red_unit
{
private:
    static int number_of_red_sw, max_num_of_red_sw;
public:
    ~red_sw() { number_of_red_sw--; };
    red_sw(int in_pos_x, int in_pos_y);
    red_sw(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_red_sw; }
    friend class red_army;
};

class red_arch : public red_unit
{
private:
    static int number_of_red_arch, max_num_of_red_arch;
public:
    ~red_arch() { max_num_of_red_arch--; }
    red_arch(int in_pos_x, int in_pos_y);
    red_arch(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_red_arch; }
};

class red_knight : public red_unit
{
private:
    static int number_of_red_knight, max_num_of_red_knight;
public:
    ~red_knight() { max_num_of_red_knight--; }
    red_knight(int in_pos_x, int in_pos_y);
    red_knight(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_red_knight; }
};

//Blue units
class blue_unit : public unit
{
public:
    char get_allegence() { return 'b'; }
};

class blue_sw : public blue_unit
{
private:
    static int number_of_blue_sw;
    static int max_num_of_blue_sw;
public:
    ~blue_sw() { number_of_blue_sw--; }
    blue_sw(int in_pos_x, int in_pos_y);
    blue_sw(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_blue_sw; }
};

class blue_arch : public blue_unit
{
private:
    static int number_of_blue_arch;
    static int max_num_of_blue_arch;
public:
    ~blue_arch() { number_of_blue_arch--; }
    blue_arch(int in_pos_x, int in_pos_y);
    blue_arch(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_blue_arch; }
};

class blue_knight : public blue_unit
{
private:
    static int number_of_blue_knight;
    static int max_num_of_blue_knight;
public:
    ~blue_knight() { number_of_blue_knight--; }
    blue_knight(int in_pos_x, int in_pos_y);
    blue_knight(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    void take_damage(int damage);
    void attack(std::shared_ptr<unit>& target);
    void take_action(const battlefield& field);
    int return_current_number() { return number_of_blue_knight; }
};

//Battlefield tiles (framed as units)

class tile : public unit
{
public:
    char get_allegence() { return 't'; }
    void take_damage(int damage) {}
    void attack(std::shared_ptr<unit>& target) {}
    int return_current_number() { return 0; }
};

class caltrops : public tile
{
public:
    caltrops(int in_pos_x, int in_pos_y);
    caltrops(int in_pos_x, int in_pos_y, int in_hitpoints, bool in_buffed);
    ~caltrops() = default;
    void take_action(const battlefield& field);
    void fill_targets(std::vector<std::shared_ptr<unit>>& targets, std::shared_ptr<army> target_army);
};

class army
{
private:
    std::vector<std::shared_ptr<unit>> unit_list{};
    std::vector<std::shared_ptr<unit>>::iterator unit_it;
    int number_of_units{ 0 };
    std::string name{ "blank" };
public:
    army(std::string in_name) { name = in_name; }
    army() = default;
    ~army() { unit_list.clear(); }
    void add_unit(std::shared_ptr<unit>& unit_p);
    void erase_unit(std::shared_ptr<unit>& to_erase);
    std::shared_ptr<unit>& get_unit(int unit_no) { return unit_list.at(unit_no); }
    int get_unit_number() { return  number_of_units; }
    std::string get_name() { return name; }
    std::string get_unit_string();
    friend std::ostream& operator<<(std::ostream& out, std::shared_ptr<army> printed_army);
};

class battlefield
{
private:
    std::vector<std::shared_ptr<army>> armies{};
    std::shared_ptr<army> tileset{};
    std::vector<int> army_1_stats{};
    std::vector<int> army_2_stats{};
    int width{}, height{}, max_iterations{}, curr_iteration{ 0 }, tick_length{};
    int snapshot_rate{ 10 }; //Save snapshot every 10 iterations by default
    std::unique_ptr<char>field_data{};
    void deploy_army(std::shared_ptr<army> deployed_army);
    bool stop_sim{ false };
public:
    battlefield(int in_width, int in_height, std::shared_ptr<army> in_army1,
        std::shared_ptr<army> in_army2, std::shared_ptr<army> in_tileset, int in_iterations, int in_rate);
    ~battlefield() { armies.clear(); }
    void army_action(std::shared_ptr<army> acting_army, const battlefield& field);
    void update_field();
    void declare_winner();
    std::shared_ptr<army> get_red_army() const { return armies.at(0); }
    std::shared_ptr<army> get_blue_army() const { return armies.at(1); }
    int get_width() const { return width; }
    int get_height() const { return height; }
    void set_tick_length(int in_lenght) { tick_length = in_lenght; }
    bool is_stopped() { return stop_sim; }
    void save_snapshot();
    friend void run_tick(battlefield& field);
    friend std::ostream& operator<<(std::ostream& out, battlefield& field);
};


#endif
