#ifndef GL_ERROR_H
#define GL_ERROR_H

#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

static int _C = 0;

#define GL_ERROR(name) { 		   \
	int _T = glGetError();		   \
	while (_T != 0) {		   \
	    printf("Error: " name " %s\n", \
		   gluErrorString(_T));	   \
	    _C++;			   \
	    if (_C > 5)			   \
		exit(0);		   \
	    _T = glGetError();		   \
	}				   \
    }

#endif
