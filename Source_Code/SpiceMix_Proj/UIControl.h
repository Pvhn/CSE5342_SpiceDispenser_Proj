/* =======================================================
 * File Name: UICONTROL
 * =======================================================
 * File Description: Header File for UICONTROL.c
 * =======================================================
 */

#ifndef UICONTROL_H_
#define UICONTROL_H_



#include "System.h"
#include "eepromControl.h"
#include "parsing.h"


/*========================================================
 * Preprocessor Declarations
 *========================================================
 */
#define ERRORMATCH 255

/*========================================================
* Variable Declarations
*========================================================
*/

// Spice Dictionary (Initialized with EEPROM valuues on start-up)
// See initSpiceList()
extern char SpiceList[MAXSLOTS][MAXNAMESIZE];

// Recipe Dictionary (Initialized with EEPROM valuues on start-up)
// See initRecipeList()
extern char RecipeList[MAXNUMRECP][MAXNAMESIZE];

/*========================================================
* Function Declarations
*========================================================
*/

char* rusty_itoa(uint16_t num);

/*=======================================================
 * Function Name: getUserInput
 *=======================================================
 * Parameters: data
 * Return: None
 * Description:
 * Helper function for executing and obtaining a user
 * input from the UART. Data is a pointer to the data
 * buffer struct.
 *=======================================================
 */
extern void getUserInput(USER_DATA* data);

/*=======================================================
 * Function Name: dispenseSpice
 *=======================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will dispense a requested spice. Data is
 * a pointer to the UART buffer. The function will
 * validate the requested spice against the list of known
 * stored spices. If no existing spice is found, the
 * user is notified and the action is aborted. The function
 * will also validate that the spice has enough to dispense
 * the requested quantity. If an insufficient amount
 * is detected, the action is aborted. If all checks are
 * validated, the Dispense sequence is executed and
 * the spice quantity is updated in the EEPROM.
 *=======================================================
 */
extern void dispenseSpice(USER_DATA* data);

/*=======================================================
 * Function Name: dispenseRecipe
 *=======================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will dispense a requested recipe. Data is
 * a pointer to the UART buffer. The function will
 * validate the requested recipe against the list of known
 * stored recipes. If no existing recipe is found, the
 * user is notified and the action is aborted. The function
 * will also validate that there is enough of each spice
 * to dispense. If an insufficient amount
 * is detected, the action is aborted. If all checks are
 * validated, the Dispense sequence is executed and
 * the spice quantity is updated in the EEPROM.
 *=======================================================
 */
extern void dispenseRecipe(USER_DATA* data);

/*=======================================================
 * Function Name: printRecipe
 *=======================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will display the elements of a given recipe.
 * Data is a pointer to the UART buffer. The function
 * will validate the given recipe against a list of
 * known recipes. If no match is found the user is notified
 * and the action is aborted.
 *=======================================================
 */
extern void printRecipe(USER_DATA* data);

/*====================================================================
 * Function Name: saveRecipe
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will save/update a given recipe. The function first checks
 * that the given recipe exists. If it does, the user is prompted to
 * either continue writing to the recipe (aka update the recipe) or
 * cancel by entering "cancel". If the recipe does not currently exists 
 * or the user chooses to continue the function will enter a while loop
 * to prompt the user to enter spice data for the recipe. This will
 * continue to prompt the user for entries until either the user
 * enters "done" or the max amount of spice slots is entered. The user
 * may also enter "cancel" to abort the process entirely. With each
 * spice entry, the entry is checked that it exists in the known spice list
 * and that a valid quantity was provided. If the user enters a spice
 * twice, the most recent entry will take precedence. Once all entries
 * have been entered the recipe is saved (or updated) in the EEPROM.
 * In the event there is an issue writing to the EEPROM, the user is
 * notified.
 *====================================================================
 */
extern void saveRecipe(USER_DATA* data);

/*====================================================================
 * Function Name: initSpiceList
 *====================================================================
 * Parameters: None
 * Return: None
 * Description:
 * Function will read the EEPROM and initialize the user-interface
 * spice list dictionary. The SpiceList dictionary is used for displaying
 * the stored spices to the user as well as for searching and validating
 * recipe data.
 *====================================================================
 */
extern void initSpiceList(void);

/*====================================================================
 * Function Name: initRecipeList
 *====================================================================
 * Parameters: None
 * Return: None
 * Description:
 * Function will read the EEPROM and initialize the user-interface
 * recipe list dictionary. This RecipeList dictionary is used for displaying
 * the stored recipes to the user as well as for validating and updating
 * existsing recipes.
 *====================================================================
 */
extern void initRecipeList(void);

extern void UIRackHome(void);


extern void refillSpice(USER_DATA* data);

extern void changeSpice(USER_DATA* data);

extern void deleteRecipe(USER_DATA* data);

extern void resetSystem(USER_DATA* data);

extern void viewItems(USER_DATA* data);

extern void displaySpices(void);

extern void displayRecipes(void);

extern void calibrate(USER_DATA* data);



#endif /* UICONTROL_H_ */
