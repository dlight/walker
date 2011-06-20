#ifndef EVENT_H
#define EVENT_H

#include <SDL/SDL.h>

extern float theta;
extern float phi;
extern vec3 mypos;
extern float light[4];
extern float light_color[4];
extern float shininess;
extern float diffuse[4];
extern float ambient[4];
extern float specular[4];
extern char hide_text;
extern char use_texture;
extern char use_fog;
extern char use_heightmap;

extern char show_grid;
extern char show_map;

extern char fps_str[8];
extern char status_str[3][256];

extern float minha_altura;

void init_event_keys();
void update_status_str();
void update_fps_str();
void event_handler(SDL_Event ev);
void toggle();
void model(float dt);
void physics(float dt);

#endif
