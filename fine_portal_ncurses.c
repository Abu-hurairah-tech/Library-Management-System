#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FINE_MENU_OPTIONS 3

static const char *fine_menu_items[FINE_MENU_OPTIONS] = {
    "Fine Slip",
    "Fine Clearance",
    "Exit"
};

void remove_newline(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

int get_field_input_fine(int y, int x, char *buffer, int max_len)
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

int prompt_and_get_input_fine(int row, int col, const char *label, char *buffer, int max_len)
{
    mvprintw(row, col, "%s", label);
    return get_field_input_fine(row, col + strlen(label), buffer, max_len);
}

void slip_generation_ncurses()
{
    char member_id[200], line[300];
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    while (1)
    {
        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Fine Slip Generation ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        if (!prompt_and_get_input_fine(3, 4, "Enter Member ID: ", member_id, sizeof(member_id)))
            return;

        FILE *fine = fopen("fine.csv", "r");
        if (!fine) {
            mvprintw(5, 4, "Unable to open fine.csv!");
            getch();
            return;
        }

        int found = 0, total_fine = 0, row = 6;
        fgets(line, sizeof(line), fine); // skip header

        mvprintw(row++, 2, "----------------------------------------------------------------------------------------");
        mvprintw(row++, 2, "| %-8s | %-10s | %-15s | %-15s | %-10s | %-6s |",
                 "Book ID", "Member ID", "Date Of Issue", "Date Of Return", "Days Late", "Fine");
        mvprintw(row++, 2, "----------------------------------------------------------------------------------------");

        while (fgets(line, sizeof(line), fine)) {
            char line_copy[300];
            strcpy(line_copy, line);

            char *book_id = strtok(line, ",");
            char *user_id = strtok(NULL, ",");
            char *doi = strtok(NULL, ",");
            char *dor = strtok(NULL, ",");
            char *days_late = strtok(NULL, ",");
            char *fine_amount = strtok(NULL, ",\n");

            if (user_id) remove_newline(user_id);
            if (fine_amount) remove_newline(fine_amount);

            if (user_id && strcmp(member_id, user_id) == 0) {
                mvprintw(row++, 2, "| %-8s | %-10s | %-15s | %-15s | %-10s | %-6s |",
                         book_id ? book_id : "", user_id, doi ? doi : "", dor ? dor : "",
                         days_late ? days_late : "", fine_amount ? fine_amount : "");
                if (fine_amount) total_fine += atoi(fine_amount);
                found = 1;
            }
        }
        fclose(fine);

        if (found) {
            mvprintw(row++, 2, "----------------------------------------------------------------------------------------");
            mvprintw(row++, 2, "Total Fine for Member %s = %d", member_id, total_fine);
        } else {
            mvprintw(row++, 2, "No records found for Member ID: %s", member_id);
        }

        mvprintw(row + 2, 2, "Generate another slip? (y/n): ");
        char choice_line[10];
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        noecho();
        if (choice_line[0] != 'y' && choice_line[0] != 'Y')
            break;
    }
}

void clear_fine_records_ncurses()
{
    char member_id[200], line[300];
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    while (1)
    {
        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Fine Clearance ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        if (!prompt_and_get_input_fine(3, 4, "Enter Member ID to clear fine: ", member_id, sizeof(member_id)))
            return;

        FILE *fine = fopen("fine.csv", "r");
        FILE *temp = fopen("temp.csv", "w");
        if (!fine || !temp)
        {
            mvprintw(5, 4, "Error opening file!");
            if (fine) fclose(fine);
            if (temp) fclose(temp);
            getch();
            return;
        }

        int found = 0;
        fgets(line, sizeof(line), fine); // header
        fputs(line, temp);

        while (fgets(line, sizeof(line), fine))
        {
            char line_copy[300];
            strcpy(line_copy, line);

            char *book_id = strtok(line, ",");
            char *user_id = strtok(NULL, ",");
            if (user_id) remove_newline(user_id);

            if (user_id && strcmp(user_id, member_id) != 0)
            {
                fputs(line_copy, temp);
            }
            else
            {
                found = 1;
            }
        }

        fclose(fine);
        fclose(temp);

        if (found)
        {
            remove("fine.csv");
            rename("temp.csv", "fine.csv");
            mvprintw(6, 4, "Fine cleared for Member ID: %s", member_id);
        }
        else
        {
            remove("temp.csv");
            mvprintw(6, 4, "No fine records found for Member ID: %s", member_id);
        }

        mvprintw(8, 4, "Clear another fine? (y/n): ");
        char choice_line[10];
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        noecho();
        if (choice_line[0] != 'y' && choice_line[0] != 'Y')
            break;
    }
}

void fine()
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
        const char *header = "=== Fine Portal ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 30) / 2;

        for (int i = 0; i < FINE_MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", fine_menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", fine_menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }

        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + FINE_MENU_OPTIONS * 2 + 2;
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + FINE_MENU_OPTIONS) % FINE_MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % FINE_MENU_OPTIONS;
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
                slip_generation_ncurses();
                break;
            case 1:
                clear_fine_records_ncurses();
                break;
            case 2:
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