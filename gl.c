#define GL_GLEXT_PROTOTYPES

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "types.h"
#include "texture.h"
#include "event.h"

#include "mesh/ruinas.h"

#include "gl.h"

GLuint ruinas_textura, ruinas_minimap;

int res_x = 640, res_y = 480;    /* resolucao padrao */

void (*desenhar_terreno)(void) = ruinasDraw;

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

    glColor3f(1, 1, 1);
    glRasterPos2i(10, 10);

    for (int i = 0; fps_str[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, fps_str[i]);

    glRasterPos2i(10, res_y - 20);

    for (int i = 0; status_str[0][i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, status_str[0][i]);

    glRasterPos2i(10, res_y - 20 - 14);

    for (int i = 0; status_str[1][i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, status_str[1][i]);
}

void draw_map()
{
    glBindTexture(GL_TEXTURE_2D, ruinas_minimap);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f (0.1, 0.8, 0.95, 0.6);

    int size = 200;
    int m = 5;

    glPushMatrix();
    glTranslated(res_x - size - m, res_y - size - m, 0);

    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);    glVertex2i(0, 0);
    glTexCoord2f(1, 1);    glVertex2i(size, 0);
    glTexCoord2f(1, 0);    glVertex2i(size, size);
    glTexCoord2f(0, 0);    glVertex2i(0, size);

    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glColor4f(1, 0, 0, 1);

    glBegin(GL_POINTS);

    glVertex2f(-mypos.z / 10 + 95,
               - mypos.x / 10 + 105);

    glEnd();

    glPopMatrix();
}

void projection_2d()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, res_x, 0, res_y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void projection_3d()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45,
                   (GLfloat) res_x /
                   (GLfloat) res_y, 10, 10000);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void draw_light_point()
{
    glBegin(GL_POINTS);
    glVertex3f(light[0], light[1], light[2]);
    glEnd();
}

void draw()
{
    if (use_texture)
        glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_color);

    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);

    projection_3d();

    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);

    glTranslatef(-mypos.x, -mypos.y, -mypos.z);

    float luz1[4] = { mypos.x, mypos.y+10, mypos.z, 1 };

    glLightfv(GL_LIGHT0, GL_POSITION, light);
    glLightfv(GL_LIGHT1, GL_POSITION, luz1);

    glBindTexture(GL_TEXTURE_2D, ruinas_textura);
    desenhar_terreno();

    glDisable(GL_TEXTURE_2D);

    layer(0);

    glDisable(GL_LIGHTING);

    draw_light_point();

    projection_2d();

    glDisable(GL_DEPTH_TEST);


    glEnable(GL_TEXTURE_2D);

    draw_map();

    glDisable(GL_TEXTURE_2D);

    if (!hide_text)
        draw_status();

    SDL_GL_SwapBuffers();
}
void initgl()
{
    glClearColor (0.0, 0.0, 0.2, 0.5);

    glViewport (0, 0, res_x, res_y); 

    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPointSize(1.5);

    glEnable(GL_CULL_FACE);
    
    float ambient[]={ 0.005, 0.005, 0.005, 0 };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001);

    glEnable(GL_LIGHT1);

    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05);

    float luz0[]={ 3, 3, 3, 1 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0);
}

void carregar_texturas()
{
    unsigned w, h;

    ruinas_textura = png_texture("./mesh/ruinas.png", &w, &h);
    printf("img %d, %d\n", w, h);
    ruinas_minimap = png_texture("./mesh/ruinas_minimap.png", &w, &h);
    printf("heightmap %d, %d\n", w, h);
}
