#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define MENU_OPTIONS 3

const char *menu_items[MENU_OPTIONS] = {
    "Register",
    "Login",
    "Exit"};

void get_password_ncurses(char *password, int max_len, int y, int x)
{
    int i = 0;
    int ch;
    move(y, x);
    noecho();

    while ((ch = getch()) != '\n' && i < max_len - 1)
    {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            if (i > 0)
            {
                i--;
                password[i] = '\0';     // Clear the character in the buffer
                mvaddch(y, x + i, ' '); // Clear '*' from screen
                move(y, x + i);
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            password[i] = ch;
            mvaddch(y, x + i, '*');
            i++;
        }
    }

    password[i] = '\0';
    echo();
}

int get_username_ncurses(char *username, int max_len, int y, int x)
{
    int i = 0;
    int ch;

    move(y, x);
    noecho();

    while ((ch = getch()) != '\n' && i < max_len - 1)
    {
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            if (i > 0)
            {
                i--;
                username[i] = '\0';
                mvaddch(y, x + i, ' ');
                move(y, x + i);
            }
        }
        else if (ch == 27) // ESC key pressed
        {
            int menu_start_y = 5;
            int note_row = menu_start_y + 2 + 2;
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
            return ch; // ESC pressed — cancel input
        }
        else if (ch == '\n' || ch == '\r')
            break;                      // Enter pressed without input (optional guard)
        else if (ch >= 32 && ch <= 126) // Printable characters
        {
            username[i] = ch;
            mvaddch(y, x + i, ch);
            i++;
        }
    }

    username[i] = '\0';
    echo();
}

void register_user_ncurses()
{
    char username[50], password[50];

    clear();
    attron(COLOR_PAIR(1));
    wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
    attroff(COLOR_PAIR(1));
    const char *register_msg = "=== Register User System ===";
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int start_col = (max_x - strlen(register_msg)) / 2;
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, start_col, "%s", register_msg);
    attroff(COLOR_PAIR(1) | A_BOLD);
    mvprintw(3, 2, "Enter new username (ESC to cancel): ");
    int ch = get_username_ncurses(username, sizeof(username), 3, 40);

    if (ch == 27)
    {
        int menu_start_y = 5;
        int note_row = menu_start_y + 2 + 2;
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
        return; // ESC pressed — cancel registration
    }
    else
    {
        mvprintw(4, 2, "Enter new password: ");
        get_password_ncurses(password, sizeof(password), 4, 24);
    }

    FILE *fptr = fopen("loginfile.csv", "a+");
    if (fptr != NULL)
    {
        char file_un[50], file_pass[50];
        while (fscanf(fptr, "%[^,],%[^\n]\n", file_un, file_pass) != EOF)
        {
            if (strcmp(file_un, username) == 0)
            {
                mvprintw(6, 2, "Username already exists. Try a different one.");
                fclose(fptr);
                getch();
                return;
            }
        }
        fclose(fptr);
    }

    fptr = fopen("loginfile.csv", "a");
    if (fptr == NULL)
    {
        mvprintw(6, 2, "Error opening file.");
        getch();
        return;
    }

    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    if (size == 0)
    {
        fprintf(fptr, "UserName,Password\n");
    }
    fprintf(fptr, "%s,%s\n", username, password);
    fclose(fptr);
    mvprintw(6, 2, "User registered successfully!");
    getch();
}

int login_user_ncurses()
{
    char input_un[50], input_pass[50];
    char file_un[50], file_pass[50];
    int attempts = 0;

    while (attempts < 3)
    {
        clear();
        attron(COLOR_PAIR(1));
        wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
        attroff(COLOR_PAIR(1));
        const char *login_msg = "=== WELCOME to Login System ===\n";
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        int start_col = (max_x - strlen(login_msg)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", login_msg);
        attroff(COLOR_PAIR(1) | A_BOLD);
        mvprintw(3, 2, "Enter new username (ESC to cancel): ");
        int ch = getch();
        if (ch == 27)
        {
            int menu_start_y = 4;
            int note_row = menu_start_y + MENU_OPTIONS * 2 + 2;
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
        else if (ch == '\n')
            return 0; // Enter pressed without input (optional guard)
        else
        {
            echo();
            move(3, 38); // Move to the position where input should start
            getnstr(input_un, sizeof(input_un) - 1);
        }
        // getstr(input_un);
        mvprintw(4, 2, "Password: ");
        get_password_ncurses(input_pass, sizeof(input_pass), 4, 12);

        FILE *fptr = fopen("loginfile.csv", "r");
        if (fptr == NULL)
        {
            mvprintw(6, 2, "Error: Could not open loginfile.csv");
            getch();
            return 0;
        }

        int login = 0;
        while (fscanf(fptr, "%49[^,],%49[^\n]\n", file_un, file_pass) == 2)
        {
            if (strcmp(input_un, file_un) == 0 && strcmp(input_pass, file_pass) == 0)
            {
                login = 1;
                break;
            }
        }

        fclose(fptr);

        if (login)
        {
            mvprintw(6, 2, "Access granted. Welcome, %s!", input_un);
            refresh();
            mvprintw(7, 2, "Loading");
            refresh();
            for (int i = 0; i < 3; ++i)
            {
                napms(300); // Delay for 300ms
                printw(".");
                refresh();
            }
            napms(300); // Final pause
            return 1;   // Successful login
        }
        else
        {
            mvprintw(6, 2, "Access denied. Invalid credentials.");
            getch();
            attempts++;
        }
    }
    mvprintw(8, 2, "Too many failed attempts. Returning to main menu.");
    getch();
    return 0;
}

int admin()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_WHITE);  // Welcome message
    init_pair(2, COLOR_WHITE, COLOR_BLUE);  // Highlighted menu
    init_pair(3, COLOR_BLACK, COLOR_WHITE); // Normal menu

    bkgd(COLOR_PAIR(1));
    int highlight = 0;
    int choice = -1;
    int ch;
    while (1)
    {
        clear();
        attron(COLOR_PAIR(1));
        wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
        attroff(COLOR_PAIR(1));
        const char *welcome_msg = "=== WELCOME to Admin Portal ===";
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        int start_col = (max_x - strlen(welcome_msg)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", welcome_msg);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 20) / 2;

        for (int i = 0; i < MENU_OPTIONS; i++)
        {
            if (i == highlight)
            {
                attron(COLOR_PAIR(2));
                mvprintw(menu_start_y + i * 2, menu_start_x, " > %s", menu_items[i]);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                attron(COLOR_PAIR(3));
                mvprintw(menu_start_y + i * 2, menu_start_x, "   %s", menu_items[i]);
                attroff(COLOR_PAIR(3));
            }
        }
        const char *note_msg = "Use UP/DOWN arrows and ENTER to select.";
        int note_row = menu_start_y + MENU_OPTIONS * 2 + 2; // Place below the menu
        int note_start_col = (max_x - strlen(note_msg)) / 2;
        attron(A_BOLD);
        mvprintw(note_row, note_start_col, "%s", note_msg);
        attroff(A_BOLD);

        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            highlight = (highlight - 1 + MENU_OPTIONS) % MENU_OPTIONS;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % MENU_OPTIONS;
            break;
        case 10: // Enter key
            choice = highlight;
            break;
        case 27:                       // ESC key
            choice = MENU_OPTIONS - 1; // Equivalent to Exit
            break;
        default:
            break;
        }

        if (choice != -1)
        {
            switch (choice)
            {
            case 0:
                register_user_ncurses();
                break;
            case 1:
                if (login_user_ncurses() == 1)
                {
                    endwin();
                    return 0;
                }
                break;
            case 2:
                mvprintw(note_row + 1, 2, "Exiting");
                refresh();
                for (int i = 0; i < 3; ++i)
                {
                    napms(300); // Delay for 300ms
                    printw(".");
                    refresh();
                }
                napms(300); // Final pause
                return 1;
                endwin();
                return 1;
            }
            choice = -1; // Reset choice after action
        }
    }

    endwin();
    return 1;
}