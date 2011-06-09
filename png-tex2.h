#define PNG_DEBUG 3
#include <png.h>

#define TEXTURE_LOAD_ERROR 0

typedef struct {
    png_byte r;
    png_byte g;
    png_byte b;
    png_byte a;
 } rgba;

/** loadTexture
 *     loads a png file into an opengl texture object, using cstdio , libpng, and opengl.
 * 
 *     \param filename : the png file to be loaded
 *     \param width : width of png, to be updated as a side effect of this function
 *     \param height : height of png, to be updated as a side effect of this function
 * 
 *     \return GLuint : an opengl texture id.  Will be 0 if there is a major error,
 *                                     should be validated by the client of this function.
 * 
 */
GLuint loadTexture(char* filename, unsigned* width, unsigned* height) 
{
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
        PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
            ZLIB_VERSION, zlib_version);

    //header for testing if it is a png
    unsigned char header[8];
 
    //open file as binary
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("can't open file\n");
        return TEXTURE_LOAD_ERROR;
    }
 
    //read the header
    fread(header, 1, 8, fp);
 
    if (!png_check_sig(header, 8)) {
        fclose(fp);
        printf("signature error\n");
        return TEXTURE_LOAD_ERROR;
    }
 
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        printf("no png ptr\n");
        return (TEXTURE_LOAD_ERROR);
    }
 
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("no info ptr\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return (TEXTURE_LOAD_ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        printf("png error really rs\n");
        return (TEXTURE_LOAD_ERROR);
    }
 
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
 
    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);
    //png_read_update_info(png_ptr, info_ptr);

    // get info about png

    png_uint_32 w, h;

    w =  png_get_image_width(png_ptr, info_ptr);

    h = png_get_image_height(png_ptr, info_ptr);

    png_bytep *row_pointers =
        (png_bytep*) malloc(sizeof(png_bytep) * h);
    for (int y=0; y<h; y++) {
        row_pointers[y] = (png_byte*)
            malloc(png_get_rowbytes(png_ptr,info_ptr));
    }
 
    png_read_image(png_ptr, row_pointers);
    
    fclose(fp);

    rgba* image_data = malloc(4 * sizeof(rgba) * w * h);

    for (int y=0; y<h; y++) {
        png_byte* row = row_pointers[y];
        for (int x=0; x<w; x++) {
            png_byte* ptr = &(row[x*4]);
            //printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
            //x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

            rgba* ptr2 = &(image_data[x*w]);

            ptr2[y].r = ptr[0];
            ptr2[y].g = ptr[1];
            ptr2[y].b = ptr[2];
            ptr2[y].a = ptr[3];
        }
    }
 
    //Now generate the OpenGL texture object
    GLuint texture;
    printf("b\n");

    glGenTextures(1, &texture);
    printf("r\n");
    glBindTexture(GL_TEXTURE_2D, texture);
    printf("q\n");

    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    free(image_data);
    free(row_pointers);
 
    return texture;
}
