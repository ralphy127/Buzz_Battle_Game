#ifndef UPDATE_H
#define UPDATE_H

void process_input(void);
void update(void);
void add_laser(void);
void update_lasers(float delta_time);
void spawn_insect(void);
void update_insects(float delta_time);
void update_insects_and_player_hit(void);
void spawn_mini_boss(void);
void update_mini_bosses(float delta_time);
void update_mini_bosses_lasers(float delta_time);
void update_mini_bosses_and_player_hit(void);

#endif