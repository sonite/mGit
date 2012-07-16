#ifndef _sip_relays_H
#define _sip_relays_H

//** D E F I N I T I O N S ****************************************************/
// PINS
#define SIP0        (LATAbits.LATA5)
#define SIP1        (LATAbits.LATA4)
#define SIP2        (LATCbits.LATC1)
#define SIP3        (LATCbits.LATC0)

#define SIPJP3      (LATAbits.LATA4)
#define SIPJP4      (LATAbits.LATA5)
#define SIPJP5      (LATCbits.LATC0)
#define SIPJP6      (LATCbits.LATC1)

#define BUFJP7		(LATCbits.LATC6)
#define SIPJP7      (LATCbits.LATC6)
#define SIPJP8      (LATCbits.LATC7)
#define BUFJP8      (LATCbits.LATC7)
#define SIPJP9      (LATJbits.LATJ4)
#define SIPJP10    	(LATJbits.LATJ5)
#define BUFJP10		(LATJbits.LATJ5)

//** I N C L U D E S **********************************************************/
//*****************************************************************************/

//** V A R I A B L E S ********************************************************/
//*****************************************************************************/

//** F U N C T I O N S ********************************************************/
#define SIPJP3on()  (SIPJP3 = 1)
#define SIPJP4on()  (SIPJP4 = 1)
#define SIPJP5on()  (SIPJP5 = 1)
#define SIPJP6on()  (SIPJP6 = 1)

#define SIPJP3off() (SIPJP3 = 0)
#define SIPJP4off() (SIPJP4 = 0)
#define SIPJP5off() (SIPJP5 = 0)
#define SIPJP6off() (SIPJP6 = 0)

#define SIP0on()    (SIP0 = 1)
#define SIP1on()    (SIP1 = 1)
#define SIP2on()    (SIP2 = 1)
#define SIP3on()    (SIP3 = 1)

#define SIP0off()    (SIP0 = 0)
#define SIP1off()    (SIP1 = 0)
#define SIP2off()    (SIP2 = 0)
#define SIP3off()    (SIP3 = 0)
//*****************************************************************************/

#endif
