#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int day, month, year;
} Date;

Date parseDate(char *dateStr) {
    Date d;
    // Try to parse either with '-' or space separators
    if (sscanf(dateStr, "%d-%d-%d", &d.day, &d.month, &d.year) != 3) {
        sscanf(dateStr, "%d %d %d", &d.day, &d.month, &d.year);
    }
    // Convert 2-digit year to 4-digit if needed
    if (d.year < 100) {
        d.year += 2000;
    }
    return d;
}

int countLeapYears(Date d) {
    int years = d.year;
    if (d.month <= 2) 
    {
        years--;
    }
    return years / 4 - years / 100 + years / 400;
}

int dateToDays(Date d) {
    static int monthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int days = d.year * 365 + d.day;
    for (int i = 1; i < d.month; i++) {
        days += monthDays[i];
    }
    days += countLeapYears(d);
    return days;
}


int isBookIssuedToMember(char *bookID, char *memberID, Date *issueDate) {
    FILE *f = fopen("issue.csv", "r");
    if (!f) return 0;

    char line[200];
    int found = 0;
    // fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f)) {
        char *mid = strtok(line, ",");
        char *btitle = strtok(NULL, ",");
        char *bid = strtok(NULL, ",");
        char *dateStr = strtok(NULL, ",\n");

        if (bid && mid && dateStr && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0) {
            *issueDate = parseDate(dateStr);
            found = 1;
            break;
        }
    }

    fclose(f);
    return found;
}

void removeFromIssueFile(char *bookID, char *memberID) {
    FILE *f = fopen("issue.csv", "r");
    FILE *temp = fopen("temp_issue.csv", "w");

    char line[200];

    while (fgets(line, sizeof(line), f)) {
        char originalLine[200];
        strcpy(originalLine, line);

        char *mid = strtok(line, ",");
        char *btitle = strtok(NULL, ",");
        char *bid = strtok(NULL, ",");

        if (!(bid && mid && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0)) {
            fprintf(temp, "%s", originalLine);
        }
    }

    fclose(f);
    fclose(temp);
    remove("issue.csv");
    rename("temp_issue.csv", "issue.csv");
}


void addFine(char *bookID, char *memberID, int daysLate, int fine) {
    FILE *f = fopen("fine.csv", "a");
    fprintf(f, "%s,%s,%d,%d\n", bookID, memberID, daysLate, fine);
    fclose(f);
}

void return_books() {
    char bookID[20], memberID[20], returnDateStr[20], choice;
    Date issueDate, returnDate;
    int daysBetween;
    do {
        printf("Enter Book ID to return: ");
        scanf("%s", bookID);

        printf("Enter Member ID: ");
        scanf("%s", memberID);

        printf("Enter Return Date (DD-MM-YYYY or DD MM YYYY): ");
        scanf(" %19[^\n]", returnDateStr);  // read until newline

        if (!isBookIssuedToMember(bookID, memberID, &issueDate)) {
            printf("This book was not issued to this member.\n");
            continue;  // don't exit, allow retry
        }

        returnDate = parseDate(returnDateStr);
        daysBetween = dateToDays(returnDate) - dateToDays(issueDate);

        if (daysBetween < 0) {
            printf("Return date cannot be before issue date. Please enter correct dates.\n");
            continue;
        }

        removeFromIssueFile(bookID, memberID);

        printf("Days between: %d\n", daysBetween);
        if (daysBetween <= 7) {
            printf("Book returned on time. No fine.\n");
        } else {
            int fine = (daysBetween - 7) * 100;
            addFine(bookID, memberID, daysBetween - 7, fine);
            printf("Book returned late. Fine of Rs.%d added.\n", fine);
        }
        printf("Do you want to return another book? (y/n): ");
        scanf(" %c", &choice);
        getchar();  // consume newline
    } while (choice == 'y' || choice == 'Y');

}
