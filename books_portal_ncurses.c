#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "books.h"

#define BOOK_MENU_OPTIONS 5

static const char *book_menu_items[BOOK_MENU_OPTIONS] = {
    "Add Book",
    "Search Book",
    "Delete Book",
    "Display Books",
    "Exit"};

// Utility to remove newline from fgets input
void static remove_newline(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = '\0';
            break;
        }
    }
}

// General field input with ESC support
int get_field_input_book(int y, int x, char *buffer, int max_len)
{
    move(y, x);
    echo();
    int i = 0;
    while (i < max_len - 1)
    {
        int ch = getch();
        if (ch == 27)
        { // ESC key pressed
            int menu_start_y = 4;
            int note_row = menu_start_y + BOOK_MENU_OPTIONS * 2 + 2;
            mvprintw(note_row + 1, 2, "Exiting");
            refresh();
            for (int i = 0; i < 3; ++i)
            {
                napms(300);
                printw(".");
                refresh();
            }
            napms(300);
            endwin();
            return 0;
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8)
        {
            if (i > 0)
            {
                i--;
                buffer[i] = '\0';
                mvaddch(y, x + i, ' ');
                move(y, x + i);
            }
        }
        else if (ch == '\n' || ch == '\r')
        {
            break;
        }
        else
        {
            buffer[i++] = ch;
            buffer[i] = '\0';
            mvaddch(y, x + i - 1, ch);
        }
    }
    buffer[i] = '\0';
    return 1;
}

int prompt_and_get_input_book(int row, int col, const char *label, char *buffer, int max_len)
{
    mvprintw(row, col, "%s", label);
    return get_field_input_book(row, col + strlen(label), buffer, max_len);
}

// Add Book
int book_addition()
{
    FILE *book;
    char title[200], id[200], author[200], publisher[200], year[200], choice_line[10], data[200];
    char choice;
    int menu_start_y = 4;
    int note_row = menu_start_y + BOOK_MENU_OPTIONS * 2 + 2;

    book = fopen("books.csv", "a+");
    if (book == NULL)
    {
        clear();
        mvprintw(2, 2, "Unable to open file");
        getch();
        return 1;
    }

    // Write header if file is empty
    fseek(book, 0, SEEK_END);
    long size = ftell(book);
    if (size == 0)
    {
        fprintf(book, "ID,Title,Author\n");
    }

    do
    {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        while (1)
        {
            clear();
            box(stdscr, 0, 0);
            const char *header = "=== Add New Book ===";
            int start_col = (max_x - strlen(header)) / 2;
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(1, start_col, "%s", header);
            attroff(COLOR_PAIR(1) | A_BOLD);

            while (1)
            {
                mvprintw(4, 4, "2. Book ID: ");
                if (!get_field_input_book(4, 20, id, sizeof(id)))
                    return 0;

                int is_unique = 1;
                rewind(book);
                fgets(data, sizeof(data), book); // Skip header
                while (fgets(data, sizeof(data), book))
                {
                    char *tmp = strdup(data);
                    char *book_id = strtok(tmp, ",");
                    if (book_id && strcmp(id, book_id) == 0)
                    {
                        is_unique = 0;
                        free(tmp);
                        break;
                    }
                    free(tmp);
                }

                if (!is_unique)
                {
                    mvprintw(20, 4, "Book ID already exists. Enter a different ID.");
                    move(4, 20);
                    clrtoeol();
                    continue;
                }
                else
                {
                    break; // ID is unique, exit loop
                }
            }

            // Title
            mvprintw(5, 4, "1. Title: ");
            if (!get_field_input_book(5, 20, title, sizeof(title)))
                return 0;

            // Author
            mvprintw(6, 4, "3. Author: ");
            if (!get_field_input_book(6, 20, author, sizeof(author)))
                return 0;

            // Publisher

            if (strlen(title) == 0 || strlen(id) == 0 || strlen(author) == 0)
            {
                mvprintw(9, 4, "All fields must be filled!");
            }
            else
            {
                break;
            }
        }

        fprintf(book, "%s,%s,%s\n", id, title, author);

        mvprintw(11, 4, "Do you want to enter another book? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');

    fclose(book);
    mvprintw(13, 4, "Book(s) added successfully!");
    return 0;
}

// Search Book
int book_search()
{
    FILE *book;
    book = fopen("books.csv", "r");

    if (book == NULL)
    {
        clear();
        mvprintw(2, 2, "Unable to open file");
        getch();
        return 1;
    }

    char id[200], data[300], choice, choice_line[10];
    int found;

    do
    {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Search Book ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        echo();
        if (!prompt_and_get_input_book(3, 4, "Enter the Book ID: ", id, sizeof(id)))
            return 0;
        found = 0;

        rewind(book);
        fgets(data, sizeof(data), book);

        for (int i = 5; i <= 11; ++i)
        {
            move(i, 0);
            clrtoeol();
        }

        while (fgets(data, sizeof(data), book))
        {
            remove_newline(data);

            char *tmp = strdup(data);
            char *book_id = strtok(tmp, ",");
            char *title = strtok(NULL, ",");
            char *author = strtok(NULL, ",");

            if (book_id && strcmp(book_id, id) == 0)
            {
                mvprintw(5, 4, "Book found:");
                mvprintw(6, 6, "ID: %s", book_id);
                mvprintw(7, 6, "Title: %s", title);
                mvprintw(8, 6, "Author: %s", author);

                found = 1;
                free(tmp);
                break;
            }
            free(tmp);
        }

        if (!found)
        {
            mvprintw(5, 4, "Book with ID %s not found.", id);
        }

        mvprintw(13, 4, "Do you want to find another book? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        noecho();
        choice = choice_line[0];

    } while (choice == 'y' || choice == 'Y');

    fclose(book);
    return 0;
}

// Delete Book
int book_delete()
{
    char id[200], data[200], choice, choice_line[10];

    do
    {
        int found = 0;
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        FILE *Book = fopen("books.csv", "r");
        if (Book == NULL)
        {
            clear();
            mvprintw(2, 2, "Unable to open file");
            getch();
            return 1;
        }

        FILE *temp = fopen("temp_books.csv", "w");
        if (temp == NULL)
        {
            clear();
            mvprintw(3, 2, "Unable to create temp file");
            fclose(Book);
            getch();
            return 1;
        }

        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Delete Book ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        echo();
        if (!prompt_and_get_input_book(3, 4, "Enter the Book ID: ", id, sizeof(id)))
            return 0;

        while (fgets(data, sizeof(data), Book))
        {
            char line_copy[200];
            strncpy(line_copy, data, sizeof(line_copy));
            line_copy[sizeof(line_copy) - 1] = '\0';

            char *book_id = strtok(line_copy, ",");
            char *title = strtok(NULL, ",");
            char *author = strtok(NULL, ",");

            if (book_id && strcmp(book_id, id) != 0)
            {
                fputs(data, temp);
            }
            else if (book_id && strcmp(book_id, id) == 0)
            {
                found = 1;
            }
        }

        fclose(Book);
        fclose(temp);

        if (found)
        {
            remove("books.csv");
            rename("temp_books.csv", "books.csv");
            mvprintw(5, 4, "Book successfully deleted.");
        }
        else
        {
            mvprintw(5, 4, "ID not found.");
            remove("temp_books.csv");
        }

        mvprintw(7, 4, "Do you want to delete another book? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        choice = choice_line[0];

    } while (choice == 'y' || choice == 'Y');
    return 0;
}

void display_books()
{
    FILE *file = fopen("books.csv", "r");
    if (file == NULL)
    {
        clear();
        mvprintw(2, 2, "Could not open file!");
        getch();
        return;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    clear();
    box(stdscr, 0, 0);

    const char *header = "=== Book List ===";
    int start_col = (max_x - strlen(header)) / 2;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, start_col, "%s", header);
    attroff(COLOR_PAIR(1) | A_BOLD);

    int row = 4, col = 4, count = 0;
    char line[512];

    // Print column headers
    attron(A_BOLD | COLOR_PAIR(3));
    mvprintw(row++, col, "%-10s | %-30s | %-20s", "Book ID", "Title", "Author");
    attroff(A_BOLD | COLOR_PAIR(3));
    mvhline(row++, col, '-', 65);

    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {
        // Remove newline at end
        line[strcspn(line, "\n")] = '\0';

        // Parse the line safely
        char id[64] = "", title[256] = "", author[128] = "";
        char *first = strchr(line, ',');
        char *second = first ? strchr(first + 1, ',') : NULL;

        if (first && second)
        {
            size_t id_len = first - line;
            size_t title_len = second - first - 1;
            strncpy(id, line, id_len);
            id[id_len] = '\0';
            strncpy(title, first + 1, title_len);
            title[title_len] = '\0';
            strncpy(author, second + 1, sizeof(author) - 1);
            author[sizeof(author) - 1] = '\0';

            mvprintw(row++, col, "%-10s | %-30s | %-20s", id, title, author);
            count++;
        }

        // Pagination if too many rows
        if (row >= max_y - 3)
        {
            mvprintw(row, col, "-- Press any key to continue --");
            getch();
            clear();
            box(stdscr, 0, 0);
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(1, start_col, "%s", header);
            attroff(COLOR_PAIR(1) | A_BOLD);
            row = 4;
            attron(A_BOLD | COLOR_PAIR(3));
            mvprintw(row++, col, "%-10s | %-30s | %-20s", "Book ID", "Title", "Author");
            attroff(A_BOLD | COLOR_PAIR(3));
            mvhline(row++, col, '-', 65);
        }
    }

    if (count == 0)
    {
        mvprintw(row + 1, col, "No books found.");
    }
    else
    {
        mvprintw(row + 2, col, "Total books: %d", count);
    }

    mvprintw(row + 4, col, "Press any key to return...");
    getch();
    fclose(file);
}

// Main Book Portal Menu
void manage_books()
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
        const char *header = "=== Book Portal ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 30) / 2;

        for (int i = 0; i < BOOK_MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", book_menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", book_menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }

        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + BOOK_MENU_OPTIONS * 2 + 2;
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + BOOK_MENU_OPTIONS) % BOOK_MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % BOOK_MENU_OPTIONS;
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
                book_addition();
                break;
            case 1:
                book_search();
                break;
            case 2:
                book_delete();
                break;
            case 3:
                display_books();
                break;
            case 4:
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