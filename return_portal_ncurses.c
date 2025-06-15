#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETURN_MENU_OPTIONS 2

static const char *return_menu_items[RETURN_MENU_OPTIONS] = {
    "Return Book",
    "Exit"
};

typedef struct
{
    int day, month, year;
} Date;

Date parseDate(const char *dateStr)
{
    Date d = {0, 0, 0};
    if (sscanf(dateStr, "%d-%d-%d", &d.day, &d.month, &d.year) != 3)
        sscanf(dateStr, "%d %d %d", &d.day, &d.month, &d.year);
    if (d.year < 100) d.year += 2000;
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

// Returns number of days from 01-01-0001 to the given date
long days_from_start(Date d)
{
    static int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    long days = d.day;

    // Add days for months in current year
    for (int i = 0; i < d.month - 1; i++)
        days += monthDays[i];

    // Add leap day if after Feb in a leap year
    if (d.month > 2 && isLeapYear(d.year))
        days += 1;

    // Add days for years
    days += (d.year - 1) * 365L;
    // Add leap years
    days += (d.year - 1) / 4 - (d.year - 1) / 100 + (d.year - 1) / 400;

    return days;
}

// Returns days between d1 and d2 (d2 - d1)
int days_between(Date d1, Date d2)
{
    long days1 = days_from_start(d1);
    long days2 = days_from_start(d2);
    return (int)(days2 - days1);
}

int get_field_input_return(int y, int x, char *buffer, int max_len)
{
    move(y, x);
    noecho();
    int i = 0;
    while (i < max_len - 1)
    {
        int ch = getch();
        if (ch == 27) // ESC
            return 0;
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8)
        {
            if (i > 0)
            {
                i--;
                buffer[i] = '\0';
                mvaddch(y, x + i, ' ' | COLOR_PAIR(1));
                move(y, x + i);
            }
        }
        else if (ch == '\n' || ch == '\r')
            break;
        else if (ch >= 32 && ch <= 126)
        {
            buffer[i++] = ch;
            buffer[i] = '\0';
            mvaddch(y, x + i - 1, ch | COLOR_PAIR(1));
        }
    }
    buffer[i] = '\0';
    return 1;
}

int prompt_and_get_input_return(int row, int col, const char *label, char *buffer, int max_len)
{
    mvprintw(row, col, "%s", label);
    return get_field_input_return(row, col + strlen(label), buffer, max_len);
}

// Reads issue.csv and finds the record, returns 1 if found and fills out params
int isBookIssuedToMember(const char *bookID, const char *memberID, Date *issueDate, char *bookTitleOut, char *bookAuthorOut)
{
    FILE *f = fopen("issue.csv", "r");
    if (!f)
        return 0;

    char line[512];
    int found = 0;
    fgets(line, sizeof(line), f); // skip header

    while (fgets(line, sizeof(line), f))
    {
        char *linecpy = strdup(line);
        char *mid = strtok(linecpy, ",");
        char *bid = strtok(NULL, ",");
        char *btitle = strtok(NULL, ",");
        char *bauthor = strtok(NULL, ",");
        char *dateStr = strtok(NULL, ",\n");
        if (mid && bid && dateStr && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0)
        {
            *issueDate = parseDate(dateStr);
            strcpy(bookTitleOut, btitle ? btitle : "Unknown");
            strcpy(bookAuthorOut, bauthor ? bauthor : "Unknown");
            found = 1;
            free(linecpy);
            break;
        }
        free(linecpy);
    }
    fclose(f);
    return found;
}

void removeFromIssueFile(const char *bookID, const char *memberID)
{
    FILE *f = fopen("issue.csv", "r");
    FILE *temp = fopen("temp_issue.csv", "w");
    if (!f || !temp)
        return;

    char line[512];
    fgets(line, sizeof(line), f); // header
    fputs(line, temp);
    while (fgets(line, sizeof(line), f))
    {
        char linecpy[512];
        strcpy(linecpy, line);
        char *mid = strtok(linecpy, ",");
        char *bid = strtok(NULL, ",");
        // skip title/author/date
        if (!(bid && mid && strcmp(bid, bookID) == 0 && strcmp(mid, memberID) == 0))
            fputs(line, temp);
    }
    fclose(f);
    fclose(temp);
    remove("issue.csv");
    rename("temp_issue.csv", "issue.csv");
}

void addFine(const char *bookID, const char *memberID, int daysLate, int fine, Date DOI, Date DOR)
{
    FILE *f = fopen("fine.csv", "a+");
    if (!f)
        return;

    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
        fprintf(f, "Book ID,Member ID,Date of Issue,Date of Return,Days Late,Fine\n");

    char doiStr[16], dorStr[16];
    snprintf(doiStr, sizeof(doiStr), "%02d-%02d-%04d", DOI.day, DOI.month, DOI.year);
    snprintf(dorStr, sizeof(dorStr), "%02d-%02d-%04d", DOR.day, DOR.month, DOR.year);

    fprintf(f, "%s,%s,%s,%s,%d,%d\n", bookID, memberID, doiStr, dorStr, daysLate, fine);
    fclose(f);
}

int return_book_ncurses()
{
    char bookID[50], memberID[50], returnDateStr[50];
    Date issueDate, returnDate;
    char bookTitle[100], bookAuthor[100];
    int daysBetween;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    clear();
    box(stdscr, 0, 0);
    const char *header = "=== Return Book ===";
    int start_col = (max_x - strlen(header)) / 2;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, start_col, "%s", header);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // Book ID
    if (!prompt_and_get_input_return(3, 4, "Enter Book ID to return: ", bookID, sizeof(bookID)))
        return 0;

    // Member ID
    if (!prompt_and_get_input_return(4, 4, "Enter Member ID: ", memberID, sizeof(memberID)))
        return 0;

    if (!isBookIssuedToMember(bookID, memberID, &issueDate, bookTitle, bookAuthor))
    {
        mvprintw(6, 4, "This book was not issued to this member.");
        mvprintw(8, 4, "Press any key to return...");
        getch();
        return 0;
    }

    // Return Date
    while (1)
    {
        if (!prompt_and_get_input_return(6, 4, "Enter Return Date (DD-MM-YYYY or DD MM YYYY): ", returnDateStr, sizeof(returnDateStr)))
            return 0;
        returnDate = parseDate(returnDateStr);

        if (!isValidDate(returnDate))
        {
            mvprintw(7, 4, "Invalid return date. Please enter a valid date.");
            continue;
        }

        daysBetween = days_between(issueDate, returnDate);

        if (daysBetween < 0)
        {
            mvprintw(7, 4, "Return date cannot be before issue date.");
            continue;
        }
        break;
    }

    removeFromIssueFile(bookID, memberID);

    // Restore book to books.csv
    FILE *booksFile = fopen("books.csv", "a");
    if (booksFile)
    {
        fprintf(booksFile, "%s,%s,%s\n", bookID, bookTitle, bookAuthor);
        fclose(booksFile);
        mvprintw(9, 4, "Book restored to books.csv successfully.");
    }
    else
    {
        mvprintw(9, 4, "Error: Could not open books.csv to restore the book.");
    }

    mvprintw(10, 4, "Days between issue and return: %d", daysBetween);

    if (daysBetween <= 7)
    {
        mvprintw(11, 4, "Book returned on time. No fine.");
    }
    else
    {
        int fine = (daysBetween - 7) * 100;
        addFine(bookID, memberID, daysBetween - 7, fine, issueDate, returnDate);
        mvprintw(11, 4, "Book returned late. Fine of Rs.%d added.", fine);
    }

    mvprintw(13, 4, "Press any key to return...");
    getch();
    return 0;
}

void return_books()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_WHITE);  // Header
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // Highlighted
    init_pair(3, COLOR_BLACK, COLOR_WHITE); // Normal

    int highlight = 0, choice = -1, ch;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    while (1)
    {
        clear();
        attron(COLOR_PAIR(1));
        wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
        attroff(COLOR_PAIR(1));
        const char *header = "=== Return Portal ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 30) / 2;

        for (int i = 0; i < RETURN_MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", return_menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", return_menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }

        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + RETURN_MENU_OPTIONS * 2 + 2;
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + RETURN_MENU_OPTIONS) % RETURN_MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % RETURN_MENU_OPTIONS;
            break;
        case 10: // Enter
            choice = highlight;
            break;
        case 27: // ESC key
            mvprintw(note_row + 1, 4, "Exiting");
            refresh();
            for (int i = 0; i < 3; ++i)
            {
                napms(300);
                printw(".");
                refresh();
            }
            napms(300);
            endwin();
            return;
        default:
            break;
        }

        if (choice != -1)
        {
            endwin();
            switch (choice)
            {
            case 0:
                return_book_ncurses();
                break;
            case 1:
                mvprintw(note_row + 1, 4, "Exiting");
                refresh();
                for (int i = 0; i < 3; ++i)
                {
                    napms(300);
                    printw(".");
                    refresh();
                }
                napms(300);
                endwin();
                return;
            }
            // Re-initialize ncurses for the menu after stdio functions
            initscr();
            noecho();
            cbreak();
            keypad(stdscr, TRUE);
            start_color();
            init_pair(1, COLOR_BLUE, COLOR_WHITE);
            init_pair(2, COLOR_WHITE, COLOR_BLUE);
            init_pair(3, COLOR_BLACK, COLOR_WHITE);
            getmaxyx(stdscr, max_y, max_x);
            choice = -1;
        }
    }
    endwin();
    return;
}