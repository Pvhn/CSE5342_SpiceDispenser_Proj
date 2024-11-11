// EEPROM functions
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target uC:       TM4C123GH6PM
// System Clock:    -

#ifndef EEPROM_H_
#define EEPROM_H_

typedef union
{
	uint32_t FullWord;

	struct
	{
		uint16_t Lower16Bits : 16;
		uint16_t Upper16Bits : 16;
	} HalfWord;

}EEPROMDataBlockType;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initEeprom();
uint16_t writeEeprom(uint16_t add, uint32_t data);
uint32_t readEeprom(uint16_t add);

#endif
