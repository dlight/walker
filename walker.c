#define GL_GLEXT_PROTOTYPES

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "param.h"
#include "nanosec.h"

#include <SDL/SDL.h>

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "mesh/terrain.h"
#include "mesh/terrain2.h"
#include "mesh/terrain3.h"
#include "mesh/paredes.h"
#include "mesh/terrenoecasa.h"
#include "mesh/terrenoecasa2.h"
#include "mesh/casa_montanha.h"
#include "mesh/casa_montanha_rapido.h"
#include "mesh/com_textura.h"

void (*desenhar_terreno)(void) = casa_montanha_rapidoDraw;

void cubep(float posx, float posy, float posz)
{
    glPushMatrix();
    glTranslatef(posx, posy, posz);
    glutSolidCube (1.0);
    glPopMatrix();
}

void layer(float posz)
{
    cubep(2, 2, posz);
    cubep(2, 0, posz);

    int i;
    for (i = 0; i < 30; i++) {
        cubep(2*i, 0, posz);
        cubep(2*i, 2, posz);
        cubep(-2*i, 0, posz);
        cubep(-2*i, 2, posz);
    }
}



void draw_status()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, res_x, 0, res_y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1, 0.4, 0);
    glRasterPos2i(10, 10);

    for (int i = 0; fps_str[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, fps_str[i]);

    glRasterPos2i(10, res_y - 20);

    for (int i = 0; status_str[0][i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, status_str[0][i]);

    glRasterPos2i(10, res_y - 20 - 14);

    for (int i = 0; status_str[1][i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, status_str[1][i]);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void setup_projection()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45,
                   (GLfloat) res_x /
                   (GLfloat) res_y, 0.1, 10000);
    glMatrixMode (GL_MODELVIEW);
}

void draw_light_point()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    glVertex3f(light[0], light[1], light[2]);
    glEnd();
    glEnable(GL_LIGHTING);
}

void draw()
{
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_color);

    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    setup_projection();
    glLoadIdentity ();  

    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);

    glTranslatef(-mypos.x, -mypos.y, -mypos.z);

    float luz1[4] = { mypos.x, mypos.y+50, mypos.z, 1 };

    glLightfv(GL_LIGHT0, GL_POSITION, light);
    glLightfv(GL_LIGHT1, GL_POSITION, luz1);

    draw_light_point();

    layer(0);

    desenhar_terreno();

    if (!hide_text)
        draw_status();

    SDL_GL_SwapBuffers();
}
void initgl()
{
    glClearColor (0.0, 0.0, 0.2, 0.0);

    glViewport (0, 0, res_x, res_y); 

    glShadeModel(GL_SMOOTH);

    glPointSize(1.5);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    float ambient[]={ 0.005, 0.005, 0.005, 0 };

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001);
    glEnable(GL_LIGHT0);

    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05);

    float luz0[]={ 1, 2, 3, 1 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0);

    //glEnable(GL_LIGHT1);

    glEnable(GL_LIGHTING);
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

    if (key_hit['1']) {
        key_hit['1'] = 0;
        desenhar_terreno = terrainDraw;
    }
    if (key_hit['2']) {
        key_hit['2'] = 0;
        desenhar_terreno = terrain2Draw;
    }
    if (key_hit['3']) {
        key_hit['3'] = 0;
        desenhar_terreno = terrain3Draw;
    }
    if (key_hit['4']) {
        key_hit['4'] = 0;
        desenhar_terreno = paredesDraw;
    }
    if (key_hit['5']) {
        key_hit['5'] = 0;
        desenhar_terreno = terrenoecasaDraw;
    }
    if (key_hit['6']) {
        key_hit['6'] = 0;
        desenhar_terreno = terrenoecasa2Draw;
    }

    if (key_hit['7']) {
        key_hit['7'] = 0;
        desenhar_terreno = casa_montanhaDraw;
    }

    if (key_hit['8']) {
        key_hit['8'] = 0;
        desenhar_terreno = casa_montanha_rapidoDraw;
    }

    if (key_hit['9']) {
        key_hit['9'] = 0;
        desenhar_terreno = com_texturaDraw;
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
    float vel_vel = 100;

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

int main(int argc, char *argv[])
{
    int mode = SDL_HWSURFACE | SDL_HWACCEL |
        SDL_DOUBLEBUF | SDL_OPENGL;

    int shift = 1;
    if (argc >= 2 && ! strcmp(argv[1], "-fs")) {
        mode |= SDL_FULLSCREEN;
        res_x = 1360;
        res_y = 768;
        shift++;
    }
    if (argc >= 3) {
        res_x = atoi(argv[shift]);
        res_y = atoi(argv[shift+1]);
    }

    memset(key_pressed, 0, 256);

    glutInit(&argc, argv);

    putenv("SDL_VIDEO_CENTERED=1");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_Surface* sdl = SDL_SetVideoMode(res_x, res_y, 32, mode);

    initgl();

    SDL_Event ev;

    while (1)
        if (! SDL_PollEvent(&ev)
            || ev.type == SDL_MOUSEMOTION)
            break;
        else
            event_handler(ev);

    uint64_t old_time, new_time;
    uint64_t old_border, new_border;
    old_time = time_get();
    old_border = time_in_secs(old_time);

    float dt = 0;
    int count = 0;

    while (1) {
        while (SDL_PollEvent(&ev))
            event_handler(ev);

        new_time = time_get();
        new_border = time_in_secs(new_time);

        dt = time_diff(new_time, old_time);

        if (new_border > old_border) {
            snprintf(fps_str, 8, "%d FPS", count);
            printf("fps: %d\n", count);
            old_border = new_border;
            count = 0;
        }

        old_time = new_time;
        count++;

        snprintf(status_str[0], 256, "vel %.1f pos (% 9.3f,% 7.3f,% 9.3f) "
                 "theta % 6.1f phi % 5.1f",
                 vel, mypos.x, mypos.y, mypos.z,
                 theta, phi);


        snprintf(status_str[1], 256, "l% 8.2f, %.0f,%.0f r%7.1f "
                 "c%6.3f,%6.3f,%6.3f h%.0f s%.2f d%.2f",
                 light[0], light[1], light[2], rluz,
                 light_color[0], light_color[1], light_color[2],
                 shininess, specular[0], diffuse[0]);

        toggle();
        model(dt);
        physics(dt);
        draw();
    }
}
