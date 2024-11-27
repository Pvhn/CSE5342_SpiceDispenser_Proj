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

// Default Spices
SpiceStructType DefaultSpices[MAXSLOTS] =
{
	{"SALT", 0x300},
	{"BLACKPEPPER", 0x301},
	{"GARLIC", 0x302},
	{"PAPRIKA", 0x303},
	{"ONION", 0x304},
	{"OREGANO", 0x305},
	{"THYME", 0x306},
	{"ROSEMARY", 0x307},
};

// Recipes for testing EEPROM
RecipeStructType test_recipes[3] =
{
	{"CAJUN", {0x41, 0x12}},
	{"ITALIAN", {0x63, 0x24, 0x55, 0x16}},
	{"5SPICE", {0x27, 0x10, 0x42}}
};

/*========================================================
 * Function Declarations
 *========================================================
 */

//Forward Declaration since we don't this to be used outside of this library
uint16_t Write_NameEEProm(uint16_t offset, uint8_t* name);
uint8_t* Read_NameEEProm(uint16_t offset);

/*=======================================================
 * Function Name: Read_NameEEProm
 *=======================================================
 * Parameters: offset
 * Return: name
 * Description:
 * This helper function is used to read string type data that
 * has been stored in the EEPROM. A pointer to the
 * read string is passed back to the calling function
 *=======================================================
 */
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

uint8_t* Read_RecipeName(uint8_t number)
{
    uint16_t offset = 0;
    uint8_t num_of_stored_rec = Read_NumofRecipes();
    offset = (number * RECBLKSIZE) + RECBLKADDR;

    // Validate the provided position
    if (number > num_of_stored_rec)
    {
        return (uint8_t *) ERRORINVALID;
    }

    // Return pointer to the string
    return Read_NameEEProm(offset);
}

/*=======================================================
 * Function Name: Read_NameEEProm
 *=======================================================
 * Parameters: number
 * Return: recipe
 * Description:
 * This function is used to read a recipe that has been
 * stored in the EEPROM. A struct which contains the 
 * recipe name and the various spices in it is returned
 * to the calling function. 
 *=======================================================
 */
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

	for (indx = 0; (indx < MAXNAMESIZE) && (endofstr == false); indx = indx + 4)
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

	// Reset Offset to Recipe Data Position
	offset = (number * RECBLKSIZE) + RECBLKADDR + 0x04;

	// Extract the recipe data 2 positions at a time
	for (indx = 0; indx < MAXSLOTS; indx=indx+2)
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

/*=======================================================
 * Function Name: Read_NameEEProm
 *=======================================================
 * Parameters: position
 * Return: quantity or error
 * Description:
 * This function is used to read the remaining quantity
 * of a given spice position. If an invalid position
 * was provided, an "Invalid" Error code is returned.
 *=======================================================
 */
uint16_t Read_SpiceRemQty(uint8_t position)
{
	EEPROMDataBlockType data;
	SpiceDataType spicedata;
	// Divide Position by 2 to determine Word Offset
	uint16_t offset = position >> 1;

	data.FullWord = readEeprom(SPICEDATOFST + offset);

	// Validate the position is within range
	if (position > MAXSLOTS - 1)
	{
		return ERRORINVALID;
	}

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

/*=======================================================
 * Function Name: Read_NumofRecipes
 *=======================================================
 * Parameters: None
 * Return: NumofRecipes
 * Description:
 * This function is used to read the amount of stored
 * recipes in the system.
 *=======================================================
 */
uint16_t Read_NumofRecipes(void)
{
	EEPROMDataBlockType data;

	data.FullWord = readEeprom(SPICEDATOFST + NUMOFRECOFST);

	return data.HalfWord.Lower16Bits;
}

/*=======================================================
 * Function Name: Read_SpiceName
 *=======================================================
 * Parameters: position
 * Return: *name or error
 * Description:
 * This function is used to read the stored spice name
 * at a given position. A pointer to the name is returned
 * If an invalid position is given an "Invalid" Error 
 * code will be returned instead.
 *=======================================================
 */
uint8_t* Read_SpiceName(uint8_t position)
{
	uint16_t offset = 0;

	offset = (SPICENMOFST) + (position * 0x04);

	// Validate the provided position
	if (position > MAXSLOTS - 1)
	{
		return (uint8_t *) ERRORINVALID;
	}

	// Return pointer to the string
	return Read_NameEEProm(offset);
}



/*=======================================================
 * Function Name: Write_NameEEProm
 *=======================================================
 * Parameters: position, *name
 * Return: error
 * Description:
 * This helper function is used to write string type
 * data to the EEPROM. Arguments are an offset for
 * where the string is to be written and a pointer
 * to the string that is to be stored. In the event
 * of some EEPROM write error, an EEPROM error code
 * will be returned.
 *=======================================================
 */
uint16_t Write_NameEEProm(uint16_t offset, uint8_t* name)
{
	EEPROMDataBlockType data;
	uint16_t indx = 0;
	uint16_t x = 0;
	uint16_t error = 0;
	uint8_t temp[4];
	uint8_t* ptr = name;
	bool endofstr = false;

	// Write the spice name 4 characters at a time
	for (indx = 0; (indx < MAXNAMESIZE) && (endofstr == false); indx = indx + 4)
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

/*=======================================================
 * Function Name: Write_SpiceRemQty
 *=======================================================
 * Parameters: position, qty
 * Return: error
 * Description:
 * This function is used to write or update the remaining
 * quantity of a spice at the given position. If an
 * invalid position is given, an "Invalid" error code
 * will be returned. In the event of some EEPROM write 
 * error, an EEPROM error code will be returned.
 *=======================================================
 */
uint16_t Write_SpiceRemQty(uint8_t position, uint16_t qty)
{
	EEPROMDataBlockType eeprom_data;
	SpiceDataType spice_data;
	uint16_t error = 0;

	// Limit the max quantity
	if (qty > MAXQTY)
	{
		qty = MAXQTY;
	}

	// Divide Position by 2 to determine Word Offset
	uint16_t offset = position >> 1;

	spice_data.DataBits.position = position;
	spice_data.DataBits.quantity = qty;

	// Make a copy of the current 32-bit word in the EEPROM,
	// since spice data is only 16 bits and we do not want to overwrite
	// the other data.
	eeprom_data.FullWord = readEeprom(SPICEDATOFST + offset);

	if (position > MAXSLOTS - 1)
	{
		return ERRORINVALID;
	}

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

/*=======================================================
 * Function Name: Write_Recipe
 *=======================================================
 * Parameters: recipe
 * Return: error
 * Description:
 * This function is used to write a new recipe at the
 * next available position in the EEPROM. If a specific
 * recipe number is to be updated or written to, the
 * Write_RecipeX should be used instead. See
 * Write_RecipeX description for information on
 * error handling
 *=======================================================
 */
uint16_t Write_Recipe(RecipeStructType recipe)
{
	uint16_t error = 0;
	error = Write_RecipeX(recipe, 0xDEAD);

	return error;
}

/*=======================================================
 * Function Name: Write_RecipeX
 *=======================================================
 * Parameters: recipe,  number
 * Return: error
 * Description:
 * This function is used to write a new recipe to the
 * EEPROM. A number may be provided to specify a specific
 * recipe to be updated. The function will verify that
 * there is still enough storage left in the EEPROM
 * to allocate the recipe. The function will return 
 * an error code if any error occurs. This includes
 * "Out of Memory" errors if the max amount of recipes
 * have been reached, an "Invalid" error code if an
 * invalid recipe number was given, or an EEPROM error
 * code if there was an issue writing to the EEPROM.
 *=======================================================
 */
uint16_t Write_RecipeX(RecipeStructType recipe, uint16_t number)
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
		// Check if a specific recipe number was given and validate
		if (number != 0xDEAD)
		{
			// Validate the Recipe Number is within range
			if (number >= MAXNUMRECP)
			{
				return ERRORINVALID; // Return Invalid Error Code
			}
		}
		// No specific number given, write to next allowed position
		else
		{
			number = stored_num;
		    // Write the remaining recipe number
		    error = writeEeprom(SPICEDATOFST + NUMOFRECOFST, number+1);
		}
	}

	// Calculate offset to the Recipe Block then write
	offset = (number * RECBLKSIZE) + RECBLKADDR;
	error = Write_NameEEProm(offset, recipe.Name);

	// Check if there was a write error before continuing
	if (error != 0)
	{
		return error;
	}

	// Reset Offset to Recipe Data Position
	offset = (number * RECBLKSIZE) + RECBLKADDR + 0x04;

	// Write the recipe data 2 positions at a time
	for (indx = 0; indx < MAXSLOTS; indx = indx + 2)
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

/*=======================================================
 * Function Name: Write_SpiceName
 *=======================================================
 * Parameters: position, name
 * Return: error
 * Description:
 * This function writes the given spice name to the
 * given position. An EEPROM error code is returned if
 * there was an issue writing to the EEPROM.
 *=======================================================
 */
uint16_t Write_SpiceName(uint8_t position, uint8_t *name)
{
	uint16_t offset = 0;
	uint16_t error = 0;

	offset = (SPICENMOFST)+(position * 0x04);

	error = Write_NameEEProm(offset, name);

	return error;
}

/*=======================================================
 * Function Name: Update_RecipeName
 *=======================================================
 * Parameters: number, name
 * Return: error
 * Description:
 * This function updates the name of a recipe.
 * An error code is returned if there was an issue writing
 * to the EEPROM.
 *=======================================================
 */
uint16_t Update_RecipeName(uint8_t number, uint8_t *name)
{
	uint16_t offset = 0;
	uint16_t error = 0;

	offset = (number * RECBLKSIZE) + RECBLKADDR;

	error = Write_NameEEProm(offset, name);

	return error;
}

/*=======================================================
 * Function Name:initSpiceData
 *=======================================================
 * Parameters: None
 * Return: error
 * Description:
 * This function initializes the Spice Data Blocks
 * in the EEPROM with the default spices and quantities
 * when it is the first time the system has powered on. 
 *=======================================================
 */
uint16_t initSpiceData(void)
{
	EEPROMDataBlockType FirstPowerUp;
	EEPROMDataBlockType data;
	uint16_t pos = 0;
	uint16_t error = 0;
	uint16_t offset = 0;

	// Read the First PowerUp Flag
	FirstPowerUp.FullWord = readEeprom(SPICEINITOFST);

	// If the FirstPowerUp flag doesn't match key
	// initialize the EEPROM Spice Blocks using the defaults
	if (FirstPowerUp.FullWord != 0xBEEF)
	{
		// Write Init Key value for next power up state.
		error = writeEeprom(SPICEINITOFST, 0xBEEF);
		
		// Initialize each of the spice positions
		for (pos = 0; pos < MAXSLOTS; pos++)
		{
			offset = pos >> 1;

			// Write the Spice name and check for error. Abort if error.
			error = Write_SpiceName(pos, DefaultSpices[pos].name);
			if (error != 0)
			{
				break;
			}

			// Store the data to the appropriate upper or lower 16-bit word
			if ((pos & 0x01) == 0)
			{
				data.HalfWord.Lower16Bits = DefaultSpices[pos].data.As16BitWord;
			}
			else
			{
				data.HalfWord.Upper16Bits = DefaultSpices[pos].data.As16BitWord;
			}

			// Write the spice data and check for error. Abort if error
			error = writeEeprom(SPICEDATOFST + offset, data.FullWord);
			if (error != 0)
			{
				break;
			}
		}

		// Initialize number of recipes to 0.
		error = writeEeprom(SPICEDATOFST + NUMOFRECOFST, 0);
	}

	return error;
}

/*=======================================================
 * Function Name:TestEEPROM
 *=======================================================
 * Parameters: None
 * Return: None
 * Description:
 * This function is for debugging purposes and is used
 * to verify functionality of the EEPROM.
 *=======================================================
 */
void TestEEPROM(void)
{
	int x = 0;
	RecipeStructType recipe;
	uint16_t error = 0;

	// Initialize number of recipes to 0.
	error = writeEeprom(SPICEDATOFST + NUMOFRECOFST, 0);

	for (x = 0; x < 3; x++)
	{
		error = Write_Recipe(test_recipes[x]);
		recipe = Read_Recipe(x);
	}
}
