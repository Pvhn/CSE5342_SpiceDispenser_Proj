/* =======================================================
 * File Name: UICONTROL
 * =======================================================
 * File Description: Header File for UICONTROL.c
 * =======================================================
 */

#ifndef UICONTROL_H_
#define UICONTROL_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

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
extern char SpiceList[MAXSLOTS][MAXNAMESIZE];
extern char RecipeList[MAXNUMRECP][MAXNAMESIZE];

/*========================================================
* Function Declarations
*========================================================
*/

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
extern void saveRecipe(USER_DATA* data);
extern void initSpiceList(void);
extern void initRecipeList(void);


#endif /* UICONTROL_H_ */
