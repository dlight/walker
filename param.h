typedef struct {
    float x;
    float y;
    float z;
} vec3;

int res_x = 640, res_y = 480;    /* resolucao padrao */

float vel = 150;                 /* velocidade linear */
float ang_vel = 0.2 ;            /* velocidade angular */

float theta = -271;              /* orientacao no plano xz */
float phi = 0;                   /* orientacao no plano xy */

float max_phi = 90;              /* limitacao ao olhar pra cima */
float min_phi = -90;

vec3 mypos =                     /* posicao do personagem */
    { .x = 620, .y = 42, .z = 18 };


float light[4] =                 /* posicao da luz 0 */
    { 0, 700, 0, 1 };
char stop_light = 0;             /* luz 0 esta parada? */

float light_color[4] =           /* cor da luz 1 */
    { 0.1, 0.11, 0.1, 0 };

float shininess = 128;           /* brilho do material */  
float diffuse[4]=                /* reflectancia difusa */
    { 15.2, 15.2, 15.2, 1 };
float specular[4]=               /* reflectancia especular */
    { 15.2, 15.2, 15.2, 1 };

char key_pressed[256];           /* keymap continuo */
char key_hit[256];               /* keymap toggle */

char fps_str[8] = "O FPS";       /* fps na tela */
char status_str[3][256];         /* variaveis na tela */

char hide_text = 0;              /* esconder texto */
char grab = 1;                   /* se o mouse esta preso na janela */
