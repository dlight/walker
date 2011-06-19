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

float vel = 250;                 /* velocidade linear       */
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
    { 15, 15, 15, 1 };
float ambient[4]=
    { 30, 30, 30, 1 };
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

char use_heightmap = 0;          /* usar mapa de altura */

char fps_str[8] = "0 FPS";       /* fps na tela             */
char status_str[3][256];         /* variaveis na tela       */

void init_event_keys()
{
    memset(key_pressed, 0, 512);
    memset(key_hit, 0, 512);
}


void update_status_str()
{
    snprintf(status_str[0], 256, "v%f p(% 9.3f,% 7.3f,% 9.3f) "
             "a(% 6.1f,%5.1f)",
             vel, mypos.x, mypos.y, mypos.z,
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

void toggle()
{
    if (key_hit[SDLK_F2]) {
        key_hit[SDLK_F2] = 0;
            SDL_WM_GrabInput(SDL_GRAB_OFF);
            SDL_ShowCursor(SDL_ENABLE);
            grab = 0;
    }

    if (key_hit['p']) {
        key_hit['p'] = 0;
        stop_light = ! stop_light;
    }
    if (key_hit['o']) {
        key_hit['o'] = 0;
        hide_text = ! hide_text;
    }

    if (key_hit['j']) {
        key_hit['j'] = 0;
        light_color[0] = 0.8;
        light_color[1] = 1;
        light_color[2] = 0.8;
    }
    if (key_hit['k']) {
        key_hit['k'] = 0;
        light_color[0] = 1;
        light_color[1] = 1;
        light_color[2] = 1;
    }

    if (key_hit['6']) {
        key_hit['6'] = 0;
        show_map = !show_map;
    }

    if (key_hit['7']) {
        key_hit['7'] = 0;
        show_grid = !show_grid;
    }

    if (key_hit['8']) {
        key_hit['8'] = 0;
        use_heightmap = !use_heightmap;
    }

    if (key_hit['9']) {
        key_hit['9'] = 0;
        use_fog = !use_fog;
    }

    if (key_hit['0']) {
        key_hit['0'] = 0;
        use_texture = !use_texture;
    }

    if (key_hit['-']) {
        key_hit['-'] = 0;
        if (glIsEnabled(GL_LIGHT0))
            glDisable(GL_LIGHT0);
        else
            glEnable(GL_LIGHT0);
    }
    if (key_hit['=']) {
        key_hit['='] = 0;
        if (glIsEnabled(GL_LIGHT1))
            glDisable(GL_LIGHT1);
        else
            glEnable(GL_LIGHT1);
    }
}

void model(float dt)
{
    float vel_vel = 500;

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
        mypos.y += vel * dt;
    }
    if (key_pressed['e']) {
        mypos.y -= vel * dt;
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