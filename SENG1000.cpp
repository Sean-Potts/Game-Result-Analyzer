/*
*  FILE	         : m4.cpp
*  PROJECT       : Major Assignment 4
*  PROGRAMMER    : Sean Potts
*  FIRST VERSION : 2021-12-05
*  DESCRIPTION   :
*	This program opens a text file called teams.txt to be processed. Teams.txt provides the file names of other files
*	one file per line. These files contain the game results between two teams, the primary team which can be found in the file name
*	located inside teams.txt and the opponent team which can be found inside the game results file, along with the score. This program uses 3 functions
*	to proccess and display the game results for teams found in teams.txt displaying who they played, if it was a win/loss/tie, and the score. It then displays
*	the primary teams win percentage followed by their total wins, losses, and ties.
*/

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#pragma warning(disable: 4996)

void clearCR(char buf[]);
int processGames(char teamText[]);
int parseLine(char gameResult[], char opponentName[], int* primaryTeamScore, int* opponentTeamScore);

const int ARRAY_LENGTH = 41;
const int SUCCESS = 0;
const int ERROR = -1;

int main() {

	char teamName[ARRAY_LENGTH] = { 0 };

	FILE* fp = NULL;

	// Creates a text file 
	if ((fp = fopen("teams.txt", "r")) == NULL)
	{
		printf("Error, can't open teams.txt for reading");
		return ERROR;
	}

	while (fgets(teamName, sizeof teamName, fp) != NULL)
	{
		// Checks for blank/empty lines and skips over them
		if ((teamName[0] == ' ') || (teamName[0] == '\n') || (teamName[0] == '\0'))
		{
			continue;
		}
		// Checks for end of file
		if (feof(fp))
		{
			break;
		}
		// Checks for error
		if (ferror(fp))
		{
			printf("Error, reading lines from teams.txt file\n");

			if (fclose(fp) != 0)
			{
				// we can't even close the file
				printf("Error, can't close file opened teams.txt for reading\n");
			}
			return ERROR;
		}

		// check and remove \n if present at last character
		clearCR(teamName);

		printf("Processing %s:\n", teamName);

		if (processGames(teamName) != 0)
		{
			printf("Error occured procces game results\n");
			return ERROR;
		}
	}

	if (fclose(fp) != 0)
	{
		printf("Can't close file opened, teams.txt, for reading\n");
		return ERROR;
	}

	return 0;
}

/*
*	Function		 : clearCR()
*	Description		 :
*			This function checks if a string has a '\n' character and removes it if it does.
*	Parameters		 :
*			char buf[]		  : A string given to the function to check if it has a '\n'
*	Returns			 : void :
*			This function does not return anything.
*/
void clearCR(char buf[])
{
	char* whereCR = strchr(buf, '\n');
	if (whereCR != NULL)
	{
		*whereCR = '\0';
	}
}

/*
*	Function		: processGames()
*	Description		:
*			Takes 1 parameter, The file name of the team's game result to be processed. This function totals
*			the wins, losses, and ties for the team being processed and then displays a final winning percentage. This
*			function calls on two supporting functions parseline(), and clearCR().
*	Parameters		:
*			char teamText[]		 :	The file name of the game results file, where the file name is the primary teams name
*	Returns			: 0 or -1:
*			0 indicates the function worked without any errors and -1 indicates an error occured.
*/
int processGames(char teamText[]) {

	char* index = NULL;

	// Variables to keep track of Wins, Ties, and Losses of the primary team
	int gamesWon = 0;
	int gamesLost = 0;
	int gamesTied = 0;

	double winPrecent = 0;

	int primaryTeamScore = 0;
	int opponentTeamScore = 0;

	char opponentName[ARRAY_LENGTH] = { 0 };
	char primaryName[ARRAY_LENGTH] = { 0 };
	char gameResultsString[ARRAY_LENGTH] = { 0 };

	// Checks for the presents of a extension and removes it, used strrchr to search from the end of the string
	// so no periods are removed from withing the file name.
	if ((index = strrchr(teamText, '.')) == NULL)
	{
		printf("No extension\n");
		return ERROR;
	}

	//Removes extensions from teamText
	strncpy(primaryName, teamText, (index - teamText));

	FILE* fpa = NULL;

	// Creates a text file 
	if ((fpa = fopen(teamText, "r")) == NULL) {
		printf("Error, can't open game results file for reading\n");
		return ERROR;
	}

	while (fgets(gameResultsString, sizeof gameResultsString, fpa) != NULL) {

		if (feof(fpa))
		{
			break;
		}

		if (ferror(fpa))
		{
			printf("Error reading lines from game results file\n");

			if (fclose(fpa) != 0)
			{
				printf("Error, can't close game results file opened for reading\n");
			}
			return ERROR;
		}

		clearCR(gameResultsString);

		if (parseLine(gameResultsString, opponentName, &primaryTeamScore, &opponentTeamScore) == -1)
		{
			continue;
		}

		if (primaryTeamScore > opponentTeamScore)
		{
			gamesWon += 1;
			printf("\t%s beat %s %d-%d\n", primaryName, opponentName, primaryTeamScore, opponentTeamScore);
		}

		if (primaryTeamScore == opponentTeamScore)
		{
			gamesTied += 1;
			printf("\t%s and %s tied at %d\n", primaryName, opponentName, primaryTeamScore);
		}

		if (primaryTeamScore < opponentTeamScore)
		{
			gamesLost += 1;
			printf("\t%s lost to %s %d-%d\n", primaryName, opponentName, opponentTeamScore, primaryTeamScore);
		}
	}

	winPrecent = ((2 * (double)gamesWon + (double)gamesTied) / (2 * ((double)gamesWon + (double)gamesTied + (double)gamesLost)));
	printf("Season results for %s: %0.3f (%d-%d-%d)\n\n", primaryName, winPrecent, gamesWon, gamesLost, gamesTied);

	if (fclose(fpa) != 0) {
		printf("Error, can't close game results file");
		return ERROR;
	}
	return SUCCESS;
}

/*
*	Function		: parseLine()
*	Description		:
*			Takes 4 parameters, the array containing the game result string, the array for the opponent's name,
*			a pointer to the score for the primary team, a pointer to the score for the opponent team. It the parse the
*			string found in gameResults[] extracting the opponents team name and storing it in the varriable opponentName[],
*			the score of the primary team as an int and stores it in primaryTeamScore, and the score of the opponent team as an int
*			and stores it in opponentTeamScore.
*	Parameters		:
*			char gameResults[]				 :	The array containing the game result string
*			char opponentName[]				 :	The array for the opponent's name
*			int* primaryTeamScore			 :	A pointer to the score for the primary team
*			int* opponentTeamScore			 :	A pointer to the score for the opponent team
*	Returns			: 0 or -1 :
*			0 indicates the function worked without any errors and -1 indicates an error occured.
*/
int parseLine(char gameResult[], char opponentName[], int* primaryTeamScore, int* opponentTeamScore) {

	char* index = NULL;

	// used size_t data type to avoid warring because of possible loss of data 
	size_t indexOfComma = 0;
	size_t indexOfDash = 0;
	size_t indexOfNewLine = 0;

	// buffer variable to hold string before it is converted into a number
	char buffer[10] = { 0 };

	// Checks if the text file has empty/blank lines and returns 1 to indicate it does
	if (gameResult[0] == ' ' || gameResult[0] == '\n' || gameResult[0] == '\0')
	{
		return ERROR;
	}
	// Finds the ',' character in gameResults
	if ((index = strchr(gameResult, ',')) == NULL)
	{
		printf("Error: No comma in data file. Skipping this entry\n");
		return ERROR;
	}

	indexOfComma = index - gameResult;

	//copies the opponents name from the string into the variable opponentName
	strncpy(opponentName, gameResult, indexOfComma);

	// Finds the '-' character in gameResults
	if ((index = strchr(gameResult, '-')) == NULL)
	{
		printf("Error: No dash in data file. Skipping this entry\n");
		return ERROR;
	}

	indexOfDash = index - gameResult - 1;

	//copies the game primary teams score into a variable called buffer
	strncpy(buffer, &gameResult[indexOfComma + 1], (indexOfDash - indexOfComma));

	// Need null character at the end of buffer to signify it is a string so the function atoi can be used
	buffer[strlen(buffer)] = '\0';
	*primaryTeamScore = atoi(buffer);

	indexOfNewLine = strlen(gameResult) - 1;

	//copies the game opponent teams score into a variable called buffer
	// indexOfDash +2 because took 1 away one line 259, so I need to go back to the original position plus one more
	strncpy(buffer, &gameResult[indexOfDash + 2], (indexOfNewLine - indexOfDash));

	// Need null character at the end of buffer to signify it is a string so the function atoi can be used
	buffer[strlen(buffer)] = '\0';
	*opponentTeamScore = atoi(buffer);

	return SUCCESS;
}