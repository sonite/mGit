#ifndef _TESTER_H
#define _TESTER_H

// Width =128 Height = 64
static rom const unsigned char fin[] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
 128,192,192,224,240,240,248,248,252,252,252,126, 62, 62, 30, 30, 
  30, 30, 30, 30, 30, 62, 62, 60,124,252,248,248,240,240,224,224, 
 192,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  2,  7, 14, 14, 28, 28,248,240,248, 60,239, 
 255, 63,255,255, 63,  7, 25,255,255,195,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3,  7, 31,255,255, 
 255,255,255,222,124,240,192,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  2,  3,  3,  7,  7,  6,  6,254,255,255,  3,  0,255,255, 
   1,  7,127,255,240,224,192,192,193,199,207,206,220,248,248,240, 
 240,248,124, 60,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,240,255, 
  31,  0,255,255,252,  1, 15,255,252,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  3,  3,  3,  3,131,131,131,195,255,255,252,  0,  0,  7, 63, 
 252,240,128,  1,  7, 15, 28, 57,121,241,225,193,193,129,129,128, 
 128,128,128,128,128,192,192,192,224,112,120, 60, 14,  7,  3,128, 
 192,248,127, 31,  3,  0,192,255,255,  3,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  1,  3,  3,  3,131,129,193,193,227,127,127,124,240,192, 
 128,  3,  7, 15, 14, 28, 56,112,112,224,224,193,193,193,193,131, 
 131,131,131,129,193,193,193,192,224, 96,112, 56, 56, 28, 14,  7, 
   3,  1,192,224,248, 62,127,103,224,192,192,128,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  1,  3,  1,  1,  0, 64,224,112, 56, 60, 31, 
  15, 15, 14,220,248,112,112, 96,224,192,192,192,192,193,129,129, 
 129,129,129,129,193,193,192,192,192,224, 96,112,112,248,220,156, 
  14,  7, 15, 29, 56,112,224,192,  0,  0,  1,  3,  1,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,240,240,  0,  0,  0,  0,  0,  0,240,240,  0,  0,240,240,  0, 
   0,128,192,240,240,240,  0,  0, 48, 48, 48, 48, 48,224,192,  0, 
   0,192,224,112, 48, 48, 48,112,224,192,  0,  0,240,240,  0,  0, 
 136,220,255,247,241,  0,  0,  0,224,254, 63,  7,  1,  1,  1,  1, 
 255,255,255,  1,241,241,  1, 31,255,252,  0,  0,128,193,243,255, 
 254,  8,  0,240,240,  0,  0,  0,192,240,112,  0,  0,240,240, 48, 
  48, 48, 48,  0,  0, 48, 48, 48, 48, 48,224,192,  0,  0,128,224, 
  96, 48, 48, 48, 48, 96,224,128,  0,240,240, 48, 48, 48, 48,  0, 
   0,127,127, 99, 99, 99, 99,  0,  0,127,127,  0,  0,127,127,120, 
  62, 15,  3,  0,127,127,  0,  0, 24, 62,102,103,103, 99, 65,  0, 
   0,127,127,  0,  0,  0,  0,  0,127,127,  0,  0,127,127,120, 62, 
  15,  3,  0,127,127,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   1,  1,  1,  0,127,127,  0,  0,127,127,120, 62, 15,  3,  0,127, 
 127,  0,  0,127,127, 99, 99, 99, 99, 63, 60,  0,  0,127,127,102, 
 102,102,102,  0,  0, 24, 62,102,103,103, 99, 65,  0,  0, 15, 63, 
  48, 96, 96, 96, 96, 48, 63, 15,  0,127,127,  0,  0,  0,  0,  0 
};

// ------------------------------------------------------  
// GLCD Picture name: menu1.bmp            
// GLCD Model: KS0108 128x64            
// ------------------------------------------------------  

static rom const unsigned char menu1_bmp[] = {
 255,  1,255,  1,129,129,129,129,129,  1,193,129,129,  1,  1,  1, 
   1,255,  1,  1,  1,  1,253,  3,251,251, 27,203,227,255,225,193, 
   1,  1,  1,  1,  1,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  1, 
   1,  1,  1,  1,  1,  1,  7,  7,  1, 31, 31,  1,127,127,  1,255, 
 255,  1,255,255,  1,  1,  1,  1,  1,  1,255,  1,  1,  1,  1,  1, 
 225, 33,241,241,253,  1,  1,  1, 33,193,  1,  1,  1,  1,  1,255, 
 127, 64, 95, 64, 79, 79, 79, 79, 79, 64, 67, 67, 67, 67, 64, 64, 
  64,127, 64, 64, 64, 64, 71, 72, 91, 91, 88, 73, 93, 65, 93, 65, 
  64, 64, 64, 64, 64,127, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,127, 64, 
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 
  65, 64, 71, 71, 64, 64, 64, 64, 64, 64,127, 64, 64, 64, 64, 64, 
  67, 66, 71, 71, 95, 64, 64, 64, 66, 65, 64, 64, 64, 64, 64,127, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 
};

// ------------------------------------------------------  
// Picture name: fire_bmp            
// GLCD Model: Nokia3310 Like            
// ------------------------------------------------------  

static rom const unsigned char fire_bmp[] = {
1, 3, 225, 9, 241, 4, 
253, 71, 221, 51, 239, 15,
135, 255, 195, 254, 231, 15,
143, 63, 253, 3, 249, 11,
241, 7, 97, 0, 193, 3, 1, 0
};


#endif
