/* gl.c : operacoes envolvendo opengl

   COMO FUNCIONA

   * central, aqui, eh a funcao draw. ela desenha um unico frame

   * primeiro, começamos um novo desenho: limpamos a tela, carregamos
   * parâmetros iniciais, etc.

     * a reflectancia do material e a cor da luz acima do personagem
     * podem mudar durante a execução do programa. atualizamos esses
     * dados no opengl aqui

   * depois desenhamos o mundo 3D. para isso:

     * carregamos a matriz de projeção 3D
     * desenhamos o terreno (com ou sem textura)
     * desenhamos os cubos
     * desenhamos o pontinho da luz que se move la no alto

     * o mundo 3D utiliza o z-buffer

   * depois, desenhamos o mundo 2D. para isso:

     * carregamos a matriz de projeção 2D
     * desenhamos o mini mapa
     * desenhamos, se necessario, o FPS e o status

   * por fim, trocamos os buffers de desenho


   * alem disso, existe aqui o initgl, que cria as luzes, insere
   * parametros ao opengl, etc.


   * escrevo mais detalhes depois se for o caso..

 */


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
rgba* ruinas_map;
unsigned ruinas_x, ruinas_y;

int res_x = 800, res_y = 600;    /* resolucao padrao */

void (*desenhar_terreno)(void) = ruinasDraw;



// -----     -----------------     ----- //
// |                                   | //
// |              MUNDO 3D             | //
// |                                   | //
// -----     -----------------     ----- //

void projecao_3d()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45,
                   (GLfloat) res_x /
                   (GLfloat) res_y, 20, 5000);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

int f(int x, int y)
{
    return ruinas_map[ruinas_y*y+x].r;
}

void posicionar_camera()
{
    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);

    float xq = -mypos.z / 10 + 95;
    float yq = mypos.x / 10 + 105;

    int x = xq;
    int y = yq;

    int q[6] = { f(x-1, y), f(x, y-1), f(x-1, y-1),
                 f(x+1, y), f(x, y+1), f(x+1, y+1) };

    float p = 0;

    for (int i = 0; p < 6; i++)
        p += q[i];

    //float res = f(x, y) * 0.8 + 0.2 * (p / 6);

    float res = f(x, y);

    if (use_heightmap)
        mypos.y = res * 7.5 - 600;

    glTranslatef(-mypos.x, -mypos.y, -mypos.z);
}

void posicionar_luzes()
{
    float luz1[4] = { mypos.x, mypos.y+10, mypos.z, 1 };

    glLightfv(GL_LIGHT0, GL_POSITION, light);
    glLightfv(GL_LIGHT1, GL_POSITION, luz1);
}

void fog() {
    if (use_fog) {
            glClearColor (0.2, 0.2, 0.2, 0);
            glEnable(GL_FOG);
    }
    else {
        glClearColor(0, 0, 0.1, 0);
    }
}

void terreno()
{
    glEnable(GL_LIGHTING);
    if (use_texture)
        glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, ruinas_textura);
    desenhar_terreno();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}

void cubep(float posx, float posy, float posz)
{
    glPushMatrix();
    glTranslatef(posx, posy, posz);
    glutSolidCube (1.0);
    glPopMatrix();
}

void layer(float posz)
{
    glEnable(GL_LIGHTING);

    cubep(2, 2, posz);

    int i;
    for (i = 0; i < 30; i+=2) {
        cubep(i, 0, posz);
        cubep(i, 2, posz);
    }

    glDisable(GL_LIGHTING);
}

void draw_light_point()
{
    glBegin(GL_POINTS);
    glVertex3f(light[0], light[1], light[2]);
    glEnd();
}

void desenhar_mundo_3d()
{
    glEnable(GL_DEPTH_TEST);

    fog();

    terreno();
    layer(0);
    draw_light_point();

    glDisable(GL_DEPTH_TEST);
}



// -----     -----------------     ----- //
// |                                   | //
// |              MUNDO 2D             | //
// |                                   | //
// -----     -----------------     ----- //

void projecao_2d()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, res_x, 0, res_y);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_map()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, ruinas_minimap);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f (0.4, 0.9, 0.95, 0.6);

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

    float x = -mypos.z / 10 + 95;
    float y = - mypos.x / 10 + 105;

    glBegin(GL_POINTS);

    glVertex2f(x, y);

    glEnd();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
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

void desenhar_mundo_2d()
{
    glDisable(GL_FOG);

    draw_map();

    if (!hide_text)
        draw_status();
}



// -----     -----------------     ----- //
// |                                   | //
// |          desenha o frame          | //
// |                                   | //
// -----     -----------------     ----- //

void initdraw()
{
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_color);

    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
}

void draw()
{
    initdraw();

    projecao_3d();

    posicionar_camera();
    posicionar_luzes();
    desenhar_mundo_3d();

    projecao_2d();

    desenhar_mundo_2d();

    SDL_GL_SwapBuffers();
}



// -----     -----------------     ----- //
// |                                   | //
// |        inicializa o opengl        | //
// |                                   | //
// -----     -----------------     ----- //

void initgl()
{


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


    GLfloat fogColor[4]= {0.2, 0.2, 0.2, 1};


    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.001f);
    glFogf(GL_FOG_START, 0);
    glFogf(GL_FOG_END, 2000.0f);
}

void carregar_texturas()
{
    ruinas_textura = png_texture("./mesh/ruinas.png");

    ruinas_minimap =
        png_loadmap("./mesh/ruinas_minimap.png",
                    &ruinas_map, &ruinas_x,
                    &ruinas_y);
}
