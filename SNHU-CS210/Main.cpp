/*
 * Austin Henley
 * CS-210 T1111
 * October 2022
 *
 * Airgaed Banking Project Two
 * Create banking app that shows reports of data regarding input
 */

 //Main file for running program


#include "Banking.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;


int main()
{
	double initialDeposit = 0;
	double monthlyDeposit = 0;
	double annualInterest = 0;
	int numOfYears = 0;
	bool ContinueSesh = true;

	while (ContinueSesh) //continue loop while user inputs new amounts
	{

		printInputMenu(initialDeposit, monthlyDeposit, annualInterest, numOfYears); //Print input menu to screen and take input as varaiables

		system("pause"); //Pause program and wait for button press to continue
		system("CLS"); //clear screen


		printYearlyBalHeadr(); //Print header for non-monthly deposit report
		balancesWithoutMonthlyDeposit(initialDeposit, annualInterest, numOfYears); //Print report for balances without monthly deposit

		cout << endl; //Insert empty line between reports

		printYearlyBalHeadWithMonth(); //Print header for monthly deposit report
		balancesWithMonthlyDeposit(initialDeposit, monthlyDeposit, annualInterest, numOfYears); //Print report for balances with monthly deposit

		system("pause"); //Pause program and wait for button press to continue
		system("CLS"); //clear screen

		cin.ignore(); //clear input buffer before asking if user wants to continue through getline()

		ContinueSesh = continueSession(); //Function for asking if user wants to continue session, returning false quits the loop

		system("CLS"); //clear screen before starting program again
	}


	return 0;
}