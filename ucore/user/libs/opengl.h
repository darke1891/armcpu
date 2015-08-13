// OpenGL ? no way, it is a joke

//#include "system.h"

int gl_width;
int gl_height;

int gl_i;
int gl_j;


#define MAX_WIDTH 512
#define MAX_HEIGHT 300

typedef volatile unsigned* memio_ptr_t;
memio_ptr_t const
	vga_buffer = (memio_ptr_t)0xBA000000;

void gl_set_size(int width, int height) {
    gl_width = width;
    gl_height = height;
    memio_ptr_t ptr = vga_buffer;
    int i,j;
    for (i=0; i<MAX_HEIGHT; i++) {
        for (j=0; j<MAX_WIDTH; j++)
            ptr[j] = 0;
        ptr += 512;
    }
    gl_i = gl_j = 0;
}

void gl_push_pixel(int color) {
    int index = gl_i * 512 + gl_j;
    if (gl_i < MAX_WIDTH && gl_j < MAX_WIDTH)
        vga_buffer[index] = color;

    gl_j ++;
    if (gl_j >= gl_width) {
        gl_j = 0;
        gl_i ++;
        if (gl_i >= gl_height)
            gl_i = 0;
    }
}
