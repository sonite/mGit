#ifndef _MEL_DELAY_H
#define _MEL_DELAY_H

//** D E F I N I T I O N S ****************************************************/
#define OVERHEAD (2)
#define CONST_RANGE(min, val, max) (sizeof(char (*)[(val) >= (min) && (val) <= (max) ? +1 : -1]), (val))

#define DELAY_US_10X(n) CONST_RANGE(1, ((n) * 12L + 6) / 10, 255)
#define Delay_us(n) Delay10TCYx(DELAY_US_10X(n))

#define DELAY_MS_10KX(n) CONST_RANGE(1, ((n) * 12L + 6) / 10, 255)
#define Delay_ms(n) Delay10KTCYx(DELAY_MS_10KX(n))

#endif
//*****************************************************************************/

