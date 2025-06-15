#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ISSUE_MENU_OPTIONS 2

static const char *issue_menu_items[ISSUE_MENU_OPTIONS] = {
    "Issue Book",
    "Exit"
};

typedef struct
{
    int day, month, year;
} Date;

void format_date(Date d, char *buffer, size_t size)
{
    snprintf(buffer, size, "%02d-%02d-%04d", d.day, d.month, d.year);
}

void remove_newLine(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        if (str[i] == '\n') { str[i] = '\0'; break; }
}

int get_field_input_issue(int y, int x, char *buffer, int max_len)
{
    move(y, x);
    noecho();
    int i = 0;
    while (i < max_len - 1)
    {
        int ch = getch();
        if (ch == 27) // ESC
        {
            int menu_start_y = 4;
            int note_row = menu_start_y + ISSUE_MENU_OPTIONS * 2 + 2;
            mvprintw(note_row + 1, 4, "Exiting");
            refresh();
            for (int i = 0; i < 3; ++i)
            {
                napms(300); // Delay for 300ms
                printw(".");
                refresh();
            }
            napms(300); // Final pause
            // return 1;
            endwin();
            return 0;
        }
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

int prompt_and_get_input_issue(int row, int col, const char *label, char *buffer, int max_len)
{
    mvprintw(row, col, "%s", label);
    return get_field_input_issue(row, col + strlen(label), buffer, max_len);
}

int is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int is_valid_date(int day, int month, int year)
{
    if (year < 1 || month < 1 || month > 12 || day < 1)
        return 0;
    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (is_leap_year(year)) days_in_month[1] = 29;
    return day <= days_in_month[month - 1];
}

int input_valid_date_ncurses(int y, int x, Date *d)
{
    char date_str[32];
    while (1)
    {
        mvprintw(y, x, "Enter date (DD MM YYYY): ");
        clrtoeol();
        if (!get_field_input_issue(y, x + strlen("Enter date (DD MM YYYY): "), date_str, sizeof(date_str)))
            return 0;
        int day, month, year;
        if (sscanf(date_str, "%d %d %d", &day, &month, &year) == 3 && is_valid_date(day, month, year))
        {
            d->day = day; d->month = month; d->year = year;
            return 1;
        }
        mvprintw(y + 1, x, "Invalid date. Please enter a valid calendar date.");
        clrtoeol();
    }
}

int application_ncurses()
{
    FILE *member;
    char name[200], id[200], contact[200], dep[200], session[200], data[200];
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    member = fopen("member.csv", "a+");
    if (member == NULL)
    {
        clear();
        mvprintw(2, 2, "Unable to open file");
        getch();
        return 1;
    }

    // Write header if file is empty
    fseek(member, 0, SEEK_END);
    long size = ftell(member);
    if (size == 0)
    {
        fprintf(member, "Name,ID,Department,Session,Contact\n");
        fflush(member);
    }

    while (1)
    {
        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Membership Application ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        // Name
        mvprintw(3, 4, "1. Name: ");
        if (!get_field_input_issue(3, 20, name, sizeof(name)))
        {
            fclose(member);
            return 0;
        }

        // ID (unique)
        int is_unique;
        do
        {
            is_unique = 0;
            mvprintw(4, 4, "2. Member ID: ");
            if (!get_field_input_issue(4, 20, id, sizeof(id)))
            {
                fclose(member);
                return 0;
            }

            fflush(member);
            rewind(member);
            fgets(data, sizeof(data), member); // Skip header
            while (fgets(data, sizeof(data), member))
            {
                char *tmp = strdup(data);
                char *name_tok = strtok(tmp, ",");
                char *user_id = strtok(NULL, ",");
                if (user_id && strcmp(id, user_id) == 0)
                {
                    mvprintw(20, 4, "Member ID already exists. Enter a different ID.");
                    is_unique = 1;
                    free(tmp);
                    break;
                }
                free(tmp);
            }
            if (is_unique)
            {
                move(4, 20);
                clrtoeol();
            }
        } while (is_unique);

        // Department
        mvprintw(5, 4, "3. Department: ");
        if (!get_field_input_issue(5, 20, dep, sizeof(dep)))
        {
            fclose(member);
            return 0;
        }

        // Session
        mvprintw(6, 4, "4. Session: ");
        if (!get_field_input_issue(6, 20, session, sizeof(session)))
        {
            fclose(member);
            return 0;
        }

        // Contact
        mvprintw(7, 4, "5. Contact: ");
        if (!get_field_input_issue(7, 20, contact, sizeof(contact)))
        {
            fclose(member);
            return 0;
        }

        if (strlen(name) == 0 || strlen(id) == 0 || strlen(dep) == 0 ||
            strlen(session) == 0 || strlen(contact) == 0)
        {
            mvprintw(10, 4, "All fields must be filled! Press any key to try again.");
            getch();
        }
        else
        {
            break; // all fields are filled
        }
    }

    // Save member data
    fseek(member, 0, SEEK_END);
    fprintf(member, "%s,%s,%s,%s,%s\n", name, id, dep, session, contact);
    fflush(member);

    fclose(member);
    mvprintw(13, 4, "Membership application successful!");
    mvprintw(15, 4, "Press any key to return...");
    getch();
    return 0;
}

int member_verification_ncurses(int y, int x, char *id_verify)
{
    char data[200], choice_line[10];
    int found = 0;
    FILE *member = fopen("member.csv", "r");
    if (!member)
    {
        mvprintw(y, x, "Unable to open member file.");
        getch();
        return 0;
    }

    echo();
    mvprintw(y, x, "Enter member ID to verify: ");
    if (!get_field_input_issue(y, x + strlen("Enter member ID to verify: "), id_verify, 199))
    {
        fclose(member);
        return 0;
    }
    noecho();

    rewind(member);
    fgets(data, sizeof(data), member); // Skip header

    while (fgets(data, sizeof(data), member))
    {
        char *name = strtok(data, ",");
        char *user_id = strtok(NULL, ",");
        if (user_id && strcmp(user_id, id_verify) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(member);

    if (!found)
    {
        mvprintw(y + 1, x, "You have to buy membership to issue books.");
        mvprintw(y + 2, x, "Do you want to buy membership? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        noecho();
        if (choice_line[0] == 'y' || choice_line[0] == 'Y')
        {
            application_ncurses();
            // After registration, ask for ID again
            mvprintw(y + 3, x, "Enter member ID to verify: ");
            if (!get_field_input_issue(y + 3, x + strlen("Enter member ID to verify: "), id_verify, 199))
                return 0;
            return 1;
        }
        else
        {
            mvprintw(y + 3, x, "Unable to issue a book!");
            mvprintw(y + 4, x, "Press any key to return...");
            getch();
            return 0;
        }
    }
    return 1;
}

int issue_book_ncurses()
{
    FILE *stock = fopen("books.csv", "r");
    FILE *issue_file = fopen("issue.csv", "a+");
    if (!stock || !issue_file)
    {
        clear();
        mvprintw(2, 2, "Unable to open file!");
        getch();
        if (stock) fclose(stock);
        if (issue_file) fclose(issue_file);
        return 1;
    }

    // Write header if file is empty
    fseek(issue_file, 0, SEEK_END);
    long size = ftell(issue_file);
    if (size == 0)
    {
        fprintf(issue_file, "Member ID,Book ID,Book Title,Book Author,Issue Date\n");
        fflush(issue_file);
    }

    char id[200], book_id[200], book_title[200], data_book[300], id_verify[200], book_author[200];
    Date issue_date;
    char date_str[20];

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    clear();
    box(stdscr, 0, 0);
    const char *header = "=== Issue Book ===";
    int start_col = (max_x - strlen(header)) / 2;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, start_col, "%s", header);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // Member verification
    if (!member_verification_ncurses(5, 4, id_verify))
    {
        fclose(stock);
        fclose(issue_file);
        return 0;
    }

    // Book ID input
    if (!prompt_and_get_input_issue(9, 4, "Enter Book ID to issue: ", book_id, sizeof(book_id)))
    {
        fclose(stock);
        fclose(issue_file);
        return 0;
    }

    int found = 0;
    rewind(stock);
    fgets(data_book, sizeof(data_book), stock); // Skip header

    while (fgets(data_book, sizeof(data_book), stock))
    {
        char *tmp = strdup(data_book);
        char *curr_book_id = strtok(tmp, ",");
        char *curr_title = strtok(NULL, ",");
        char *curr_author = strtok(NULL, ",");
        remove_newLine(curr_author);
        char *issue_date_str = strtok(NULL, ","); // Not used but read to maintain structure
        if (curr_book_id && strcmp(curr_book_id, book_id) == 0)
        {
            strcpy(book_title, curr_title ? curr_title : "");
            strcpy(book_author, curr_author ? curr_author : "");
            found = 1;
            free(tmp);
            break;
        }
        free(tmp);
    }

    if (!found)
    {
        mvprintw(10, 4, "Book not found in stock.");
        fclose(stock);
        fclose(issue_file);
        getch();
        return 0;
    }

    // Date input
    if (!input_valid_date_ncurses(10, 4, &issue_date))
    {
        fclose(stock);
        fclose(issue_file);
        return 0;
    }
    format_date(issue_date, date_str, sizeof(date_str));

    // Save issue record
    fprintf(issue_file, "%s,%s,%s,%s,%s\n", id_verify, book_id, book_title, book_author, date_str);
    fflush(issue_file);

    // Remove the issued book from books.csv
    fclose(stock); // Close the original books.csv file first
    FILE *books_in = fopen("books.csv", "r");
    FILE *books_out = fopen("temp_books.csv", "w");
    if (books_in && books_out) {
        char line[512];
        // Copy header
        if (fgets(line, sizeof(line), books_in)) {
            fputs(line, books_out);
        }
        // Copy all books except the issued one
        while (fgets(line, sizeof(line), books_in)) {
            char line_copy[512];
            strcpy(line_copy, line);
            char *curr_book_id = strtok(line_copy, ",");
            if (!curr_book_id || strcmp(curr_book_id, book_id) != 0) {
                fputs(line, books_out);
            }
        }
        fclose(books_in);
        fclose(books_out);
        remove("books.csv");
        rename("temp_books.csv", "books.csv");
    }

    mvprintw(13, 4, "Book issued successfully!");
    mvprintw(15, 4, "Press any key to return...");
    fclose(issue_file);
    getch();
    return 0;
}

void issue_books()
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
        const char *header = "=== Issue Portal ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 30) / 2;

        for (int i = 0; i < ISSUE_MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", issue_menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", issue_menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }

        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + ISSUE_MENU_OPTIONS * 2 + 2;
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + ISSUE_MENU_OPTIONS) % ISSUE_MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % ISSUE_MENU_OPTIONS;
            break;
        case 10: // Enter
            choice = highlight;
            break;
        case 27: // ESC key
            mvprintw(note_row + 2, 4, "Exiting");
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
                issue_book_ncurses();
                break;
            case 1:
                mvprintw(note_row + 2, 4, "Exiting");
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