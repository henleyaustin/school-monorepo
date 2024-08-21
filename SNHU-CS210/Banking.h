#pragma once

/*
 * Austin Henley
 * CS-210 T1111
 * October 2022
 *
 * Airgaed Banking Project Two
 * Create banking app that shows reports of data regarding input
 */

//Header file for function declarations

#ifndef BANKING_H
#define BANKING_H

void printMenuHead(); //Print header for input menu

void printInputMenu(double& deposit, double& monthly, double& annual, int& years); //Print menu asking for input into 4 variables

void printYearlyBalHeadr(); //Print header for static reports

void printYearlyBalHeadWithMonth();

void balancesWithMonthlyDeposit(double initialInvestment, double monthlyDeposit, double interestRate, int numberOfYears); //Print yearly balances base off input - With monthly deposit

void balancesWithoutMonthlyDeposit(double initialInvestment, double interestRate, int numberOfYears); //Print yearly balances base off input - Without monthly deposit

void printDetails(int year, double yearEndBalance, double interestEarned); //Function used by other functions to print in formatted width

bool continueSession(); //Function to ask user if they want to continue

#endif BANKING_H

