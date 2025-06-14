#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int day, month, year;
} Date;

Date parseDate(char *dateStr)
{
    Date d;
    if (sscanf(dateStr, "%d-%d-%d", &d.day, &d.month, &d.year) != 3)
    {
        sscanf(dateStr, "%d %d %d", &d.day, &d.month, &d.year);
    }
    if (d.year < 100)
    {
        d.year += 2000;
    }
    return d;
}

int isLeapYear(int year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int isValidDate(Date d)
{
    if (d.month < 1 || d.month > 12 || d.day < 1)
        return 0;

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (d.month == 2 && isLeapYear(d.year))
        return d.day <= 29;

    return d.day <= daysInMonth[d.month];
}

int countLeapYears(Date d)
{
    int years = d.year;
    if (d.month <= 2)
    {
        years--;
    }
    return years / 4 - years / 100 + years / 400;
}

int dateToDays(Date d)
{
    static int monthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = d.year * 365 + d.day;
    for (int i = 1; i < d.month; i++)
    {
        days += monthDays[i];
    }
    days += countLeapYears(d);
    return days;
}

// Updated to return book title and author
int isBookIssuedToMember(char *bookID, char *memberID, Date *issueDate, char *bookTitleOut, char *bookAuthorOut)
{
    FILE *f = fopen("issue.csv", "r");
    if (!f)
        return 0;

    char line[300];
    int found = 0;

    while (fgets(line, sizeof(line), f))
    {
        char *mid = strtok(line, ",");
        char *btitle = strtok(NULL, ",");
        char *author = strtok(NULL, ",\n");
        char *bid = strtok(NULL, ",");
        char *dateStr = strtok(NULL, ",");

        if (bid && mid && dateStr && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0)
        {
            *issueDate = parseDate(dateStr);
            if (btitle)
                strcpy(bookTitleOut, btitle);
            if (author)
                strcpy(bookAuthorOut, author);
            else
                strcpy(bookAuthorOut, "Unknown");
            found = 1;
            break;
        }
    }

    fclose(f);
    return found;
}

void removeFromIssueFile(char *bookID, char *memberID)
{
    FILE *f = fopen("issue.csv", "r");
    FILE *temp = fopen("temp_issue.csv", "w");
    if (!f || !temp)
    {
        printf("File error while updating issue.csv\n");
        return;
    }

    char line[300];
    fgets(line, sizeof(line), f); // Read header
    fputs(line, temp);            // Write header to temp file
    while (fgets(line, sizeof(line), f))
    {
        char originalLine[300];
        strcpy(originalLine, line);

        char *mid = strtok(line, ",");
        char *btitle = strtok(NULL, ",");
        char *author = strtok(NULL, ",\n");
        char *bid = strtok(NULL, ",");

        if (!(bid && mid && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0))
        {
            fputs(originalLine, temp);
        }
    }

    fclose(f);
    fclose(temp);
    remove("issue.csv");
    rename("temp_issue.csv", "issue.csv");
}

void addFine(char *bookID, char *memberID, int daysLate, int fine, Date DOI, Date DOR)
{
    FILE *f = fopen("fine.csv", "a+");
    if (!f)
    {
        printf("Unable to open fine.csv\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
    {
        fprintf(f, "Book ID,Member ID,Date of Issue,Date of Return,Days Late,Fine\n");
    }

    char doiStr[11], dorStr[11];
    snprintf(doiStr, sizeof(doiStr), "%02d-%02d-%04d", DOI.day, DOI.month, DOI.year);
    snprintf(dorStr, sizeof(dorStr), "%02d-%02d-%04d", DOR.day, DOR.month, DOR.year);

    fprintf(f, "%s,%s,%s,%s,%d,%d\n", bookID, memberID, doiStr, dorStr, daysLate, fine);
    fclose(f);
}

void return_books()
{
    char bookID[50], memberID[50], returnDateStr[50], choice;
    Date issueDate, returnDate;
    char bookTitle[100], bookAuthor[100];
    int daysBetween;

    printf("\n\n\t\t\t=== Return Books ===\n\n");

    do
    {
        printf("Enter Book ID to return: ");
        scanf("%s", bookID);

        printf("Enter Member ID: ");
        scanf("%s", memberID);

        if (!isBookIssuedToMember(bookID, memberID, &issueDate, bookTitle, bookAuthor))
        {
            printf("This book was not issued to this member.\n");
            continue;
        }

        while (1)
        {
            printf("Enter Return Date (DD-MM-YYYY or DD MM YYYY): ");
            scanf(" %[^\n]", returnDateStr);

            returnDate = parseDate(returnDateStr);

            if (!isValidDate(returnDate))
            {
                printf("Invalid return date. Please enter a valid date.\n");
                continue;
            }

            daysBetween = dateToDays(returnDate) - dateToDays(issueDate);

            if (daysBetween < 0)
            {
                printf("Return date cannot be before issue date.\n");
                continue;
            }

            break; // valid and logical return date
        }


        removeFromIssueFile(bookID, memberID);

        // Restore book to books.csv
        FILE *booksFile = fopen("books.csv", "a");
        if (booksFile)
        {
            fprintf(booksFile, "%s,%s,%s\n", bookID, bookTitle, bookAuthor);
            fclose(booksFile);
            printf("Book restored to books.csv successfully.\n");
        }
        else
        {
            printf("Error: Could not open books.csv to restore the book.\n");
        }

        printf("Days between issue and return: %d\n", daysBetween);

        if (daysBetween <= 7)
        {
            printf("Book returned on time. No fine.\n");
        }
        else
        {
            int fine = (daysBetween - 7) * 100;
            addFine(bookID, memberID, daysBetween - 7, fine, issueDate, returnDate);
            printf("Book returned late. Fine of Rs.%d added.\n", fine);
        }

        printf("\nDo you want to return another book? (y/n): ");
        scanf(" %c", &choice);
        getchar(); // consume newline

    } while (choice == 'y' || choice == 'Y');
}
