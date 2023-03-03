#include <line.h>

int getParabolaVertexArray(GLfloat * vertexArray, GLfloat a, GLfloat b, GLfloat c, GLfloat xb, GLfloat xe, GLfloat step) {
    GLfloat y = a * xb * xb + b * xb + c;
    GLfloat dy = 2 * a * xb * step + a * step * step + b * step;
    GLfloat ddy = 2 * a * step * step;
    int count = 0;
    while (xb + step < xe) {
        *(vertexArray++) = xb;
        *(vertexArray++) = y;
        xb += step;
        y += dy;
        dy += ddy;
        *(vertexArray++) = xb;
        *(vertexArray++) = y;
        count += 4;
    }
    return count;
}
