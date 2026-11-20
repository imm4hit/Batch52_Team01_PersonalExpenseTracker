# SPENDEMIC

SPENDEMIC is a command-line expense tracking and budget management program written in C.

The program allows users to manage a monthly budget, record expenses, analyze spending and check their current financial status.

## Features

### 1. Set Monthly Budget

Allows the user to set or update their monthly budget.

The budget is used to calculate:

* Total amount spent
* Money remaining
* Percentage of budget used

### 2. Add Expense

Allows the user to record a new expense.

Each expense can contain information such as:

* Expense amount
* Category
* Transaction details

After an expense is added, the program automatically updates the total spending and remaining budget.

### 3. View Transactions

Displays the transactions recorded by the user.

The user can view:

* Transaction number
* Amount spent
* Expense category
* Transaction details

This provides a simple record of previous expenses.

### 4. Expense Detector

Analyzes the user's recorded expenses.

It can be used to:

* Analyze spending
* Identify high expenses
* Detect unusual spending

### 5. Can I Afford This?

Checks whether a planned purchase is affordable based on the user's remaining budget.

The user enters the cost of the planned purchase.

The program then:

1. Checks the remaining budget
2. Compares it with the purchase amount
3. Displays whether the purchase is affordable

Example:

```text
Remaining budget: Rs 9000.00
Planned purchase: Rs 20000

[NO] UNAFFORDABLE - exceeds remaining budget.
```

### 6. Budget Survival Forecast

Estimates whether the current budget can survive for the remaining part of the month.

The forecast considers:

* Current day of the month
* Daily spending
* Projected spending
* Available budget

The program then indicates whether the budget is currently safe or at risk.

Example:

```text
Day of month       : 20
Daily spending     : Rs 50.00
Projected spending : Rs 1500.00

[SAFE] Budget is currently on track.
```

### 7. Spending Timeline

Displays recorded expenses using a simple visual representation.

Example:

```text
#1   Rs 1000.00   | ##########
```

This makes it easier to visually compare spending between transactions.

### 8. Undo Last Transaction

Removes the most recently recorded transaction.

After removing the transaction, the program updates:

* Total spending
* Remaining money
* Number of transactions
* Percentage of budget used

### 9. Exit

Ends the SPENDEMIC program.

## Dashboard

The main dashboard gives the user a summary of their current financial status.

Example:

```text
=============== SPENDEMIC ===============
              TRACK THE DAMAGE
-----------------------------------------

Monthly Budget : Rs 10000.00
Damage Done    : Rs 1000.00
Money Left     : Rs 9000.00
Transactions   : 1
Budget Used    : 10.0%
```

### Dashboard Information

1. **Monthly Budget**

   The total monthly budget entered by the user.

2. **Damage Done**

   The total amount of money spent.

3. **Money Left**

   The amount remaining from the monthly budget.

4. **Transactions**

   The total number of recorded expenses.

5. **Budget Used**

   The percentage of the monthly budget that has already been spent.

## Main Menu

The program provides nine options:

```text
1. Set Monthly Budget
2. Add Expense
3. View Transactions
4. Expense Detector
5. Can I Afford This?
6. Budget Survival Forecast
7. Spending Timeline
8. Undo Last Transaction
9. Exit
```

The user selects a feature by entering its corresponding number.

## Program Flow

The basic working of SPENDEMIC is:

1. Start the program.
2. Load or initialize the required data.
3. Display the current budget dashboard.
4. Display the main menu.
5. Ask the user to select an option.
6. Perform the selected operation.
7. Update the budget information when required.
8. Return to the main menu.
9. Continue until the user selects Exit.

## C Programming Concepts Used

The project uses several important concepts of C programming:

* Functions
* Structures
* Arrays
* Strings
* Loops
* Conditional statements
* Pointers
* File handling
* Header files
* User input and output
* Modular programming


SPENDEMIC was created as a simple budget and expense management system.

The project demonstrates how fundamental C programming concepts can be combined to create a complete command-line application with multiple useful features.
