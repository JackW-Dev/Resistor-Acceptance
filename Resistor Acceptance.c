/* 	
Author : Jack Walker
Company : Staffordshire University
Date :	10th April 2019
Function: Resistor Analysis
Version 2.0
Modifications : Added data logging and review functionality
Software : Dev C/C++	
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct shipmentInfo {
    char company[18], date[8];
    double nominalValue, tolerance, meanResistance;
    float failureRate, standDevResistance, varianceResistance;
};
typedef struct shipmentInfo data;
int operation();
int menu();
void dateInput(data*);
int toInt(char[]);
int supplierSelect(const char**);
void fourBands(const char**, const char**, const char**, int, int, int, data*);
void fiveBands(const char**, const char**, const char**, int, int, int, data*);
void sixBands(const char**, const char**, const char**, const char**, int, int, int, int, data*);
int bandInput(const char**, int, int);
double actualValInp(int);
double idealVal(int, int, int, int, int);
double multVal(int);
double tolVal(int);
int tempVal(int);
double sampleMean(double[]);
float sampleStandDev(double[], double);
double minTolVal(double, float);
double maxTolVal(double, float);
FILE* fileHandling();

/* 
Purpose: Resistor Analysis
    - Input a supplier name and date of shipment
    - Select the amount of bands the resistor has (4, 5 or 6)
    - Input the band colours that the batch consists of
    - Input the actual ohm resistance value for each resistor in the sample
    - Calculate the mean, standard deviation and variance of these inputs
    - Calculate the nominal value based on the colours input by the user
    - Using the tolerance, compare the actual resistance values to the nominal (using the tolerance as min and max values)
    - Record the failure rate of the batch
    - Output this data from a structure
    - Store this data to a log (user selected text file)
    - Output the log
    - Output log items with a specified supplier
Main variables:
    const char* suppliers[] - Constant array of character arrays (strings) for supplier names
    const char* bandColours[] - Constant array of character arrays (strings) for band colours
    const char* multiplierColours[] - Constant array of character arrays (strings) for multiplier colours
    const char* toleranceColours[] - Constant array of character arrays (strings) for tolerance colours
    const char* temperatureColours[] - Constant array of character arrays (strings) for temperature colours
    data output - Structure storing the date, supplier name, nominal value, tolerance, mean, standard deviation, variance and failure rate for the sample			
Functions:
    supplierSelect() - Menu system for selecting the supplier that the batch is from
    dateInput() - Input system for the date
    menu() - Menu screen for selecting the amount of bands the resistor has
    fourBands(), fiveBands() & sixBands() - Record the colours of the batch's bands through a text based selection
        bandInput() - Request an input based on the values passed to the function
        idealVal() - Calculate the nominal value based on the input provided
    actualValInp() - Input the actual resistance for each of the 10 resistors in the sample
    minTolVal() & maxTolVal() - Calculate the minimum and maximum acceptable resistance values
    sampleMean() - Calculates the mean of the sample
    sampleStandDev() - Calculates the standard deviation of the sample
 */

void main() {
    const char* suppliers[4] = {"Farnell", "RSComponents", "Rapid Electronics", "DigiKey"};
    const char* bandColours[10] = {"Black", "Brown", "Red", "Orange", "Yellow", "Green", "Blue", "Violet", "Grey", "White"};
    const char* multiplierColours[10] = {"Silver", "Gold", "Black", "Brown", "Red", "Orange", "Yellow", "Green", "Blue", "Violet"};
    const char* toleranceColours[7] = {"Silver", "Gold", "Brown", "Red", "Green", "Blue", "Violet"};
    const char* temperatureColours[4] = {"Brown", "Red", "Orange", "Yellow"};
    int i, fail = 0, trigger = 0, index;
    double arr[10];
    data output;
    FILE *fp;
    char fileName[30], buffer[30];

    switch (operation()) {
        case 1:
            strcpy(output.company, suppliers[supplierSelect(suppliers) - 1]);
            dateInput(&output);
            output.failureRate = 0;
            switch (menu()) {
                case 1:
                    fourBands(bandColours, multiplierColours, toleranceColours, 10, 10, 7, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(sampleStandDev(arr, sampleMean(arr)));
                    output.failureRate = fail;
                    break;
                case 2:
                    fiveBands(bandColours, multiplierColours, toleranceColours, 10, 10, 7, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(output.varianceResistance);
                    output.failureRate = fail;
                    break;
                case 3:
                    sixBands(bandColours, multiplierColours, toleranceColours, temperatureColours, 10, 10, 7, 4, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(output.varianceResistance);
                    output.failureRate = fail;
                    break;
                case 4:
                    exit(0);
            }
            printf("Company - %s\nDate - %s\nFailure Rate - %f\n", output.company, output.date, output.failureRate);
            printf("Nominal Value - %lf\nTolerance - %f\n", output.nominalValue, output.tolerance);
            printf("Mean Resistance - %lf\nStandard Deviation - %lf\nVariance - %lf\n", output.meanResistance, output.standDevResistance, output.varianceResistance);
            break;
        case 2:
            printf("Please input the name of the text file for the application to use (if the file can't be found, one will be created using that name): ");
            scanf("%s", &fileName);
            strcat(fileName, ".txt");
            fp = fopen(fileName, "a");
            strcpy(output.company, suppliers[supplierSelect(suppliers) - 1]);
            dateInput(&output);
            output.failureRate = 0;
            switch (menu()) {
                case 1:
                    fourBands(bandColours, multiplierColours, toleranceColours, 10, 10, 7, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(sampleStandDev(arr, sampleMean(arr)));
                    output.failureRate = fail;
                    break;
                case 2:
                    fiveBands(bandColours, multiplierColours, toleranceColours, 10, 10, 7, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(output.varianceResistance);
                    output.failureRate = fail;
                    break;
                case 3:
                    sixBands(bandColours, multiplierColours, toleranceColours, temperatureColours, 10, 10, 7, 4, &output);
                    for (i = 0; i < 10; i++) {
                        arr[i] = actualValInp(i);
                        if ((arr[i] > maxTolVal(output.nominalValue, output.tolerance)) || (arr[i] < minTolVal(output.nominalValue, output.tolerance))) {
                            fail += 10;
                        }
                    }
                    output.meanResistance = sampleMean(arr);
                    output.standDevResistance = sampleStandDev(arr, output.meanResistance);
                    output.varianceResistance = sqrt(output.varianceResistance);
                    output.failureRate = fail;
                    break;
                case 4:
                    exit(0);
            }
            fprintf(fp, "%s\n%s\n%f\n%lf\n%f\n%lf\n%lf\n%lf\n", output.company, output.date, output.failureRate, output.nominalValue, output.tolerance, output.meanResistance, output.standDevResistance, output.varianceResistance);
            fclose(fp);
            break;
        case 3:
            fp = fileHandling();
            printf("%-20s\t%-8s\t%-3s\t%-20s\t%-11s\t%-16s\t%-8s\t%-8s\n", "Company", "Date", "Failure Rate (%)", "Nominal Value (Ohms)", "Tolerance", "Mean (Ohms)", "Standard Deviation", "Variance");
            while (fgets(buffer, 30, fp) != NULL) {
                strtok(buffer, "\n"); /*Removes a token from a string and returns a pointer to the new form, in this case removing newline*/              	
                printf("%-20s\t", buffer);
                fgets(buffer, 30, fp);
				strtok(buffer, "\n");        
                printf("%-8s\t", buffer);
                fgets(buffer, 30, fp);
				strtok(buffer, "\n");               
                printf("%-3s\t", buffer);
                fgets(buffer, 30, fp);
				strtok(buffer, "\n");               
                printf("\t%-20s\t", buffer);
                fgets(buffer, 30, fp);
				strtok(buffer, "\n");                 
                printf("%-9s\t", buffer);
                fgets(buffer, 30, fp);
				strtok(buffer, "\n");               
                printf("%-16s\t", buffer);
                fgets(buffer, 30, fp);
                strtok(buffer, "\n");
                printf("%-8s\t", buffer);
            	fgets(buffer, 30, fp);
            	strtok(buffer, "\n");
                printf("\t%-8s\n", buffer);  				                   
                }     
            fclose(fp);            
            break;
        case 4:
            fp = fileHandling();
            index = supplierSelect(suppliers) - 1;
            printf("%-20s\t%-8s\t%-3s\t%-20s\t%-11s\t%-16s\t%-8s\t%-8s\n", "Company", "Date", "Failure Rate (%)", "Nominal Value (Ohms)", "Tolerance", "Mean (Ohms)", "Standard Deviation", "Variance");
            while (fgets(buffer, 30, fp) != NULL) {
                strtok(buffer, "\n"); /*Removes a token from a string and returns a pointer to the new form, in this case removing newline*/
                if (strcmp(buffer, suppliers[index]) == 0) {                	
                    printf("%-20s\t", buffer);
                    fgets(buffer, 30, fp);
					strtok(buffer, "\n");        
                    printf("%-8s\t", buffer);
                    fgets(buffer, 30, fp);
					strtok(buffer, "\n");               
                    printf("%-3s\t", buffer);
                    fgets(buffer, 30, fp);
					strtok(buffer, "\n");               
                    printf("\t%-20s\t", buffer);
                    fgets(buffer, 30, fp);
					strtok(buffer, "\n");                 
                    printf("%-9s\t", buffer);
                    fgets(buffer, 30, fp);
					strtok(buffer, "\n");               
                    printf("%-16s\t", buffer);
                    fgets(buffer, 30, fp);
                    strtok(buffer, "\n");
                    printf("%-8s\t", buffer);
                	fgets(buffer, 30, fp);
                	strtok(buffer, "\n");
                    printf("\t%-8s\n", buffer);  
					trigger = 1;                        
                    }                   
                else if (trigger = 1) {
                    trigger = 0;
        		}           
            }
            fclose(fp);
            break;
        case 5:
            exit(0);
    }

}

FILE* fileHandling(){
	/*
    Name: FileHandling
    Function: To allow the user to input a file name and open it if valid, returning an error message and exiting if it is not
    Variables: fp, fileName
    Function will only return the file pointer if the file is found, otherwise the program will report this issue and exit processing
     */
	FILE *fp;
	char fileName[30];	
	printf("Please input the name of the text file for the application to use: ");
            scanf("%s", &fileName);
            strcat(fileName, ".txt");
            fp = fopen(fileName, "r");
            if (fp == NULL) {
                printf("File not found. Exiting program.");
                exit(0);
            }else{
            	return fp;
			} 
}

int operation() {
    /*
    Name: Operation
    Function: To provide a validated menu system for selecting the operation wanted by the user
    Variables: Choice, Valid, valueRead, followChar
    Function will only return the value of choice when valid = 1, otherwise it will loop until valid input is given
     */
    int choice, valid = 0, valueRead;
    char followChar;

    do {
        printf("======================================================\n");
        printf("1 - Input resistor batch\n2 - Input batch and store data in log\n3 - View data log\n4 - View data log (flitered by supplier)\n5 - Exit\n");
        printf("======================================================\n\r");
        valueRead = scanf("%d%c", &choice, &followChar);
        if (valueRead == 2) {
            if (isspace(followChar) && choice > 0 && choice < 6) {
                /*Integer followed by whitespace*/
                valid = 1;
            } else {
                /*Integer followed by character*/
                printf("Only defined integer values will be accepted, please try again.\n\r");
                fflush(stdin);
            }
        } else if (valueRead == 1 && choice > 0 && choice < 6) {
            /*Integer followed by nothing*/
            valid = 1;
        } else {
            /*Not an integer*/
            printf("Only defined integer values will be accepted, please try again.\n\r");
            fflush(stdin);
        }
    } while (valid == 0);
    if (choice == 1 || choice == 2 || choice == 3 || choice == 4) {
        return choice;
    } else {
        exit(0);
    }
}

int menu() {
    /*
    Name: Menu
    Function: To provide a validated menu system for selecting the amount of bands on the resistor batch being input
    Variables: choice, valid, valueRead, followChar
    Function will only return the value of choice when valid = 1, otherwise it will loop until valid input is given
     */
    int choice, valid = 0, valueRead;
    char followChar;

    do {
        printf("======================================================\n");
        printf("1 - 4 Band Resistors\n2 - 5 Band Resistors\n3 - 6 Band Resistors\n4 - Exit Program\n");
        printf("======================================================\n\r");
        valueRead = scanf("%d%c", &choice, &followChar);
        if (valueRead == 2) {
            if (isspace(followChar) && choice > 0 && choice < 4) {
                /*Integer followed by whitespace*/
                valid = 1;
            } else {
                /*Integer followed by character*/
                printf("Only defined integer values will be accepted, please try again.\n\r");
                fflush(stdin);
            }
        } else if (valueRead == 1 && choice > 0 && choice < 4) {
            /*Integer followed by nothing*/
            valid = 1;
        } else {
            /*Not an integer*/
            printf("Only defined integer values will be accepted, please try again.\n\r");
            fflush(stdin);
        }
    } while (valid == 0);
    if (choice == 1 || choice == 2 || choice == 3) {
        return choice;
    } else {
        exit(0);
    }
}

void dateInput(data* q) {
    /*
    Name: dateInput
    Function: To allow the user to input a date as a string, validate the input and return the string to a structure if valid, looping if not
    Paramaters: data* q - A pointer to a structure of type data
    Variables: daysPerMonth[], valid, d, m, y, dateInt
    Function will prompt the user to input a date in the form ddMMyyyy.
    This input will then be validated by being cast to an integer using the toInt() function, after which the integer is checked to ensure the date is valid
    If valid, the string form is set to the date of the data structure, else, the function loops
     */
    int daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int valid = 0, d, m, y, dateInt;
    char dateArr[8];
    while (valid != 1) {
        fflush(stdin);
        printf("Please input the date in form ddMMyyyy (e.g. 07062020 is 7 June 2020): ");
        scanf("%s", dateArr);
        dateInt = toInt(dateArr);
        d = dateInt / 1000000;
        m = dateInt / 10000 - d * 100;
        y = dateInt - d * 1000000 - m * 10000;
        if (y % 400 == 0 || (y % 100 != 0 && y % 4 == 0)) {
            daysPerMonth[1] = 29;
        }
        if (d < 1 || daysPerMonth[m - 1] < d || m < 1 || m > 12) {
             printf("Invalid date, please try again (format ddMMyyyy)\n\r");
        } else {
            valid = 1;
        }
    }
    strcpy(q->date, dateArr);
}

int toInt(char a[]) {
    /*
    Name: toInt
    Function: Convert a string value to an integer
    Paramaters: char a[] - Array of characters
    Variables: i, num
    Function will convert a character array to an integer and return it (Validation of char array occurs in dateInput so isn't needed here)
     */
    int i, num;
    num = 0;

    for (i = 0; a[i] != '\0'; i++) {
        num = num * 10 + a[i] - '0';
    }
    return num;
}

int supplierSelect(const char** suppliers) {
    /*
            Name: supplierSelect
            Function: Provide a menu system for selecting the supplier whose batch is being sampled
            Paramaters: const char** suppliers - Constant array of strings (passed as a pointer)
            Variables: choice, valid, valueRead, followChar
            Function will only return the value of choice when valid = 1, otherwise it will loop until valid input is given
     */
    int choice, valid = 0, valueRead;
    char followChar;

    do {
        printf("======================================================\n");
        printf("1 - %s\n2 - %s\n3 - %s\n4 - %s\n5 - Exit\n", suppliers[0], suppliers[1], suppliers[2], suppliers[3]);
        printf("======================================================\n\r");
        valueRead = scanf("%d%c", &choice, &followChar);
        if (valueRead == 2) {
            if (isspace(followChar) && choice > 0 && choice < 6) {
                /*Integer followed by whitespace*/
                valid = 1;
            } else {
                /*Integer followed by character*/
                printf("Only defined integer values will be accepted, please try again.\n\r");
                fflush(stdin);
            }
        } else if (valueRead == 1 && choice > 0 && choice < 6) {
            /*Integer followed by nothing*/
            valid = 1;
        } else {
            /*Not an integer*/
            printf("Only defined integer values will be accepted, please try again.\n\r");
            fflush(stdin);
        }
    } while (valid == 0);
    if (choice == 1 || choice == 2 || choice == 3 || choice == 4) {
        return choice;
    } else {
        exit(0);
    }
}

void fourBands(const char** bandColours, const char** multiplierColours, const char** toleranceColours, int bCLen, int mCLen, int tCLen, data* d) {
    /*
    Name: fourBands
    Function: To prompt and accept user input for the colours of a 4 band resistor and set these values to a structure
    Paramaters: const char** bandColours, const char** multiplierColours, const char** toleranceColours - Arrays of strings (colour names)
                            int bCLen, int mCLen, int tCLen - Length of each array
                            data* d - Pointer to structure
    Variables: i, bandOne, bandTwo, bandThree, bandFour
    Function will read in values for each bands colour using bandInput to validate the selection. These values will be used to calculate the nominal value and tolerance of the resistor.
    These values will be stored in the structure that has been passed in as a pointer.
     */
    int i, bandOne, bandTwo, bandThree, bandFour;
    bandOne = bandInput(bandColours, bCLen, 1);
    bandTwo = bandInput(bandColours, bCLen, 2);
    bandThree = bandInput(multiplierColours, mCLen, 3);
    bandFour = bandInput(toleranceColours, tCLen, 4);

    d->nominalValue = idealVal(bandOne, bandTwo, bandThree, bandFour, 4);
    d->tolerance = tolVal(bandFour);
}

void fiveBands(const char** bandColours, const char** multiplierColours, const char** toleranceColours, int bCLen, int mCLen, int tCLen, data* d) {
    /*
    Name: fiveBands
    Function: To prompt and accept user input for the colours of a 5 band resistor and set these values to a structure
    Paramaters: const char** bandColours, const char** multiplierColours, const char** toleranceColours - Arrays of strings (colour names)
                            int bCLen, int mCLen, int tCLen - Length of each array
                            data* d - Pointer to structure
    Variables: i, bandOne, bandTwo, bandThree, bandFour
    Function will read in values for each bands colour using bandInput to validate the selection. These values will be used to calculate the nominal value and tolerance of the resistor.
    These values will be stored in the structure that has been passed in as a pointer.
     */
    int i, bandOne, bandTwo, bandThree, bandFour, bandFive;
    bandOne = bandInput(bandColours, bCLen, 1);
    bandTwo = bandInput(bandColours, bCLen, 2);
    bandThree = bandInput(bandColours, bCLen, 3);
    bandFour = bandInput(multiplierColours, mCLen, 4);
    bandFive = bandInput(toleranceColours, tCLen, 5);

    d->nominalValue = idealVal(bandOne, bandTwo, bandThree, bandFour, 5);
    d->tolerance = tolVal(bandFive);
}

void sixBands(const char** bandColours, const char** multiplierColours, const char** toleranceColours, const char** temperatureColours, int bCLen, int mCLen, int tCLen, int teCLen, data* d) {
    /*
    Name: sixBands
    Function: To prompt and accept user input for the colours of a 6 band resistor and set these values to a structure
    Paramaters: const char** bandColours, const char** multiplierColours, const char** toleranceColours, const char** temperatureColours - Arrays of strings (colour names)
                            int bCLen, int mCLen, int tCLen, int teCLen - Length of each array
                            data* d - Pointer to structure
    Variables: i, bandOne, bandTwo, bandThree, bandFour
    Function will read in values for each bands colour using bandInput to validate the selection. These values will be used to calculate the nominal value and tolerance of the resistor.
    These values will be stored in the structure that has been passed in as a pointer.
     */
    int i, bandOne, bandTwo, bandThree, bandFour, bandFive, bandSix;
    bandOne = bandInput(bandColours, bCLen, 1);
    bandTwo = bandInput(bandColours, bCLen, 2);
    bandThree = bandInput(bandColours, bCLen, 3);
    bandFour = bandInput(multiplierColours, mCLen, 4);
    bandFive = bandInput(toleranceColours, tCLen, 5);
    bandSix = bandInput(temperatureColours, teCLen, 6);

    d->nominalValue = idealVal(bandOne, bandTwo, bandThree, bandFour, 6);
    d->tolerance = tolVal(bandFive);
}

int bandInput(const char** colours, int arrLen, int bandNum) {
    /*
    Name: bandInput
    Function: To prompt user for input based on a given array
    Paramaters: const char** colours - Array of strings (colours)
                            int arrLen, int bandNum
    Variables: userInput, i, valueRead, pass, followChar
    Function will return userInput if it is valid, else it will loop asking for the user to try again
     */
    int userInput, i, valueRead, pass = 0;
    char followChar;

    do {
        for (i = 0; i < arrLen; i++) {
            printf("%d - %s\n\r", i, colours[i]);
        }
        printf("Please input a colour value of band %d using the table provided: ", bandNum);
        valueRead = scanf("%d%c", &userInput, &followChar);
        if (valueRead == 2) {
            if (isspace(followChar) && (userInput >= 0 || userInput <= arrLen)) {
                /*nteger followed by whitespace*/
                pass = 1;
            } else {
                /*Integer followed by character*/
                printf("Only defined integer values will be accepted, please try again.\n\r");
                fflush(stdin);
            }
        } else if (valueRead == 1 && (userInput >= 0 || userInput <= arrLen)) {
            /*Integer followed by nothing*/
            pass = 1;
        } else {
            /*Not an integer*/
            printf("Only defined integer values will be accepted, please try again.\n\r");
            fflush(stdin);
        }
    } while (pass == 0);
    return userInput;
}

double actualValInp(int i) {
    /*
    Name: actualValueInp
    Function: To input the actual resistance (in ohms) of the resistors in the batch
    Paramaters: i
    Variables: userInput
    Function will return userInput so long as the value is valid
     */
    double userInput;
    do {
        printf("Please input the actual resistance for resistor %d: ", i + 1);
        if (scanf("%lf", &userInput) != 1) {
            fflush(stdin);
            printf("Invalid value given, please try again\n\r");
        }
    } while (userInput < 0);
    return userInput;
}

double idealVal(int bandOne, int bandTwo, int bandThree, int bandFour, int bands) {
    /*
    Name: idealVal
    Function: to return the nominal value based on the number of bands and the colours given
    Paramaters: int bandOne, int bandTwo, int bandThree, int bandFout, int bands
    Variables: ideal
    Function will return the nominal value based on the colours of the bands input by using one of two cases (4 or 5/6 bands)
    If 4 bands is the case, the program will use multVal on band 3 to find its multiplier value and will use this in the calculation, otherwise, band 4 will be passed to multVal
     */
    double ideal;
    switch (bands) {
        case 4:
            ideal = (bandOne * 10 + bandTwo) * multVal(bandThree);
            return ideal;
            break;
        case 5:
            ideal = ((bandOne * 100) + (bandTwo * 10) + bandThree) * multVal(bandFour);
            return ideal;
            break;
        case 6:
        	ideal = ((bandOne * 100) + (bandTwo * 10) + bandThree) * multVal(bandFour);
            return ideal;
            break;
    }
}

double multVal(int value) {
    /*
    Name: multVal
    Function: Return the value of the multiplier band
    Paramaters: int value
    Variables: -
    Function will return the value of the multiplier based on an integer input that would represent the index of a colour in the multiplierColours array 
     */
    switch (value) {
        case 0:
            return 0.01;
            break;
        case 1:
            return 0.1;
            break;
        case 2:
            return 1;
            break;
        case 3:
            return 10;
            break;
        case 4:
            return 100;
            break;
        case 5:
            return 1000;
            break;
        case 6:
            return 10000;
            break;
        case 7:
            return 100000;
            break;
        case 8:
            return 1000000;
            break;
        case 9:
            return 10000000;
            break;
        default:
        	printf("Invalid value, cannot determine the multipier\n\r");
    }
}

double tolVal(int value) {
    /*
    Name: tolVal
    Function: Return the value of the tolerance band
    Paramaters: int value
    Variables: -
    Function will return the value of the tolerance based on an integer input that would represent the index of a colour in the toleranceColours array
     */
    switch (value) {
        case 0:
            return 0.1;
            break;
        case 1:
            return 0.05;
            break;
        case 2:
            return 0.01;
            break;
        case 3:
            return 0.02;
            break;
        case 4:
            return 0.005;
            break;
        case 5:
            return 0.0025;
            break;
        case 6:
            return 0.001;
            break;
         default:
        	printf("Invalid value, cannot determine the tolerance\n\r");
    }
}

int tempVal(int value) {
    /*
    Name: tempVal
    Function: Return the value of the temperature band
    Paramaters: int value
    Variables: -
    Function will return the value of the temperature based on an integer inpur that would represent the index of a colour in the temperatureColours array 
     */
    switch (value) {
        case 0:
            return 100;
            break;
        case 1:
            return 50;
            break;
        case 2:
            return 15;
            break;
        case 3:
            return 25;
            break;
        default:
        	printf("Invalid value, cannot determine the temperature tolerance\n\r");
    }
}

double minTolVal(double a, float b) {
    /*
    Name: minTolVal
    Function: Return the minimum tolerance value of the batch
    Paramaters: double a, double b
    Variables: -
    Function will calulate and return the minimum tolerance value
     */
    return a - a * b;
}

double maxTolVal(double a, float b) {
    /*
    Name: maxTolVal
    Function: Return the maximum tolerance value of the batch
    Paramaters: double a, double b
    Variables: -
    Function will calculate and return the maximum tolerance value
     */
    return a + a * b;
}

double sampleMean(double a[10]) {
    /*
    Name: sampleMean
    Function: Return the mean for the batch
    Paramaters: double a[]
    Variables: i
    Function will calculate and return the mean value for the 10 actual values that have been input
     */
    int i;
    double mean, tot = 0;
    for (i = 0; i < 10; i++) {
        tot += a[i];
    }
    mean = tot / 10;
    return mean;
}

float sampleStandDev(double a[10], double mean) {
    /*
    Name: sampleStandDev
    Function: Return the standard deviation for the batch
    Paramaters: double a[], double mean
    Variables: i, tot
    Function will calculate and return the standard deviation for the 10 actual values that have been input
     */
    int i;
    double tot = 0;
    float standDev;
    for (i = 0; i < 10; i++) {
        tot += pow(a[i] - mean, 2);
    }
    standDev = tot / 10;
    return standDev;
}



