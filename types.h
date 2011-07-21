#ifndef TYPES_H
#define TYPES_H

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
 } rgba;

typedef struct {
  float vertex[3];
  float normal[3];
  float texcoord[2];
} mesh_t;

#endif
