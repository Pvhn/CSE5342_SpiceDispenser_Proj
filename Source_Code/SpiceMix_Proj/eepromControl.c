/* =======================================================
 * File Name: eeprom_Control.c
 * =======================================================
 * File Description: Program Control for interfacing 
 * with the EEPROM. This contains specialized functions
 * for accessing various system data and stored recipes
 *
 * Target: TM4C123GH6PM w/ 40MHz Clock
 * =======================================================
 */


#include "eepromControl.h"

 /*========================================================
  * Variable Definitions
  *========================================================
  */

SpiceStructType DefaultSpices[MAXSLOTS] =
{
	{"SALT", 0xAB00},
	{"BLACKPEPPER", 0xCD01},
	{"GARLIC", 0xEF02},
	{"PAPRIKA", 0x1203},
	{"ONION", 0x2304},
	{"OREGANO", 0x4505},
	{"THYME", 0x6706},
	{"ROSEMARY", 0x8907},
};

/*========================================================
 * Function Declarations
 *========================================================
 */

//Forward Declaration since we don't this to be used outside of this library
uint16_t write_NameEEProm(uint16_t offset, uint8_t* name);
uint8_t* Read_NameEEProm(uint16_t offset);

uint8_t* Read_NameEEProm(uint16_t offset)
{
	// Static so that the name can be passed back to the caller
	static uint8_t name[MAXNAMESIZE];
	uint16_t indx = 0;
	uint16_t x = 0;
	uint8_t temp[4];
	bool endofstr = false;

	// Read the spice name 4 characters at a time
	for (indx = 0; (indx <= MAXNAMESIZE) && (endofstr == false); indx = indx + 4)
	{
		// Store off 4 characters 
		*((uint32_t*)(temp)) = readEeprom(offset);

		// Copy 4 characters at a time. Stop if Null
		for (x = 0; x < 4; x++)
		{
			name[indx+x] = temp[x];

			if (temp[x] == '\0')
			{
				endofstr = true;
				break;
			}
		}

		// Increment EEPROM Offset
		offset = offset + 1;
	}

	return name;
}

RecipeStructType Read_Recipe(uint8_t number)
{
	RecipeStructType recipe = { 0, };
	uint16_t indx = 0;
	uint16_t x = 0;
	uint16_t offset = 0;
	uint8_t temp[4];
	bool endofstr = false;
	 
	offset = (number * RECBLKSIZE) + RECBLKADDR;

	//TBD Add some validation for given number.
	// Not sure how to indicate bad number since function returns a struct

	for (indx = 0; (indx <= MAXNAMESIZE) && (endofstr == false); indx = indx + 4)
	{
		// Store off 4 characters 
		*((uint32_t*)(temp)) = readEeprom(offset);

		// Copy 4 characters at a time. Stop if Null
		for (x = 0; x < 4; x++)
		{
			recipe.Name[indx+x] = temp[x];

			if (temp[x] == '\0')
			{
				endofstr = true;
				break;
			}
		}

		// Increment EEPROM Offset
		offset = offset + 1;
	}

	// Extract the spice name 4 characters at a time
	for (indx = 0; indx <= MAXNAMESIZE; indx = indx + 4)
	{
		// Store off 4 characters 
		*((uint32_t*)(recipe.Name + indx)) = readEeprom(offset);

		// Check for end of string signifying to stop writing
		if (recipe.Name[indx] == '\0')
		{
			break;
		}

		// Increment EEPROM Offset
		offset = offset + 1;
	}

	//*recipe.Name = *Read_NameEEProm(offset);

	// Reset Offset to Recipe Data Position
	offset = (number * RECBLKSIZE) + RECBLKADDR + 0x04;

	// Extract the recipe data 2 positions at a time
	for (indx = 0; indx <= MAXSLOTS; indx=indx+2)
	{
		*((uint32_t *) (recipe.Data + indx)) = readEeprom(offset);;
	
		// If the quantity is 0, assume that this is the
		// end of the recipe
		if (recipe.Data[indx].DataBits.quantity == 0)
		{
			break;
		}

		// Increment EEPROM Offset
		offset = offset + 1;
	}

	return recipe;
}

uint16_t Read_SpiceRemQty(uint8_t position)
{
	EEPROMDataBlockType data;
	SpiceDataType spicedata;
	// Divide Position by 2 to determine Word Offset
	uint16_t offset = position >> 1;

	data.FullWord = readEeprom(SPICEDATOFST + offset);

	// Determine the 16-bit offset and return the appropriate word
	if ((position & 0x01) == 0)
	{
		spicedata.As16BitWord = data.HalfWord.Lower16Bits;
	}
	else
	{
		spicedata.As16BitWord = data.HalfWord.Upper16Bits;
	}

	return spicedata.DataBits.quantity;
}

uint16_t Read_NumofRecipes(void)
{
	EEPROMDataBlockType data;

	data.FullWord = readEeprom(SPICEDATOFST + NUMOFRECOFST);

	return data.HalfWord.Lower16Bits;
}

uint8_t* Read_SpiceName(uint8_t position)
{
	uint16_t offset = 0;

	offset = (SPICENMOFST) + (position * 0x04);

	return Read_NameEEProm(offset);
}

uint16_t write_NameEEProm(uint16_t offset, uint8_t* name)
{
	EEPROMDataBlockType data;
	uint16_t indx = 0;
	uint16_t x = 0;
	uint16_t error = 0;
	uint8_t temp[4];
	uint8_t* ptr = name;
	bool endofstr = false;

	// Write the spice name 4 characters at a time
	for (indx = 0; (indx <= MAXNAMESIZE) && (endofstr == false); indx = indx + 4)
	{
		// Copy 4 characters at a time. Stop if Null
		for (x = 0; x < 4; x++)
		{
			temp[x] = *ptr;
			ptr++;

			if (temp[x] == '\0')
			{
				endofstr = true;
				break;
			}
		}

		// Write the data (Done this way mainly for debugging purposes)
		data.FullWord = *((uint32_t*)(temp));
		error = writeEeprom(offset, data.FullWord);

		// Check if there was a write error before continuing
		if (error != 0)
		{
			break;
		}

		// Increment EEPROM Offset
		offset = offset + 1;
	}

	return error;
}

uint16_t Write_SpiceRemQty(uint8_t position, uint16_t qty)
{
	EEPROMDataBlockType eeprom_data;
	SpiceDataType spice_data;
	uint16_t error = 0;

	// Divide Position by 2 to determine Word Offset
	uint16_t offset = position >> 1;

	spice_data.DataBits.position = position;
	spice_data.DataBits.quantity = qty;

	// Make a copy of the current 32-bit word in the EEPROM,
	// since spice data is only 16 bits and we do not want to overwrite
	// the other data.
	eeprom_data.FullWord = readEeprom(SPICEDATOFST + offset);

	// Determine the 16-bit offset and write to the appropriate word
	if ((position & 0x01) == 0)
	{
		eeprom_data.HalfWord.Lower16Bits = spice_data.As16BitWord;
	}
	else
	{
		eeprom_data.HalfWord.Upper16Bits = spice_data.As16BitWord;
	}

	// Write the Data to the EEPROM. (NOTE THIS IS A BLOCKING FUNCTION)
	error = writeEeprom(SPICEDATOFST + offset, eeprom_data.FullWord);

	// Return Error Flag if any
	return error;
}

uint16_t Write_Recipe(RecipeStructType recipe, uint8_t number, bool update)
{
	EEPROMDataBlockType data;
	uint16_t indx = 0;
	uint16_t offset = 0;
	uint16_t stored_num = 0;
	uint16_t error = 0;

	// Read number of currently stored recipes
	stored_num = Read_NumofRecipes();

	// Check if there is any more storage
	if (stored_num >= MAXNUMRECP)
	{
		return ERROROOM; // Return Out of Memory Error Code 
	}
	else
	{
		// Check if update flag was given (indicating an update and not a new write)
		if (update)
		{
			// Validate the Recipe Number is within range
			if (number >= MAXNUMRECP)
			{
				return ERRORINVALID; // Return Invalid Error Code
			}
		}
		// New Recipe Write
		else
		{
			number = stored_num - 1;
		}
	}

	// Calculate offset to the Recipe Block then write
	offset = (number * RECBLKSIZE) + RECBLKADDR;
	error = write_NameEEProm(offset, recipe.Name);

	// Write the remaining recipe number
	error = writeEeprom(SPICEDATOFST + NUMOFRECOFST, number);

	// Check if there was a write error before continuing
	if (error != 0)
	{
		return error;
	}

	// Reset Offset to Recipe Data Position
	offset = (number * RECBLKSIZE) + RECBLKADDR + 0x04;

	// Write the recipe data 2 positions at a time
	for (indx = 0; indx <= MAXSLOTS; indx = indx + 2)
	{
		// Copy the data at each 32-bit word offset then write
		data.FullWord = *((uint32_t*)(recipe.Data + indx));
		error = writeEeprom(offset, data.FullWord);

		// Check if there was a write error before continuing
		if (error != 0)
		{
			return error;
		}

		// If the quantity is 0, assume that this is the
		// end of the recipe
		if (recipe.Data[indx].DataBits.quantity == 0)
		{
			break;
		}

		offset = offset + 1;
	}

	return error;
}

uint16_t Write_SpiceName(uint8_t position, uint8_t *name)
{
	uint16_t offset = 0;
	uint16_t error = 0;

	offset = (SPICENMOFST)+(position * 0x04);

	error = write_NameEEProm(offset, name);

	return error;
}

uint16_t Update_RecipeName(uint8_t number, uint8_t *name)
{
	uint16_t offset = 0;
	uint16_t error = 0;

	offset = (number * RECBLKSIZE) + RECBLKADDR;

	error = write_NameEEProm(offset, name);

	return error;
}

uint16_t initSpiceData(void)
{
	EEPROMDataBlockType FirstPowerUp;
	EEPROMDataBlockType data;
	uint16_t pos = 0;
	uint16_t error = 0;
	uint16_t offset = 0;

	// Read the First PowerUp Flag
	FirstPowerUp.FullWord = readEeprom(SPICEINITOFST);

	// If the FirstPowerUp flag indicates first time (0)
	// initialize the EEPROM Spice Blocks using the defaults
	if (FirstPowerUp.FullWord == 0xFFFFFFFF)
	{
		for (pos = 0; pos < MAXSLOTS; pos++)
		{
			offset = pos >> 1;

			error = Write_SpiceName(pos, DefaultSpices[pos].name);

			if (error != 0)
			{
				break;
			}

			if ((pos & 0x01) == 0)
			{
				data.HalfWord.Lower16Bits = DefaultSpices[pos].data.As16BitWord;
			}
			else
			{
				data.HalfWord.Upper16Bits = DefaultSpices[pos].data.As16BitWord;
			}

			error = writeEeprom(SPICEDATOFST + offset, data.FullWord);
		}

		error = writeEeprom(SPICEDATOFST + NUMOFRECOFST, 0);
	}

	return error;
}
