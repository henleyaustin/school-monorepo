/*
 * Austin Henley
 * CS-210 T1111
 * October 2022
 *
 * Airgaed Banking Project Two
 * Create banking app that shows reports of data regarding input
 */

 //Source file for function definitions

#include <math.h>
#include <string>
#include <iostream>
#include <iomanip> 
#include "Banking.h"

using namespace std;


/*
 * Transform characters into string of width 'n'
 * 
 * @param t_length, length of characters > 0
 * @param t_symbol, character to be replicated
 *
 * @return string of t_length size
 */
string nCharString(size_t t_length, char t_symbol) {
    string str;
    for (int i = 0; i < t_length; i++) //loops and prints character 'length' number of times
    {
        str.append(1, t_symbol);
    }
    return str;
}

/*
 * Print main menu header
 *
 */
void printMenuHead()
{
    cout << nCharString(64, '*') << endl; //prints characters to screen for visual respresentation
    cout << nCharString(26, '*') << " Data Input " << nCharString(26, '*') << endl;
}

/*
 * Print main input menu and take inputs for amounts
 *
 * @param t_deposit, initial deposit variable - call by reference
 * @param t_monthly, monthly deposit variable - call by reference
 * @param t_annualInt, annual interest rate variable - call by reference
 * @param t_years, years of balance increase variable - call by reference
 * 
 */
void printInputMenu(double &t_deposit, double &t_monthly, double &t_annualInt, int &t_years)
{
    printMenuHead(); //Print menu header 

    while (cout << "Initial Investment Amount: $" && !(cin >> t_deposit) || t_deposit < 0) //input validation for investment amount (must be a positive number)
    {
        cin.clear(); //clear input buffer to avoid trailing characters
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignore rest of the line
        cout << "-- Input must be a non-negative number --" << endl; //Output what is wrong with input
    }
    while (cout << "Monthly Deposit: $" && !(cin >> t_monthly) || t_monthly < 0) //input validation for monthly deposit (must be a positive number)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "-- Input must be a non-negative number --" << endl;
    }
    while (cout << "Annual Interest: %" && !(cin >> t_annualInt) || t_annualInt < 0) //input validation for annual interest (must be a positive number)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "-- Input must be a non-negative number --" << endl;
    }
    while (cout << "Number of years: " && !(cin >> t_years) || t_years < 0) //input validation for number of years (must be a positive number)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "-- Input must be a non-negative number --" << endl;
    }
}

/*
 * Print header for yearly balance reports
 *
 */
void printYearlyBalHeadr() 
{
    cout << nCharString(79, '=') << endl; //create line for top of report
    cout << "            Balance and Interest Without Additional Monthly Deposits           " << endl; //Header Title
    cout << nCharString(79, '=') << endl;
    cout << setw(23) << left << "  Year" << setw(26) << "Year End Balance" << setw(29) << "Year End Earned Interest   " << endl; //Column headers
    cout << nCharString(79, '-') << endl;
}

/*
 * Print header for yearly balance reports with additional monthly deposits
 *
 */
void printYearlyBalHeadWithMonth()
{
    cout << nCharString(79, '=') << endl; //create line for top of report
    cout << "            Balance and Interest With Additional Monthly Deposits           " << endl;//Header Title
    cout << nCharString(79, '=') << endl;
    cout << setw(23) << left << "  Year" << setw(26) << "Year End Balance" << setw(29) << "Year End Earned Interest   " << endl;//Column headers
    cout << nCharString(79, '-') << endl;
}


/*
 * Output variables in formatted widths
 *
 * @param t_year, year of annual balance
 * @param t_yearEndBalance, Balance at the end of year
 * @param t_interestEarned, interest earned for year
 * 
 */
void printDetails(int t_year, double t_yearEndBalance, double t_interestEarned)
{
    cout << fixed << setprecision(2); //set output to only display two places after decimal
    cout << "   " << setw(24) << left << t_year << "$" << setw(30) << t_yearEndBalance << "$" << setw(25) << t_interestEarned << endl;
}

/*
 * Print variables in formatted widths
 *
 * @param t_initialInvestment, initial investment amount
 * @param t_monthlyDeposit, amount deposited monthly
 * @param t_interestRate, interest rate to be calculated
 * @param t_numOfYears, number of years to run calculation
 *
 */
void balancesWithMonthlyDeposit(double t_initialInvestment, double t_monthlyDeposit, double t_interestRate, int t_numOfYears)
{
    double totalBal = 0; 
    double interestTotal = 0.00;
    double monthlyInterest = 0;

    totalBal = t_initialInvestment;

    for (int i = 1; i <= t_numOfYears; i++) //loop to run formula for set number of years and output at the end of each year
    {
        interestTotal = 0.0; //reset interest total at the beginning of each year

        for (int j = 1; j <= 12; j++) //loop to calculate monthly interest and add to overall total
        {
            totalBal += t_monthlyDeposit;
            monthlyInterest = totalBal * ((t_interestRate / 100.0) / 12.0);
            interestTotal += monthlyInterest;
            totalBal += monthlyInterest;
        }

        printDetails(i, totalBal, interestTotal); //Print each year to screen
    }
}

/*
 * Print variables in formatted widths
 *
 * @param t_initialInvestment, initial investment amount
 * @param t_interestRate, interest rate to be calculated
 * @param t_numOfYears, number of years to run calculation
 *
 */
void balancesWithoutMonthlyDeposit(double t_initialInvestment, double t_interestRate, int t_numOfYears)
{
    double totalBal = 0;
    double interestTotal = 0.00;
    double monthlyInterest = 0;

    totalBal = t_initialInvestment;

    for (int i = 1; i <= t_numOfYears; i++) //loop to run formula for set number of years and output at the end of each year
    {
        interestTotal = 0.0; //reset interest total at the beginning of each year

        for (int j = 1; j <= 12; j++) //loop to calculate monthly interest and add to overall total along with monthly deposit
        {
            monthlyInterest = totalBal * ((t_interestRate / 100.0) / 12.0);
            interestTotal += monthlyInterest;
            totalBal += monthlyInterest;
        }

        printDetails(i, totalBal, interestTotal);//Print each year to screen
    }
}

/*
 * Ask user if they would like to continue session
 *
 * @return bool choice, true to continue - false to quit
 */
bool continueSession()
{
    string choice;

    cout << "Press enter to try more values ('q' to quit)" << endl; //ask user to input enter to continue
    getline(cin, choice);

    if (choice.empty()) //checking if input is empty and returning true if so
    {
        return true;
    }
    else if (choice == "q") //returning false if input is 'q', therefore quitting application
    { 
        return false;
    }
}

