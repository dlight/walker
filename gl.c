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
#include "mesh/sky.h"

#include "gl.h"

GLuint ruinas_textura, ruinas_minimap, sky_textura;

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
    gluPerspective(60,
                   (GLfloat) res_x /
                   (GLfloat) res_y, 0.01, 500);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void posicionar_camera()
{
    glRotatef(phi, -1, 0, 0);
    glRotatef(theta, 0, -1, 0);

    mypos.y = minha_altura;

    if (use_heightmap)
        mypos.y += ((float)altura_terreno / 256.) * 60.653 - 20.849;

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
            glClearColor (0.3, 0.3, 0.3, 0);
            glEnable(GL_FOG);
    }
    else {
        glClearColor(0, 0, 0.1, 0);
    }
}

void sky()
{
	//glColor4f(1, 1, 1, 1);
	//glScalef(100, 100, 100);
//    glEnable(GL_LIGHTING);
    if (use_texture)
        glEnable(GL_TEXTURE_2D);
	
    glBindTexture(GL_TEXTURE_2D, sky_textura);
    skyDraw();
	
    //glDisable(GL_TEXTURE_2D);
//    glDisable(GL_LIGHTING);
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

void linhas ()
{
    if (!show_grid)
        return;

    glPushMatrix();

    glBegin(GL_LINES);

    int n = 15;

    for (int i = -n; i < n+1; i++) {
        glVertex3f(10*i, 0, n*10);
        glVertex3f(10*i, 0, -n*10);
        glVertex3f(n*10, 0, 10*i);
        glVertex3f(-n*10, 0, 10*i);
    }

    glVertex3f(0, n*10, 0);
    glVertex3f(0, -n*10, 0);

    glEnd();

    glPopMatrix();
}

void draw_light_point()
{
    glPointSize(1);
    glBegin(GL_POINTS);
    glVertex3f(light[0], light[1], light[2]);
    glEnd();
}

void desenhar_mundo_3d()
{
    glEnable(GL_DEPTH_TEST);

    fog();


    linhas();
    terreno();
    draw_light_point();
	    glDisable(GL_FOG);
	sky();

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

void direcao()
{
    glPushMatrix();

    glRotatef(theta, 0, 0, 1);

    

    glBegin(GL_TRIANGLES);

    glColor4f(0, 1, 0, 0.1);

    glVertex2f(-5, 15);

    glColor4f(0, 1, 0, 0.4);
    glVertex2f(0, 0);

    glColor4f(0, 1, 0, 0.1);
    glVertex2f(5, 15);

    glEnd();

    glPopMatrix();
}

void draw_map()
{
    if (!show_map)
        return;

    glPolygonMode(GL_FRONT, GL_FILL);

    glBindTexture(GL_TEXTURE_2D, ruinas_minimap);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f (0.5, 0.8, 1, 0.6);

    int m = 5;

    unsigned l = screen_map_len;

    glPushMatrix();
    glTranslated(res_x - l - m,
                 res_y - l - m, 0);


    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1);    glVertex2i(0, 0);
    glTexCoord2f(1, 1);    glVertex2i(l, 0);
    glTexCoord2f(1, 0);    glVertex2i(l, l);
    glTexCoord2f(0, 0);    glVertex2i(0, l);

    glEnd();

    glDisable(GL_TEXTURE_2D);


    glTranslated(map_pos_u, map_pos_v, 0);

    direcao();
    

    glDisable(GL_BLEND);

    glColor4f(1, 0, 0, 1);

    glPointSize(2);

    glBegin(GL_POINTS);

    glVertex2f(0, 0);

    glEnd();

    glPopMatrix();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    if (wireframe)
        glPolygonMode(GL_FRONT, GL_LINE);
}

void draw_status()
{
    if (!hide_text)
        return;

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
    draw_map();
    draw_status();
}



// -----     -----------------     ----- //
// |                                   | //
// |          desenha o frame          | //
// |                                   | //
// -----     -----------------     ----- //

void initdraw()
{
    if (wireframe)
        glPolygonMode(GL_FRONT, GL_LINE);
    else
        glPolygonMode(GL_FRONT, GL_FILL);

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

    glEnable(GL_CULL_FACE);
    
    float ambient[]={ 0.1, 0.1, 0.1, 0 };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001);

    glEnable(GL_LIGHT1);

    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);

    float luz0[]={ 10, 10, 10, 1 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luz0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz0);


    GLfloat fogColor[4]= {0.3, 0.3, 0.3, 1};


    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.05f);
    glFogf(GL_FOG_START, 0);
    glFogf(GL_FOG_END, 300.0f);
}

void carregar_texturas()
{
    ruinas_textura = png_texture("./mesh/ruinas.png");

    ruinas_minimap =
        png_loadmap("./mesh/ruinas_map.png",
                    &ruinas_map, &map_len_u,
                    &map_len_v);
	
	sky_textura = png_texture("./mesh/sky.png");
}
