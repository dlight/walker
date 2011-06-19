/* walker.c : loop principal do arquivo

   COMO FUNCIONA

   * O programa está dividido em vários arquivos. Os mais importantes
   * são:

     * este daqui, walker.c - aqui está o main, e todo o setup inicial
     * do SDL. os outros arquivos exportam funcoes, que este aqui usa
     * como uma biblioteca

     * event.c - lida com todo tipo de entrada do usuario e suas
     * consequências imediatas, seja via teclado ou mouse. suas funcoes
     * sao chamadas uma vez a cada iteracao do loop principal

     * gl.c - lida com tudo relacionado com o opengl. eh o arquivo mais
     * importante

*/

#include <SDL/SDL.h>

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "nanosec.h"
#include "event.h"
#include "gl.h"

SDL_Surface* initsdl(int mode)
{
    putenv("SDL_VIDEO_CENTERED=1");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_GrabInput(SDL_GRAB_ON);
    return SDL_SetVideoMode(res_x, res_y, 32, mode);
}

int main(int argc, char *argv[])
{
    int mode = SDL_HWSURFACE | SDL_HWACCEL |
        SDL_DOUBLEBUF | SDL_OPENGL;

    if (argc >= 2 && (! strcmp(argv[1], "-h") ||
                      ! strcmp(argv[1], "--help"))) {
        printf("Uso: %s [-fs] resolucao-x resolucao-y\n\n"
               "Exemplo: ./walker 1024 768\n\n"
               "Para mais informacoes, leia o README.\n",
               argv[0]);
        exit(0);
    }

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

    init_event_keys();

    glutInit(&argc, argv);
    initsdl(mode);
    initgl();

    carregar_texturas();

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
            update_fps_str(count);
            old_border = new_border;
            count = 0;
        }

        old_time = new_time;
        count++;

        update_status_str();

        toggle();
        model(dt);
        physics(dt);
        draw();
    }
}
