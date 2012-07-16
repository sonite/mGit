/******************************************************************************
 *                                                                      	  *
 *        	              Integer Types PIC 18F87J50						  *
 *                                                                      	  *
 *              		   Author: Finsun Inresol AB			              *
 ******************************************************************************
 * FileName:        inttypes.h												  *
 * Dependencies:	none													  *
 * Processor:       PIC18													  *
 * Complier:        MCC18 v3.13												  *
 * Description: 	Integer Types											  *
 ******************************************************************************/
typedef enum
{
	false,
	true
}
bool;

#ifdef __18CXX
typedef signed char int8_t; 				// -128 -> 127				 // Char & Signed Char
typedef unsigned char uint8_t; 				// 0 -> 255					 // Unsigned Char
typedef signed short int int16_t; 			// -32768 -> 32767			 // Int
typedef unsigned short int uint16_t; 		// 0 -> 65535				 // Unsigned Int
typedef signed short long int int24_t; 		// -8388608 -> 8388607		 // Short Long
typedef unsigned short long int uint24_t;	// 0 -> 16777215			 // Unsigned Short Long
typedef signed long int int32_t;			// -2147483648 -> 2147483647 // Long
typedef unsigned long int uint32_t; 		// 0 -> 4294967295			 // Unsigned Long
#else
#	include <stdint.h>
#endif
