// SL 2021-01-22 @sylefeb

// https://github.com/sylefeb/Silice
// MIT license, see LICENSE_MIT in Silice repo root

int costbl[256] = {127,127,127,127,126,126,126,125,125,124,123,122,122,121,120,118,117,116,115,113,112,111,109,107,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,49,46,43,40,37,34,31,28,25,22,19,16,12,9,6,3,0,-3,-6,-9,-12,-16,-19,-22,-25,-28,-31,-34,-37,-40,-43,-46,-49,-51,-54,-57,-60,-63,-65,-68,-71,-73,-76,-78,-81,-83,-85,-88,-90,-92,-94,-96,-98,-100,-102,-104,-106,-107,-109,-111,-112,-113,-115,-116,-117,-118,-120,-121,-122,-122,-123,-124,-125,-125,-126,-126,-126,-127,-127,-127,-127,-127,-127,-127,-126,-126,-126,-125,-125,-124,-123,-122,-122,-121,-120,-118,-117,-116,-115,-113,-112,-111,-109,-107,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-85,-83,-81,-78,-76,-73,-71,-68,-65,-63,-60,-57,-54,-51,-49,-46,-43,-40,-37,-34,-31,-28,-25,-22,-19,-16,-12,-9,-6,-3,0,3,6,9,12,16,19,22,25,28,31,34,37,40,43,46,49,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,107,109,111,112,113,115,116,117,118,120,121,122,122,123,124,125,125,126,126,126,127,127,127};

int fxcos(int angle)
{
  return costbl[angle&255];
}

int fxsin(int angle)
{
  return - costbl[(angle + 64)&255];
}

void rotX(int *M, int angle)
{
  M[0] = 128; M[1] =            0; M[2] =   0;
  M[3] =   0; M[4] = fxcos(angle); M[5] = - fxsin(angle);
  M[6] =   0; M[7] = fxsin(angle); M[8] =   fxcos(angle);
}

void rotY(int *M, int angle)
{
  M[0] =  fxcos(angle); M[1] =   0; M[2] = fxsin(angle);
  M[3] =             0; M[4] = 128; M[5] =            0;
  M[6] = -fxsin(angle); M[7] =   0; M[8] = fxcos(angle);
}

void rotZ(int *M, int angle)
{
  M[0] = fxcos(angle); M[1] = -fxsin(angle); M[2] =   0;
  M[3] = fxsin(angle); M[4] =  fxcos(angle); M[5] =   0;
  M[6] =            0; M[7] =             0; M[8] = 128;
}

void scale(int *M,int sc)
{
  M[0] = sc; M[1] =  0; M[2] = 0;
  M[3] =  0; M[4] = sc; M[5] = 0;
  M[6] =  0; M[7] =  0; M[8] = sc;
}

void scale3(int *M,int sx,int sy,int sz)
{
  M[0] = sx; M[1] =  0; M[2] = 0;
  M[3] =  0; M[4] = sy; M[5] = 0;
  M[6] =  0; M[7] =  0; M[8] = sz;
}

void mulM(int *M,const int *A,const int *B)
{
  M[0] = (A[0]*B[0] + A[1]*B[3] + A[2]*B[6]) >> 7;
  M[1] = (A[0]*B[1] + A[1]*B[4] + A[2]*B[7]) >> 7;
  M[2] = (A[0]*B[2] + A[1]*B[5] + A[2]*B[8]) >> 7;

  M[3] = (A[3]*B[0] + A[4]*B[3] + A[5]*B[6]) >> 7;
  M[4] = (A[3]*B[1] + A[4]*B[4] + A[5]*B[7]) >> 7;
  M[5] = (A[3]*B[2] + A[4]*B[5] + A[5]*B[8]) >> 7;

  M[6] = (A[6]*B[0] + A[7]*B[3] + A[8]*B[6]) >> 7;
  M[7] = (A[6]*B[1] + A[7]*B[4] + A[8]*B[7]) >> 7;
  M[8] = (A[6]*B[2] + A[7]*B[5] + A[8]*B[8]) >> 7;
}

void draw_triangle(char color,char shade,int px0,int py0,int px1,int py1,int px2,int py2)
{
  int tmp;

  // front facing?
  int d10x  = px1 - px0;
  int d10y  = py1 - py0;
  int d20x  = px2 - px0;
  int d20y  = py2 - py0;
  int cross = d10x*d20y - d10y*d20x;
  if (cross <= 0) return;
  if (shade) {
    color = color + (cross >> shade);
  }

  // reduce precision after shading
  px0 >>= 5; py0 >>= 5;
  px1 >>= 5; py1 >>= 5;
  px2 >>= 5; py2 >>= 5;

  // 0 smallest y , 2 largest y
  if (py0 > py1) {
    tmp = py1; py1 = py0; py0 = tmp;
    tmp = px1; px1 = px0; px0 = tmp;
  }
  if (py0 > py2) {
    tmp = py2; py2 = py0; py0 = tmp;
    tmp = px2; px2 = px0; px0 = tmp;
  }
  if (py1 > py2) {
    tmp = py2; py2 = py1; py1 = tmp;
    tmp = px2; px2 = px1; px1 = tmp;
  }

  int e_incr0 = (py1-py0 == 0) ? 0xFFFFF : ((px1-px0)<<10) / (py1-py0);
  int e_incr1 = (py2-py1 == 0) ? 0xFFFFF : ((px2-px1)<<10) / (py2-py1);
  int e_incr2 = (py2-py0 == 0) ? 0xFFFFF : ((px2-px0)<<10) / (py2-py0);

  if ((e_incr0 == 0xFFFFF && e_incr1 == 0xFFFFF)
   || (e_incr0 == 0xFFFFF && e_incr2 == 0xFFFFF)
   || (e_incr1 == 0xFFFFF && e_incr2 == 0xFFFFF)) {
    // flat triangle
    return;
  }

  // wait for any pending draw to complete
  while ((userdata()&1) == 1) {  }

  // send commands
  *(TRIANGLE+  0) = (px0&1023) | ((py0&1023) << 10);
  *(TRIANGLE+  1) = (px1&1023) | ((py1&1023) << 10);
  *(TRIANGLE+  2) = (px2&1023) | ((py2&1023) << 10);
  *(TRIANGLE+  3) = (e_incr0&0xffffff) | (color << 24);
  *(TRIANGLE+  4) = (e_incr1&0xffffff);
  *(TRIANGLE+  5) = (e_incr2&0xffffff);
}

void clear(char color,int xm,int ym,int xM,int yM)
{
  draw_triangle(color,0,
    xm<<5,  ym<<5,
    xM<<5,  ym<<5,
    xM<<5,  yM<<5
    );
  draw_triangle(color,0,
    xm<<5,  ym<<5,
    xM<<5,  yM<<5,
    xm<<5,  yM<<5
    );
}
