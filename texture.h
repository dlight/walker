typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
 } rgba;

rgba* load_png(char* filename, unsigned* width,
               unsigned* height);
GLuint setup_texture(rgba* image_data, unsigned w,
                     unsigned h);
GLuint png_texture(char* filename, unsigned* w,
                   unsigned* h);
