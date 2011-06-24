/* event.c : polling de eventos do sdl

   COMO FUNCIONA

   * Usamos a biblioteca SDL para gerenciar a interação com o teclado e
   * mouse.

   * O sdl envia um evento para cada interação (seja um movimento ou
   * clique do mouse, ou uma tecla apertada). A função event_handler
   * aqui lida com um único evento.

   * Eventos no geral modificam variáveis de estado (que são essas
   * variáveis globais aí embaixo). Elas são, no geral, usadas em gl.c
   * para alterar o funcionamento do mundo virtual em si

   * Teclas podem ser usadas em 2 modos: como interruptores (onde a
   * tecla liga e desliga uma flag) ou como uma interação contínua.

   * interruptores são lidados pela funcao toggle

   * interações contínuas são lidadas pela função physics (no caso do
   * movimento de seu personagem, etc) e pela função model (no caso de
   * interações que nao envolvem o personagem, como mudar a reflectancia
   * do material)

   * interações contínuas recebem como entrada o /\t, tomado como base
   * na ultima interacao feita. com um /\t baixo o suficiente, tem-se a
   * ilusao de continuidade

   * na funcao physics tb se move a luz (ja que o movimento dela eh
   * continuo)

   * o mouse eh preso na tela de uma forma meio tosca, que impede a
   * pessoa de sair com alt+tab. dai, usa-se F2 para desgrudar o mouse
   * (e um clique pra grudar denovo). usa-se esc pra sair do programa.

*/

#include <SDL/SDL.h>
#include <math.h>

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "types.h"
#include "event.h"

#define KM_H (1000.f / 3600.f)

float vel = 60 * KM_H;           /* velocidade linear, km/h */
float ang_vel = 0.2 ;            /* velocidade angular      */

float theta = 0;                 /* orientacao no plano xz  */
float phi = 0;                   /* orientacao no plano xy  */

vec3 mypos =                     /* posicao do personagem   */
    { .x = 0, .y = 0, .z = 0 };

float light[4] =                 /* posicao da luz 0        */
    { 0, 700, 0, 1 };
char stop_light = 0;             /* luz 0 esta parada?      */

float light_color[4] =           /* cor da luz 1            */
    { 0.5, 0.5, 0.5, 0 };

float shininess = 0;             /* brilho do material      */  
float diffuse[4]=                /* reflectancia difusa     */
    { 5, 5, 5, 1 };
float ambient[4]=
    { 1, 1, 1, 1 };
float specular[4]=               /* reflectancia especular  */
    { 0, 0, 0, 1 };

char key_pressed[512];           /* keymap continuo         */
char key_hit[512];               /* keymap toggle           */

char use_texture = 1;            /* usar modelo com textura */
char use_fog = 0;

char hide_text = 1;              /* esconder texto          */
char grab = 1;                   /* prender mouse na janela */
char show_grid = 1;
char show_map = 1;

char wireframe = 0;

char use_heightmap = 1;          /* usar mapa de altura     */
char use_sky = 1;                /* mostrar ceu             */

float minha_altura = 1.7;        /* em metros               */

float pos_map_x;
float pos_map_y;

char fps_str[8] = "0 FPS";       /* fps na tela             */
char status_str[3][256];         /* variaveis na tela       */

float map_pos_u;
float map_pos_v;
unsigned map_len_u;
unsigned map_len_v;

int altura_terreno;

unsigned screen_map_len = 200;

int iu, iv;

rgba* ruinas_map;

void update_map_pos()
{
    // z (blender x) 100m
    // x (blender y) 100m

    float ratio = (float) screen_map_len / 100;
    float half = screen_map_len / 2;

    map_pos_u = mypos.x * ratio + half;
    map_pos_v =-mypos.z * ratio + half;

    float mu = (float) map_len_u / screen_map_len;
    float mv = (float) map_len_v / screen_map_len;

    iu = map_pos_u * mu;
    iv = map_pos_v * mv;

    int i = (map_len_v - iv) * map_len_v + iu;

    altura_terreno = ruinas_map[i].r;
}

void init_event_keys()
{
    memset(key_pressed, 0, 512);
    memset(key_hit, 0, 512);
}


void update_status_str()
{
    snprintf(status_str[0], 256, "v%.2f h%.3f t %d u %.2f v "
             "%.2f iu %d iv %d p(% 9.3f,% 7.3f,% 9.3f) "
             "(% 6.1f,%5.1f)", vel, minha_altura,
             altura_terreno, map_pos_u, map_pos_v, iu, iv,
             mypos.x, mypos.y, mypos.z,
             theta, phi);


    snprintf(status_str[1], 256,
             "c%6.3f,%6.3f,%6.3f h%.0f s%.2f d%.2f t%d l%d,%d",
             light_color[0], light_color[1], light_color[2],
             shininess, specular[0], diffuse[0], use_texture,
             glIsEnabled(GL_LIGHT0), glIsEnabled(GL_LIGHT1));
}

void update_fps_str(int count)
{
    snprintf(fps_str, 8, "%d FPS", count);
    printf("fps: %d\n", count);
}

void event_handler(SDL_Event ev)
{
    if (ev.type == SDL_QUIT ||
        (ev.type == SDL_KEYDOWN &&
         (ev.key.keysym.sym == SDLK_ESCAPE))) {

        SDL_Quit();
        exit(0);
    }

    if (ev.type == SDL_MOUSEBUTTONDOWN &&
        ev.button.button == SDL_BUTTON_LEFT &&
        !grab) {
            SDL_WM_GrabInput(SDL_GRAB_ON);
            SDL_ShowCursor(SDL_DISABLE);
            grab = 1;
    }

    if (ev.type == SDL_MOUSEMOTION && grab) {
        theta -= ev.motion.xrel * ang_vel;
        phi -= ev.motion.yrel * ang_vel;

        float max_phi = 90;
        float min_phi = -90;

        if (phi > max_phi)
            phi = max_phi;
        else if (phi < min_phi)
            phi = min_phi;
    }
    else if (ev.type == SDL_KEYDOWN) {
        key_pressed[ev.key.keysym.sym] = 1;
        key_hit[ev.key.keysym.sym] = 1;
    }
    else if (ev.type == SDL_KEYUP) {
        key_pressed[ev.key.keysym.sym] = 0;
    };
}

#define LIDAR_COM_TECLA(tecla, acao) \
    {     if (key_hit[tecla]) { \
        key_hit[tecla] = 0;     \
        acao;                   \
        }                       \
    }

#define TECLA_TOGGLE(tecla, variavel) \
    LIDAR_COM_TECLA(tecla, { variavel = ! variavel; })

void toggle()
{
    TECLA_TOGGLE('p', stop_light);

    TECLA_TOGGLE('o', hide_text);
    TECLA_TOGGLE('4', use_sky);
    TECLA_TOGGLE('5', wireframe);
    TECLA_TOGGLE('6', show_map);
    TECLA_TOGGLE('7', show_grid);
    TECLA_TOGGLE('8', use_heightmap);
    TECLA_TOGGLE('9', use_fog);
    TECLA_TOGGLE('0', use_texture);

    LIDAR_COM_TECLA(SDLK_F2,
                    {
                        SDL_WM_GrabInput(SDL_GRAB_OFF);
                        SDL_ShowCursor(SDL_ENABLE);
                        grab = 0;
                    });

    LIDAR_COM_TECLA('j',
                    {
                        light_color[0] = 0.8;
                        light_color[1] = 1;
                        light_color[2] = 0.8;
                    });

    LIDAR_COM_TECLA('k',
                    {
                        light_color[0] = 1;
                        light_color[1] = 1;
                        light_color[2] = 1;
                    });

    LIDAR_COM_TECLA('-',
                    {
                        if (glIsEnabled(GL_LIGHT0))
                            glDisable(GL_LIGHT0);
                        else
                            glEnable(GL_LIGHT0);
                    });

    LIDAR_COM_TECLA('=',
                    {
                        if (glIsEnabled(GL_LIGHT1))
                            glDisable(GL_LIGHT1);
                        else
                            glEnable(GL_LIGHT1);
                    });
}

void model(float dt)
{
    float vel_vel = 1;

    float color_vel = 0.8;
    float param_vel = 100;
    float shin_vel = 50;

    float c = color_vel * dt;
    float p = param_vel * dt;

    if (key_pressed['z'])
        vel += vel_vel * dt;

    if (key_pressed['c'])
        vel -= vel_vel * dt;

    if (key_pressed['r'])
        light_color[0] += c;
    if (key_pressed['t'])
        light_color[0] -= c;
    if (key_pressed['f'])
        light_color[1] += c;
    if (key_pressed['g'])
        light_color[1] -= c;
    if (key_pressed['v'])
        light_color[2] += c;
    if (key_pressed['b'])
        light_color[2] -= c;


    if (key_pressed['y']) {
        diffuse[0] += p;
        diffuse[1] += p;
        diffuse[2] += p;
    }
    if (key_pressed['u']) {
        diffuse[0] -= p;
        if (diffuse[0] < 0)
            diffuse[0] = 0;

        diffuse[1] = diffuse[0];
        diffuse[2] = diffuse[0];
    }


    if (key_pressed['h']) {
        specular[0] += p;
        specular[1] += p;
        specular[2] += p;
    }
    if (key_pressed['j']) {
        specular[0] -= p;
        if (specular[0] < 0)
            specular[0] = 0;
        specular[1] = specular[0];
        specular[2] = specular[0];
    }

    if (key_pressed['n'])
        shininess += shin_vel * dt;
    if (key_pressed['m'])
        shininess -= shin_vel * dt;

    if (shininess > 128)
        shininess = 128;
    else if (shininess < 0)
        shininess = 0;
}

float rluz = 500;

void physics(float dt)
{
    float rad = M_PI / 180;

    float vel_sin = vel * sin(rad * theta) * dt;
    float vel_cos = vel * cos(rad * theta) * dt;

    if (key_pressed['q']) {
        minha_altura += vel * dt;
    }
    if (key_pressed['e']) {
        minha_altura -= vel * dt;
    }

    if (key_pressed['w']) {
        mypos.x -= vel_sin;
        mypos.z -= vel_cos;
    }
    if (key_pressed['s']) {
        mypos.x += vel_sin;
        mypos.z += vel_cos;
    }
    if (key_pressed['a']) {
        mypos.x -= vel_cos;
        mypos.z += vel_sin;
    }
    if (key_pressed['d']) {
        mypos.x += vel_cos;
        mypos.z -= vel_sin;
    }

    if (mypos.x >= 50)
        mypos.x = 50;
    if (mypos.x <= -50)
        mypos.x = -50;
    if (mypos.z >= 50)
        mypos.z = 50;
    if (mypos.z <= -50)
        mypos.z = -50;


    float ang_vel_light = 1000;

    static float theta_light = 0;
    static int dir = 0;

    if (!stop_light) {
        theta_light += (100+rluz) * dt;

        float cap = 10 + rluz/2;

        if (dir == 0) {
            rluz += cap * dt;
            if (rluz > 1000)
                dir = 1;
            if (rluz > 2000)
                rluz = 2000;
        }
        else if (dir == 1) {
            rluz -=  cap * dt;
            if (rluz < 5)
                dir = 0;
            if (rluz < 0)
                rluz = 0;
        }
    }

    light[0] = cos(rad * theta_light) * rluz;
}
