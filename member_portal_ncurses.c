#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "members.h"
#include <stdlib.h>
#define MEMBER_MENU_OPTIONS 4

static const char *member_menu_items[MEMBER_MENU_OPTIONS] = {
    "Add Member",
    "Search Member",
    "Delete Membership",
    "Exit"};

// Removes newline character from string (usually from fgets input)
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

// Function to add new member details
int get_field_input(int y, int x, char *buffer, int max_len)
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
            int note_row = menu_start_y + MEMBER_MENU_OPTIONS * 2 + 2;
            mvprintw(note_row + 1, 2, "Exiting");
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
            return 0; // ESC pressed — cancel input
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
            break; // Finish input on Enter
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

int prompt_and_get_input(int row, int col, const char *label, char *buffer, int max_len)
{
    mvprintw(row, col, "%s", label);
    return get_field_input(row, col + strlen(label), buffer, max_len);
}

int member_addition()
{
    FILE *member;
    char name[200], id[200], contact[200], dep[200], session[200], choice_line[10], data[200];
    char choice;
    int menu_start_y = 4;
    int note_row = menu_start_y + MEMBER_MENU_OPTIONS * 2 + 2;

    member = fopen("member.csv", "a+");
    if (member == NULL)
    {
        clear();
        mvprintw(2, 2, "Unable to open file");
        getch();
        return 1;
    }

    // Check if file is empty, if so write header row
    fseek(member, 0, SEEK_END);
    long size = ftell(member);
    if (size == 0)
    {
        fprintf(member, "Name,ID,Department,Session,Contact\n");
    }

    do
    {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        while (1)
        {
            clear();
            box(stdscr, 0, 0);
            const char *header = "=== Add New Member ===";
            int start_col = (max_x - strlen(header)) / 2;
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(1, start_col, "%s", header);
            attroff(COLOR_PAIR(1) | A_BOLD);

            // Name input
            mvprintw(3, 4, "1. Enter Member Name: ");
            if (!get_field_input(3, 30, name, sizeof(name)))
            {
                return 0;
            }

            // ID input (with uniqueness check)
            int is_unique;
            do
            {
                is_unique = 0;
                mvprintw(4, 4, "2. Member ID: ");
                if (!get_field_input(4, 30, id, sizeof(id)))
                {
                    return 0;
                }

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
                    move(4, 30);
                    clrtoeol();
                }
            } while (is_unique);

            // Department
            mvprintw(5, 4, "3. Department: ");
            if (!get_field_input(5, 30, dep, sizeof(dep)))
            {
                return 0;
            }

            // Session
            mvprintw(6, 4, "4. Session: ");
            if (!get_field_input(6, 30, session, sizeof(session)))
            {
                return 0;
            }

            // Contact
            mvprintw(7, 4, "5. Contact: ");
            if (!get_field_input(7, 30, contact, sizeof(contact)))
            {
                return 0;
            }

            if (strlen(name) == 0 || strlen(id) == 0 || strlen(dep) == 0 ||
                strlen(session) == 0 || strlen(contact) == 0)
            {
                mvprintw(9, 4, "All fields must be filled!");
            }
            else
            {
                break; // all fields are filled
            }
        }

        // Writing member data
        fprintf(member, "%s,%s,%s,%s,%s\n", name, id, dep, session, contact);

        mvprintw(11, 4, "Do you want to enter another Member? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');

    fclose(member);
    mvprintw(13, 4, "Member(s) added successfully!");
    return 0;
}

// Function to search a member using ID

int search()
{
    FILE *member;
    member = fopen("member.csv", "r");

    if (member == NULL)
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
        const char *header = "=== Search Member ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        echo();
        if (!prompt_and_get_input(3, 4, "Enter the ID of member: ", id, sizeof(id)))
        {
            return 0;
        }
        found = 0;

        // Skip header line
        rewind(member);
        fgets(data, sizeof(data), member);

        // Clear previous result area
        for (int i = 5; i <= 11; ++i)
        {
            move(i, 0);
            clrtoeol();
        }

        while (fgets(data, sizeof(data), member))
        {
            remove_newline(data);

            char *tmp = strdup(data);
            char *name = strtok(tmp, ",");
            char *user_id = strtok(NULL, ",");
            char *dep = strtok(NULL, ",");
            char *session = strtok(NULL, ",");
            char *contact = strtok(NULL, ",");

            if (user_id && strcmp(user_id, id) == 0)
            {
                mvprintw(5, 4, "Member found:");
                mvprintw(6, 6, "Name: %s", name);
                mvprintw(7, 6, "ID: %s", user_id);
                mvprintw(8, 6, "Department: %s", dep);
                mvprintw(9, 6, "Session: %s", session);
                mvprintw(10, 6, "Contact: %s", contact);
                found = 1;
                free(tmp);
                break;
            }
            free(tmp);
        }

        if (!found)
        {
            mvprintw(5, 4, "Member with ID %s not found.", id);
        }

        mvprintw(13, 4, "Do you want to find another member? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        noecho();
        choice = choice_line[0];

    } while (choice == 'y' || choice == 'Y');

    fclose(member);
    return 0;
}
// Function to delete a member by ID
int delete()
{
    char id[200], data[200], choice, choice_line[10];

    do
    {
        int found = 0;
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        FILE *Member = fopen("member.csv", "r");
        if (Member == NULL)
        {
            clear();
            mvprintw(2, 2, "Unable to open file");
            getch();
            return 1;
        }

        FILE *temp = fopen("temp.csv", "w");
        if (temp == NULL)
        {
            clear();
            mvprintw(3, 2, "Unable to create temp file");
            fclose(Member);
            getch();
            return 1;
        }

        clear();
        box(stdscr, 0, 0);
        const char *header = "=== Delete Membership ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        echo();
        if (!prompt_and_get_input(3, 4, "Enter the ID of member: ", id, sizeof(id)))
        {
            return 0;
        }

        // Copy lines that don't contain the ID to temp file
        while (fgets(data, sizeof(data), Member))
        {
            // Make a copy for strtok
            char line_copy[200];
            strncpy(line_copy, data, sizeof(line_copy));
            line_copy[sizeof(line_copy) - 1] = '\0';

            char *name = strtok(line_copy, ",");
            char *user_id = strtok(NULL, ",");
            char *dep = strtok(NULL, ",");
            char *session = strtok(NULL, ",");
            char *contact = strtok(NULL, ",");

            if (user_id && strcmp(user_id, id) != 0)
            {
                // Write the original line to temp file
                fputs(data, temp);
            }
            else if (user_id && strcmp(user_id, id) == 0)
            {
                found = 1; // If ID matched
            }
        }

        fclose(Member);
        fclose(temp);

        if (found)
        {
            // Replace original file with updated temp file
            remove("member.csv");
            rename("temp.csv", "member.csv");
            mvprintw(5, 4, "Data successfully deleted.");
        }
        else
        {
            // If ID not found, delete temp file and notify user
            mvprintw(5, 4, "ID not found.");
            remove("temp.csv");
        }

        mvprintw(7, 4, "Do you want to delete another membership? (y/n): ");
        echo();
        getnstr(choice_line, sizeof(choice_line) - 1);
        choice = choice_line[0];

    } while (choice == 'y' || choice == 'Y');
    return 0;
}

void manage_members()
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
        const char *header = "=== Membership Portal ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 30) / 2;

        for (int i = 0; i < MEMBER_MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", member_menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", member_menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }

        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + MEMBER_MENU_OPTIONS * 2 + 2;
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + MEMBER_MENU_OPTIONS) % MEMBER_MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % MEMBER_MENU_OPTIONS;
            break;
        case 10: // Enter
            choice = highlight;
            break;
        case 27: // ESC key
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
            return;
        default:
            break;
        }

        if (choice != -1)
        {
            endwin(); // End ncurses before calling stdio-based functions
            switch (choice)
            {
            case 0:
                member_addition();
                break;
            case 1:
                search();
                break;
            case 2:
                delete();
                break;
            case 3:
                mvprintw(note_row + 1, 4, "Exiting");
                refresh();
                for (int i = 0; i < 3; ++i)
                {
                    napms(300); // Delay for 300ms
                    printw(".");
                    refresh();
                }
                napms(300); // Final pause
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
    endwin(); // End ncurses mode
    return;
}