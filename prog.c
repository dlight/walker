#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nanosec.h"

#include <SDL/SDL.h>

#define GL_GLEXT_PROTOTYPES

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "mesh/terrain.h"

typedef struct {
    float x;
    float y;
    float z;
} vec3;

int res_x = 640, res_y = 480;  /* resolucao padrao */

float vel = 10;                /* velocidade linear */

float ang_vel = 0.2;           /* velocidade angular */

float theta = 0;               /* orientacao no plano xz */
float phi = 0;                 /* orientacao no plano xy */

float max_phi = 90;            /* limitacao ao olhar pra cima */
float min_phi = -90;

vec3 mypos = { .x = 0,        /* posicao do personagem */
               .y = 0,
               .z = 15 };

float ang_vel_light = 10;
float theta_light = 0;

float light[4] = { 0, 5, 0, 1 };

char key_pressed[256];

char fps_str[8] = "0 FPS";

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



void draw_fps()
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void setup_projection()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45,
                   (GLfloat) res_x /
                   (GLfloat) res_y, 1, 200);
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
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    setup_projection();
    glLoadIdentity ();

    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);
    glTranslatef(-mypos.x, -mypos.y, -mypos.z);

    glLightfv(GL_LIGHT0, GL_POSITION, light);

    draw_light_point();

    layer(0);

    terrainDraw();

    draw_fps();

    SDL_GL_SwapBuffers();
}
void initgl()
{
    glClearColor (0.0, 0.0, 0.0, 0.0);

    glViewport (0, 0, res_x, res_y); 

    glShadeModel(GL_SMOOTH);

    glPointSize(5.0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    float black[4]={ 0, 0, 0, 0 };
    float light_gray[4]={ 0.6, 0.6, 0.6, 0 };
    float dark_gray[4]={ 0.2, 0.2, 0.2, 0 };

    float greenish[4] = { 0.8, 1, 0.8, 0 };

    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, black);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, greenish);
    glLightfv(GL_LIGHT0, GL_SPECULAR, greenish);

    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /* testando.. */
    float w = 120;

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, black);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, light_gray);
    glMaterialfv(GL_FRONT, GL_SPECULAR, dark_gray);
    glMaterialfv(GL_FRONT, GL_SHININESS, &w);
}

void event_handler(SDL_Event ev)
{
    if (ev.type == SDL_QUIT ||
        (ev.type == SDL_KEYDOWN &&
         ev.key.keysym.sym == SDLK_ESCAPE)) {

        SDL_Quit();
        exit(0);
    }

    if (ev.type == SDL_MOUSEMOTION) {
        theta -= ev.motion.xrel * ang_vel;
        phi -= ev.motion.yrel * ang_vel;

        if (phi > max_phi)
            phi = max_phi;
        else if (phi < min_phi)
            phi = min_phi;
    }
    else if (ev.type == SDL_KEYDOWN) {
        key_pressed[ev.key.keysym.sym] = 1;
    }
    else if (ev.type == SDL_KEYUP) {
        key_pressed[ev.key.keysym.sym] = 0;
    };
}

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

    theta_light += ang_vel_light * dt;
    light[0] = cos(rad * theta_light) * 5;
    light[2] = sin(rad * theta_light) * 5;
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
            printf("fps: %d\n", count);
            snprintf(fps_str, 8, "%d FPS", count);
            old_border = new_border;
            count = 0;
        }

        old_time = new_time;
        count++;

        physics(dt);
        draw();
    }
}
