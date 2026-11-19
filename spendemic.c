/* SPENDEMIC - Single-file version, combined from the 13 uploaded files. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ===== expense(2).h ===== */
#define MAX_EXPENSES 200
#define MAX_TEXT 80
#define MAX_RULES 80
#define RESERVE_DEFAULT 500.0

typedef struct {
    int id;
    double amount;
    char date[11];
    char category[30];
    char subcategory[30];
    char description[MAX_TEXT];
    int essential; /* 1 = essential, 0 = discretionary */
} Expense;

void addExpense(Expense expenses[], int *count, int *nextID, double budget);
void viewExpenses(const Expense expenses[], int count);
void undoLastExpense(Expense expenses[], int *count, int *nextID);
double calculateTotal(const Expense expenses[], int count);

/* ===== classification(1).h ===== */
typedef struct {
    char keyword[30];
    char category[30];
    char subcategory[30];
    int essential;
} Rule;

void loadRules(Rule rules[], int *ruleCount);
void saveRules(const Rule rules[], int ruleCount);
void classifyDescription(const char description[], const Rule rules[], int ruleCount,
                        char category[], char subcategory[], int *essential, int *matches);
void addCustomRule(Rule rules[], int *ruleCount);

/* ===== storage(2).h ===== */
void loadExpenses(Expense expenses[], int *count, int *nextID);
void saveExpenses(const Expense expenses[], int count);
double loadBudget(void);
void saveBudget(double budget);

/* ===== analysis(2).h ===== */
void expenseDetector(const Expense expenses[], int count);
void affordabilityCheck(const Expense expenses[], int count, double budget);
void budgetForecast(const Expense expenses[], int count, double budget);
void spendingTimeline(const Expense expenses[], int count);
void dashboard(const Expense expenses[], int count, double budget);

/* ===== boss_battle(1).h ===== */
void bossBattle(const Expense expenses[], int count, double budget);

/* ===== ui(2).h ===== */
void showMenu(void);
int getInt(const char *prompt, int min, int max);
double getDouble(const char *prompt, double min, double max);
void getText(const char *prompt, char text[], int size);
void pauseScreen(void);
void printTitle(const char title[]);

/* ===== ui(2).c ===== */
void printTitle(const char title[]) {
    printf("\n==================================================\n");
    printf("  %s\n", title);
    printf("==================================================\n");
}

void showMenu(void) {
    printTitle("SPENDEMIC | TRACK THE DAMAGE");
    puts(" 1. Set monthly budget");
    puts(" 2. Add expense (Auto-Bifurcation)");
    puts(" 3. View transactions");
    puts(" 4. Expense detector");
    puts(" 5. Can I afford this?");
    puts(" 6. Budget survival forecast");
    puts(" 7. Spending timeline (last 7 days)");
    puts(" 8. Financial Boss Battle");
    puts(" 9. Undo last transaction");
    puts("10. Dashboard");
    puts("11. Add custom classification keyword");
    puts("12. Exit");
}

int getInt(const char *prompt, int min, int max) {
    char line[100];
    char *end;
    long value;
    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof line, stdin)) return min;
        value = strtol(line, &end, 10);
        if (end != line && (*end == '\n' || *end == '\0') && value >= min && value <= max)
            return (int)value;
        puts("Invalid input. Please enter a whole number in the allowed range.");
    }
}

double getDouble(const char *prompt, double min, double max) {
    char line[100];
    char *end;
    double value;
    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof line, stdin)) return min;
        value = strtod(line, &end);
        if (end != line && (*end == '\n' || *end == '\0') && value >= min && value <= max)
            return value;
        puts("Invalid input. Enter a valid number in the allowed range.");
    }
}

void getText(const char *prompt, char text[], int size) {
    printf("%s", prompt);
    if (!fgets(text, size, stdin)) { text[0] = '\0'; return; }
    text[strcspn(text, "\n")] = '\0';
    /* Keep the file format safe: replace pipe characters. */
    for (int i = 0; text[i] != '\0'; i++) if (text[i] == '|') text[i] = '/';
}

void pauseScreen(void) {
    char temp[8];
    printf("\nPress Enter to return to the menu...");
    fgets(temp, sizeof temp, stdin);
}

/* ===== storage(2).c ===== */
void loadExpenses(Expense expenses[], int *count, int *nextID) {
    FILE *fp = fopen("expenses.txt", "r");
    *count = 0;
    *nextID = 1;
    if (!fp) return; /* First run: no file yet. */
    while (*count < MAX_EXPENSES) {
        Expense e;
        int read = fscanf(fp, "%d|%lf|%10[^|]|%29[^|]|%29[^|]|%d|%79[^\n]\n",
                         &e.id, &e.amount, e.date, e.category, e.subcategory,
                         &e.essential, e.description);
        if (read != 7) break;
        expenses[*count] = e;
        if (e.id >= *nextID) *nextID = e.id + 1;
        (*count)++;
    }
    fclose(fp);
}

void saveExpenses(const Expense expenses[], int count) {
    FILE *fp = fopen("expenses.txt", "w");
    if (!fp) { puts("ERROR: Could not save expenses."); return; }
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d|%.2f|%s|%s|%s|%d|%s\n", expenses[i].id,
                expenses[i].amount, expenses[i].date, expenses[i].category,
                expenses[i].subcategory, expenses[i].essential, expenses[i].description);
    fclose(fp);
}

double loadBudget(void) {
    FILE *fp = fopen("budget.txt", "r");
    double budget = 0;
    if (fp) { fscanf(fp, "%lf", &budget); fclose(fp); }
    return budget;
}

void saveBudget(double budget) {
    FILE *fp = fopen("budget.txt", "w");
    if (!fp) { puts("ERROR: Could not save budget."); return; }
    fprintf(fp, "%.2f\n", budget);
    fclose(fp);
}

/* ===== classification(1).c ===== */
static int containsIgnoreCase(const char text[], const char keyword[]) {
    char lowerText[MAX_TEXT], lowerKey[30];
    size_t i;
    for (i = 0; i < sizeof lowerText - 1 && text[i]; i++) lowerText[i] = (char)tolower((unsigned char)text[i]);
    lowerText[i] = '\0';
    for (i = 0; i < sizeof lowerKey - 1 && keyword[i]; i++) lowerKey[i] = (char)tolower((unsigned char)keyword[i]);
    lowerKey[i] = '\0';
    return strstr(lowerText, lowerKey) != NULL;
}

void loadRules(Rule rules[], int *ruleCount) {
    FILE *fp = fopen("rules.txt", "r");
    *ruleCount = 0;
    if (fp) {
        while (*ruleCount < MAX_RULES && fscanf(fp, "%29[^|]|%29[^|]|%29[^|]|%d\n",
               rules[*ruleCount].keyword, rules[*ruleCount].category,
               rules[*ruleCount].subcategory, &rules[*ruleCount].essential) == 4) {
            (*ruleCount)++;
        }
        fclose(fp);
    }
    if (*ruleCount == 0) {
        Rule defaults[] = {
            {"uber", "Transport", "Cab", 1}, {"ola", "Transport", "Cab", 1},
            {"auto", "Transport", "Auto", 1}, {"bus", "Transport", "Bus", 1},
            {"petrol", "Transport", "Fuel", 1}, {"pizza", "Food", "Fast Food", 0},
            {"burger", "Food", "Fast Food", 0}, {"coffee", "Food", "Beverage", 0},
            {"chai", "Food", "Beverage", 0}, {"mess", "Food", "Meals", 1},
            {"grocery", "Food", "Groceries", 1}, {"milk", "Food", "Groceries", 1},
            {"book", "Education", "Books", 1}, {"notebook", "Education", "Stationery", 1},
            {"pen", "Education", "Stationery", 1}, {"movie", "Entertainment", "Movies", 0},
            {"netflix", "Entertainment", "Subscription", 0}, {"game", "Entertainment", "Games", 0},
            {"shirt", "Shopping", "Clothing", 0}, {"shoes", "Shopping", "Clothing", 0},
            {"shampoo", "Personal Care", "Hygiene", 1}, {"soap", "Personal Care", "Hygiene", 1},
            {"medicine", "Medical", "Medicine", 1}, {"doctor", "Medical", "Consultation", 1}
        };
        int n = (int)(sizeof defaults / sizeof defaults[0]);
        for (int i = 0; i < n; i++) rules[i] = defaults[i];
        *ruleCount = n;
        saveRules(rules, *ruleCount);
    }
}

void saveRules(const Rule rules[], int ruleCount) {
    FILE *fp = fopen("rules.txt", "w");
    if (!fp) { puts("Warning: could not save keyword rules."); return; }
    for (int i = 0; i < ruleCount; i++)
        fprintf(fp, "%s|%s|%s|%d\n", rules[i].keyword, rules[i].category,
                rules[i].subcategory, rules[i].essential);
    fclose(fp);
}

void classifyDescription(const char description[], const Rule rules[], int ruleCount,
                        char category[], char subcategory[], int *essential, int *matches) {
    *matches = 0;
    strcpy(category, "Miscellaneous");
    strcpy(subcategory, "Unsorted");
    *essential = 0;
    /* register is a historical optimisation hint; modern compilers may ignore it. */
    for (register int i = 0; i < ruleCount; i++) {
        if (containsIgnoreCase(description, rules[i].keyword)) {
            (*matches)++;
            if (*matches == 1) {
                strcpy(category, rules[i].category);
                strcpy(subcategory, rules[i].subcategory);
                *essential = rules[i].essential;
            }
        }
    }
}

void addCustomRule(Rule rules[], int *ruleCount) {
    if (*ruleCount >= MAX_RULES) { puts("Rule list is full."); return; }
    Rule newRule;
    getText("Keyword to detect: ", newRule.keyword, sizeof newRule.keyword);
    getText("Category (e.g. Food): ", newRule.category, sizeof newRule.category);
    getText("Subcategory: ", newRule.subcategory, sizeof newRule.subcategory);
    newRule.essential = getInt("Essential? (1 yes, 0 no): ", 0, 1);
    if (!newRule.keyword[0] || !newRule.category[0]) { puts("Keyword/category cannot be empty."); return; }
    rules[*ruleCount] = newRule;
    (*ruleCount)++;
    saveRules(rules, *ruleCount);
    puts("Custom rule saved. Future matching expenses can use it.");
}

/* ===== expense(2).c ===== */
static void today(char date[]) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) strftime(date, 11, "%Y-%m-%d", t);
    else strcpy(date, "0000-00-00");
}

void addExpense(Expense expenses[], int *count, int *nextID, double budget) {
    Rule rules[MAX_RULES];
    int ruleCount, matches, essential, choice;
    char category[30], subcategory[30];
    if (*count >= MAX_EXPENSES) { puts("Transaction list is full."); return; }
    Expense e;
    e.amount = getDouble("Amount (Rs): ", 0.01, 100000000.0);
    getText("Description: ", e.description, sizeof e.description);
    if (!e.description[0]) { puts("Description cannot be empty. Transaction cancelled."); return; }
    loadRules(rules, &ruleCount);
    classifyDescription(e.description, rules, ruleCount, category, subcategory, &essential, &matches);
    printf("\nAUTO-BIFURCATION RESULT\nCategory: %s\nSubcategory: %s\nType: %s\n",
           category, subcategory, essential ? "Essential" : "Discretionary");
    printf("Keyword matches: %d (%s confidence)\n", matches,
           matches >= 2 ? "Multiple" : matches == 1 ? "Basic" : "No match");
    if (matches == 0) puts("No keyword matched. Defaulting to Miscellaneous.");
    choice = getInt("Accept (1) / Correct category (2) / Cancel (3): ", 1, 3);
    if (choice == 3) { puts("Transaction cancelled."); return; }
    if (choice == 2) {
        getText("Enter category: ", category, sizeof category);
        getText("Enter subcategory: ", subcategory, sizeof subcategory);
        essential = getInt("Essential? (1 yes, 0 no): ", 0, 1);
    }
    e.id = *nextID;
    strcpy(e.category, category);
    strcpy(e.subcategory, subcategory);
    e.essential = essential;
    today(e.date);
    expenses[*count] = e;
    (*count)++;
    (*nextID)++;
    printf("Expense saved. ID #%d\n", e.id);
    if (budget > 0 && e.amount > budget - calculateTotal(expenses, *count - 1))
        puts("WARNING: This transaction takes you over your monthly budget.");
}

void viewExpenses(const Expense expenses[], int count) {
    if (!count) { puts("No expenses recorded yet."); return; }
    printf("\n%-4s %-10s %-10s %-18s %-10s %s\n", "ID", "Date", "Amount", "Category", "Type", "Description");
    puts("--------------------------------------------------------------------------");
    for (int i = 0; i < count; i++)
        printf("%-4d %-10s Rs%-8.2f %-18s %-10s %s\n", expenses[i].id, expenses[i].date,
               expenses[i].amount, expenses[i].category, expenses[i].essential ? "Essential" : "Optional",
               expenses[i].description);
    printf("Total: Rs%.2f\n", calculateTotal(expenses, count));
}

void undoLastExpense(Expense expenses[], int *count, int *nextID) {
    if (*count == 0) { puts("Nothing to undo."); return; }
    Expense last = expenses[*count - 1];
    printf("Undo transaction #%d: %s (Rs%.2f)?\n", last.id, last.description, last.amount);
    if (getInt("Confirm (1 yes, 0 no): ", 0, 1) == 1) {
        (*count)--;
        *nextID = last.id; /* Reuse the ID of the removed last entry. */
        puts("Last transaction removed.");
    } else puts("Undo cancelled.");
}

double calculateTotal(const Expense expenses[], int count) {
    double total = 0;
    for (int i = 0; i < count; i++) total += expenses[i].amount;
    return total;
}

/* ===== analysis(2).c ===== */
static int daysInMonth(int year, int month) {
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) return 29;
    return days[month - 1];
}

static void currentDate(int *year, int *month, int *day) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) { *year = t->tm_year + 1900; *month = t->tm_mon + 1; *day = t->tm_mday; }
    else { *year = 2026; *month = 1; *day = 1; }
}

void expenseDetector(const Expense expenses[], int count) {
    if (!count) { puts("No transactions to analyse."); return; }
    double total = 0, average;
    int flags = 0;
    for (int i = 0; i < count; i++) total += expenses[i].amount;
    average = total / count;
    printTitle("EXPENSE CRIME SCENE");
    printf("Average transaction: Rs%.2f\n", average);
    for (int i = 0; i < count; i++) {
        if (expenses[i].amount > average * 2 && expenses[i].amount >= 500) {
            printf("[HIGH] #%d %s: Rs%.2f is over twice the average.\n", expenses[i].id, expenses[i].description, expenses[i].amount);
            flags++;
        }
        if (expenses[i].amount <= 200 && i == 0) {
            int smallCount = 0;
            double smallTotal = 0;
            for (int j = 0; j < count; j++) {
                if (expenses[j].amount <= 200) {
                    smallCount++;
                    smallTotal += expenses[j].amount;
                }
            }
            if (smallCount >= 5)
                printf("[LEAK] %d purchases of Rs200 or less total Rs%.2f.\n", smallCount, smallTotal);
        }
        for (int j = i + 1; j < count; j++) {
            if (expenses[i].amount == expenses[j].amount &&
                strcmp(expenses[i].date, expenses[j].date) == 0 &&
                strcmp(expenses[i].description, expenses[j].description) == 0) {
                printf("[DUPLICATE?] Transactions #%d and #%d look identical.\n", expenses[i].id, expenses[j].id);
                flags++;
            }
        }
    }
    /* Category total check: if one category exceeds 40% of all spending, flag it. */
    for (int i = 0; i < count; i++) {
        double categoryTotal = 0;
        int alreadyChecked = 0;
        for (int k = 0; k < i; k++) if (strcmp(expenses[k].category, expenses[i].category) == 0) alreadyChecked = 1;
        if (alreadyChecked) continue;
        for (int j = 0; j < count; j++) if (strcmp(expenses[i].category, expenses[j].category) == 0) categoryTotal += expenses[j].amount;
        if (total > 0 && categoryTotal / total > 0.40) {
            printf("[CATEGORY] %s accounts for %.1f%% of your spending.\n", expenses[i].category, categoryTotal / total * 100);
            flags++;
        }
    }
    if (!flags) puts("No suspicious patterns detected. Your money is behaving. Suspiciously.");
}

void affordabilityCheck(const Expense expenses[], int count, double budget) {
    double spent = 0, reserve = RESERVE_DEFAULT, purchase, remaining, safe;
    if (budget <= 0) { puts("Set a monthly budget first."); return; }
    spent = calculateTotal(expenses, count);
    remaining = budget - spent;
    purchase = getDouble("Planned purchase amount (Rs): ", 0.01, 100000000.0);
    safe = remaining - reserve;
    printTitle("CAN I AFFORD THIS?");
    printf("Budget remaining: Rs%.2f\nEmergency reserve: Rs%.2f\n", remaining, reserve);
    if (purchase <= safe) puts("VERDICT: SAFE. Purchase fits while keeping the reserve.");
    else if (purchase <= remaining) puts("VERDICT: RISKY. You can pay, but it uses your emergency reserve.");
    else puts("VERDICT: NOT AFFORDABLE. This exceeds your remaining budget.");
    printf("After purchase, simulated balance: Rs%.2f\n", remaining - purchase);
    if (getInt("Record this as an expense? (1 yes, 0 no): ", 0, 1)) {
        puts("Go to Add Expense to record it with a description and category.");
    } else puts("Simulation discarded. Your real records are unchanged.");
}

void budgetForecast(const Expense expenses[], int count, double budget) {
    int y, m, d, monthDays;
    double spent, daily, projected;
    if (budget <= 0) { puts("Set a monthly budget first."); return; }
    currentDate(&y, &m, &d);
    monthDays = daysInMonth(y, m);
    spent = calculateTotal(expenses, count);
    daily = spent / d;
    projected = daily * monthDays;
    printTitle("BUDGET SURVIVAL FORECAST");
    printf("Spent so far: Rs%.2f\nRemaining: Rs%.2f\nAverage per day: Rs%.2f\n", spent, budget-spent, daily);
    printf("Days elapsed: %d / %d\nProjected month-end spending: Rs%.2f\n", d, monthDays, projected);
    if (projected <= budget) puts("FORECAST: Budget likely survives at this pace.");
    else puts("FORECAST: Budget likely to be exceeded. Consider slowing spending.");
    if (daily > 0 && spent < budget) {
        int daysUntilEmpty = (int)((budget - spent) / daily);
        printf("At current pace, remaining budget may last approximately %d more days.\n", daysUntilEmpty);
    } else if (spent >= budget) puts("Your budget is already exhausted or exceeded.");
}

void spendingTimeline(const Expense expenses[], int count) {
    int y, m, d;
    currentDate(&y, &m, &d);
    if (!count) { puts("No expenses to chart."); return; }
    printTitle("LAST 7 DAYS | SPENDING TIMELINE");
    for (int offset = 6; offset >= 0; offset--) {
        int targetDay = d - offset;
        int targetMonth = m, targetYear = y;
        if (targetDay <= 0) {
            targetMonth--;
            if (targetMonth == 0) { targetMonth = 12; targetYear--; }
            targetDay += daysInMonth(targetYear, targetMonth);
        }
        char date[11];
        snprintf(date, sizeof date, "%04d-%02d-%02d", targetYear, targetMonth, targetDay);
        double sum = 0;
        for (int i = 0; i < count; i++) if (strcmp(expenses[i].date, date) == 0) sum += expenses[i].amount;
        printf("%s Rs%-8.2f ", date, sum);
        int bars = (int)(sum / 100.0);
        if (bars > 50) bars = 50;
        for (int b = 0; b < bars; b++) putchar('#');
        putchar('\n');
    }
    puts("Each # represents approximately Rs100.");
}

void dashboard(const Expense expenses[], int count, double budget) {
    double total = calculateTotal(expenses, count);
    printTitle("FINANCIAL DASHBOARD");
    printf("Monthly budget : Rs%.2f\n", budget);
    printf("Total spent   : Rs%.2f\n", total);
    printf("Balance       : Rs%.2f\n", budget - total);
    printf("Transactions  : %d\n", count);
    if (budget > 0) {
        double percent = total / budget * 100;
        printf("Budget used   : %.1f%%\n", percent);
        int bars = (int)(percent / 5);
        if (bars > 20) bars = 20;
        printf("[ "); for (int i = 0; i < bars; i++) putchar('#');
        for (int i = bars; i < 20; i++) putchar('-');
        printf(" ]\n");
    }
}

/* ===== boss_battle(1).c ===== */
void bossBattle(const Expense expenses[], int count, double budget) {
    double spent = calculateTotal(expenses, count), remaining = budget - spent;
    printTitle("FINANCIAL BOSS BATTLE");
    puts("BOSS: Unexpected hostel expense! Your budget is under attack.");
    double emergency = getDouble("Unexpected expense amount (Rs): ", 0.01, 100000000.0);
    puts("Choose your strategy:");
    puts("1. Pay from remaining budget");
    puts("2. Cut optional spending (simulate saving 20% of optional expenses)");
    puts("3. Use emergency reserve (Rs500)");
    int choice = getInt("Strategy: ", 1, 3);
    double saved = 0;
    if (choice == 2) {
        for (int i = 0; i < count; i++) if (!expenses[i].essential) saved += expenses[i].amount * 0.20;
        printf("Simulated savings from optional spending: Rs%.2f\n", saved);
    } else if (choice == 3) saved = RESERVE_DEFAULT;
    double finalBalance = remaining + saved - emergency;
    printf("Starting balance: Rs%.2f\nEmergency cost: Rs%.2f\nSupport: Rs%.2f\n", remaining, emergency, saved);
    if (finalBalance >= 0) printf("BOSS DEFEATED! You survive with Rs%.2f.\n", finalBalance);
    else printf("BUDGET KO. Deficit: Rs%.2f. Try a different strategy.\n", -finalBalance);
    puts("This was a simulation. Your actual transaction history was not changed.");
}

/* ===== main(2).c ===== */
int main(void) {
    Expense expenses[MAX_EXPENSES];
    Rule rules[MAX_RULES];
    int count = 0, nextID = 1, ruleCount = 0, choice;
    double budget = loadBudget();
    loadExpenses(expenses, &count, &nextID);
    loadRules(rules, &ruleCount);
    puts("SPENDEMIC loaded. Your money's questionable decisions are ready for review.");
    do {
        showMenu();
        choice = getInt("Choose an option (1-12): ", 1, 12);
        switch (choice) {
            case 1:
                budget = getDouble("Enter monthly budget (Rs): ", 0.01, 100000000.0);
                saveBudget(budget);
                puts("Budget saved.");
                break;
            case 2:
                addExpense(expenses, &count, &nextID, budget);
                saveExpenses(expenses, count);
                break;
            case 3: viewExpenses(expenses, count); break;
            case 4: expenseDetector(expenses, count); break;
            case 5: affordabilityCheck(expenses, count, budget); break;
            case 6: budgetForecast(expenses, count, budget); break;
            case 7: spendingTimeline(expenses, count); break;
            case 8: bossBattle(expenses, count, budget); break;
            case 9:
                undoLastExpense(expenses, &count, &nextID);
                saveExpenses(expenses, count);
                break;
            case 10: dashboard(expenses, count, budget); break;
            case 11:
                addCustomRule(rules, &ruleCount);
                break;
            case 12:
                saveExpenses(expenses, count);
                saveBudget(budget);
                puts("Saved. Goodbye. Track the damage responsibly.");
                break;
        }
        if (choice != 12) pauseScreen();
    } while (choice != 12);
    return 0;
}
