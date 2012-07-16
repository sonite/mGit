#include "Flash.h" 
/*****************************************************************
 * EECON1 is the control register for memory accesses (EEPROM)   *
 *****************************************************************
 * EECON2 not a physical register, it is used exclusively in the *
 * memory write and erase sequences.							 *
 *****************************************************************/
 
/** Perform FLASH erase or write operation */
static void Flash_Execute(void)
{
	INTCONbits.GIE = 0; /* Disable interrupts */
	EECON1bits.WREN = 1; /* Allows write cycles to Flash program memory */

	/** Write the magic sequence to EECON2 */
	EECON2 = 0x55;
	EECON2 = 0xAA;
	
	EECON1bits.WR = 1;  /** Start the erase/write cycle and stall the CPU while it's being performed */
	EECON1 = 0x00;		/** Prevent any further writes */
	INTCONbits.GIE = 1; /** Re-enable interrupts */
}

/** Erasing Flash Program Memory (1024-byte block) */ 
void Flash_Erase(void rom *ptr)
{
	/** Load Table Pointer register with address of row being erased (base address of the memory block). */
	TBLPTR = (uint24_t) ptr;
	EECON1bits.FREE = 1; /** Set FREE bits to enable the erase operation. */
	Flash_Execute();
}

/** Write a 64-byte block of FLASH two bytes at a time supported. */
void rom *Flash_Write(void rom *dst, const void *src)
{
	unsigned char len;
	TBLPTR = (uint24_t) dst; 	/** Load Table Pointer register with target. */
	FSR0 = (uint16_t) src;		/** Load Table Pointer register with address of data to be written. */
	
	/** Write the 64 bytes from RAM into the FLASH buffer. */
	len = FLASH_WRITE_BLOCK; // 64
	do
	{
	/* Since the Table Latch (TABLAT) is only a single byte, the
	 * TBLWT instruction may need to be executed 64 times for
	 * each programming operation (if WPROG = 0). */
		TABLAT = POSTINC0;
		_asm TBLWTPOSTINC _endasm;
	}
	while(--len);
	
	_asm TBLRDPOSTDEC _endasm; /** Decrement the table pointer to keep it within the page. */

	EECON1bits.WR = 1; 		   /** Perform the write operation */
	Flash_Execute();

	_asm TBLRDPOSTINC _endasm; /** Return a pointer to the next block to be written */
	return (void rom *) TBLPTR;
}
