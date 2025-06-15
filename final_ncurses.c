#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <ncurses.h>
#include "books.h"
#include "members.h"
#include "admin.h"
#include "issue.h"
#include "return.h"
#include "fine.h"

#define MENU_OPTIONS 6

static const char *menu_items[MENU_OPTIONS] = {
    "Member Management System",
    "Books Management System",
    "Book Issue Management System",
    "Book Return Management System",
    "Fine Management System",
    "Exit"};

int library_menu()
{
    int highlight = 0;
    int choice = -1;
    int ch;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Color pairs: 1 = header, 2 = highlighted, 3 = normal
    init_pair(1, COLOR_BLUE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);

    bkgd(COLOR_PAIR(1));
    while (1)
    {
        clear();
        attron(COLOR_PAIR(1));
        wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
        attroff(COLOR_PAIR(1));
        const char *header = "=== WELCOME to Library Management System ===";
        int start_col = (max_x - strlen(header)) / 2;
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, start_col, "%s", header);
        attroff(COLOR_PAIR(1) | A_BOLD);

        int menu_start_y = 4;
        int menu_start_x = (max_x - 40) / 2;

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
        const char *note_msg = "Use UP/DOWN arrows and ENTER to select. Press ESC to exit.";
        int note_row = menu_start_y + MENU_OPTIONS * 2 + 2;
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
            return choice;
        }
    }
}

int main()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    int login_check = admin();

    if (login_check == 0)
    {
        int menu_choice;
        while (1)
        {
            menu_choice = library_menu();
            switch (menu_choice)
            {
            case 0:
                endwin();
                manage_members();
                initscr();
                noecho();
                cbreak();
                keypad(stdscr, TRUE);
                start_color();
                break;
            case 1:
                endwin();
                manage_books();
                initscr();
                noecho();
                cbreak();
                keypad(stdscr, TRUE);
                start_color();
                break;
            case 2:
                endwin();
                issue_books();
                initscr();
                noecho();
                cbreak();
                keypad(stdscr, TRUE);
                start_color();
                break;
            case 3:
                endwin();
                return_books();
                initscr();
                noecho();
                cbreak();
                keypad(stdscr, TRUE);
                start_color();
                break;
            case 4:
                endwin();
                fine();
                initscr();
                noecho();
                cbreak();
                keypad(stdscr, TRUE);
                start_color();
                break;
            case 5:
                endwin();
                return 0;
            }
        }
    }

    endwin();
    return 0;
}