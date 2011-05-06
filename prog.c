#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <time.h>

#include "mesh/terreno.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <GL/freeglut.h>

typedef struct {
    float x;
    float y;
    float z;
} vec3;

int res_x = 640, res_y = 480;  /* resolucao padrao */

float vel = 4;                 /* velocidade linear */

float ang_vel = 0.2;           /* velocidade angular */

float theta = 0;               /* orientacao no plano xz */
float phi = 0;                 /* orientacao no plano xy */

float max_phi = 90;            /* limitacao ao olhar pra cima */
float min_phi = -90;

vec3 mypos = { .x = 0,        /* posicao do personagem */
               .y = 0,
               .z = 10 };

char key_pressed[256];

char fps_str[8] = "0 FPS";

GLuint vboID;

float timespec_diff(struct timespec end,
                              struct timespec start)
{
    float nsec = end.tv_nsec - start.tv_nsec;

    return (end.tv_sec - start.tv_sec) +
        nsec / 1000000000;
}

void cubep(float posx, float posy, float posz)
{
    glPushMatrix();
    glTranslatef(posx, posy, posz);
    glutWireCube (1.0);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, res_x, 0, res_y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1, 0.4, 0);
    glRasterPos2i(10, 10);
    glutBitmapString(GLUT_BITMAP_8_BY_13, fps_str);
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

void draw()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    setup_projection();
    glLoadIdentity ();

    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);
    glTranslatef(-mypos.x, -mypos.y, -mypos.z);

    layer(0);

    glScalef(20, 20, 20);

    glBindBuffer(GL_ARRAY_BUFFER, vboID);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, terrenoNumVerts);
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    draw_fps();

    SDL_GL_SwapBuffers();
}

void initVBO()
{
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * terrenoNumVerts,
                 terrenoVerts,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void initgl()
{
    glClearColor (0.0, 0.0, 0.0, 0.0);

    glViewport (0, 0, res_x, res_y); 

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initVBO();
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
        printf("enabled\n");
        key_pressed[ev.key.keysym.sym] = 1;
    }
    else if (ev.type == SDL_KEYUP) {
        printf("disabled\n");
        key_pressed[ev.key.keysym.sym] = 0;
    };
}

void physics(float dt)
{
    float vel_sin = vel * sin(M_PI * theta / 180) * dt;
    float vel_cos = vel * cos(M_PI * theta / 180) * dt;

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

    struct timespec old_time, new_time;

    clock_gettime(CLOCK_MONOTONIC, &old_time);

    float dt = 0;
    int count = 0;

    while (1) {
        while (SDL_PollEvent(&ev))
            event_handler(ev);

        clock_gettime(CLOCK_MONOTONIC, &new_time);
        dt = timespec_diff(new_time, old_time);

        if (new_time.tv_sec > old_time.tv_sec) {
            printf("fps: %d\n", count);
            snprintf(fps_str, 8, "%d FPS", count);
            count = 0;
        }

        old_time = new_time;
        count++;

        physics(dt);
        draw();
    }
}
