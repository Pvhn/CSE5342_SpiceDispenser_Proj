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

/*====================================================================
 * Function Name: UIRackHome
 *====================================================================
 * Parameters: None
 * Return: None
 * Description:
 * Function for performing the actions of the UI command "home". 
 * Function will perform a call to the StepRackHome motor control
 * function and will notify the user if there was an error in homing.
 *====================================================================
 */
extern void UIRackHome(void);

/*====================================================================
 * Function Name: refillSpice
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will update the stored quantity of a given spice. 
 * The function first validates that the given spice name exists. 
 * If no existing spice is found, the user is notified and the 
 * action is aborted. The function will also validate that the entered
 * quantity is less than the max allowed quantity. If the entered amount
 * is greater than allowed the function will notify the user. However,
 * the action will not be aborted. Instead, it will assume that the spice
 * was filled to the max quantity. Once all parameters are validated,
 * the function will perform a call to updated the remaining
 * quantity in the EEPROM.
 *====================================================================
 */
extern void refillSpice(USER_DATA* data);

/*====================================================================
 * Function Name: changeSpice
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will accept user commands to update the name of a currently
 * stored spice. The function will first prompt the user to enter
 * a valid slot number. It will then validate that the slot number 
 * entered is within range. If the number entered is not within range
 * the function will prompt the user to try again. This will repeat
 * until a valid number is entered or the user enters "cancel".
 * Once a valid number is provided, the function will confirm with
 * the user by displaying the currently associated spice in the slot.
 * If the user chooses to continue, the function will move onto the
 * next step and prompt the user to enter the new spice name. The 
 * entered name is validated to ensure it meets the specified
 * requirements. If an invalid name is entered the function will 
 * continue to prompt the user to try-again until a valid name is entered.
 * Once a valid name is entered the function will perform a call 
 * to the EEPROM to update the name. Once the name is saved, the 
 * function will prompt the user if they would also like to update
 * the spice quantity. If the user chooses to do so, the steps are
 * performed are the same as those discussed in the refillSpice function.
 *====================================================================
 */
extern void changeSpice(USER_DATA* data);

/*====================================================================
 * Function Name: deleteRecipe
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function will accept user commands to delete a recipe from the 
 * EEPROM and program memory.The function will first validate that
 * the provided name argument exists against a known list of recipes.
 * If the recipe does not exists, an error is indicated to the user
 * and the action is aborted. If the name does exists, the function 
 * will then prompt the user to confirm that they are about to delete
 * a recipe. The user must enter "delete" to complete the action.
 * If any other entry is entered the action will be aborted.
 * Once deletion is confirmed, the function will then begin by
 * deleting the requested recipe. It will then copy the recipe
 * below it by reading the contents from the EEPROM, and then
 * writing the contents back in the index above. This is repeated for
 * each stored recipe below the deleted recipe.
 *====================================================================
 */
extern void deleteRecipe(USER_DATA* data);

/*====================================================================
 * Function Name: resetSystem
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function performs a hard reset of the EEPROM. This will remove
 * all stored recipes in the EEPROM as well as reset the System Spice
 * Data back to the default values.
 * The function will prompt the user to confirm deletion by
 * entering the reset key "RESET_SYSTEM_X342". If an invalid key
 * is entered the action is aborted. On a valid key entry, the function
 * reinitialize the EEPROM with the default spice data and then
 * prompts the user to restart the system. As the reset is
 * a partially destructive operation, usage of this function will
 * result in the program to enter an infinite loop. This is done
 * on purpose to force the user to reset the system.
 *====================================================================
 */
extern void resetSystem(USER_DATA* data);

/*====================================================================
 * Function Name: viewItems
 *====================================================================
 * Parameters: data
 * Return: None
 * Description:
 * Function performs actions for the "view" command. The function
 * will determine if the secondary argument was spices or recipes.
 * If the argument does not match either, an error is indicated
 * to the user and no further action is taken. If spices was indicated,
 * the function will perform a call to display the list
 * of known spices and their quantities to the user.
 * If recipes was indicated, the function will perform a call to
 * display the list of known recipes to the user.
 *====================================================================
 */
extern void viewItems(USER_DATA* data);

/*====================================================================
 * Function Name: displaySpices
 *====================================================================
 * Parameters: None
 * Return: None
 * Description:
 * Function for printing a list of known spices and their quantites
 * to the UART interface.
 *====================================================================
 */
extern void displaySpices(void);

/*====================================================================
 * Function Name: displayRecipes
 *====================================================================
 * Parameters: None
 * Return: None
 * Description:
 * Function for printing a list of known recipes
 * to the UART interface.
 *====================================================================
 */
extern void displayRecipes(void);

extern void calibrate(USER_DATA* data);



#endif /* UICONTROL_H_ */
