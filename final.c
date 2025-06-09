#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "books.h"
#include "members.h"
#include "admin.h"
#include "issue.h"
#include "return.h"
// #include <graphics.h>

// #define  loginfile.csv "users.txt"

void displaymenu()
{
    printf("\nWhat do you want to use:\n");
    printf("A. Member Management System\n");
    printf("B. Books Management System\n");
    printf("C. Book Issue Management System\n");
    printf("D. Book Return Management System\n");
    printf("E. Exit\n");
}

int main()
{
    int login_check = admin(); // get login result from admin()
    
    if (login_check == 0)
    {
        char portalChoice;
        
        printf("Welcome to Library Management System\n");
        
        do
        {
            displaymenu();
            printf("Enter your choice: ");
            scanf(" %c", &portalChoice); // space before %c to eat leftover newline

            switch (portalChoice)
            {
            case 'A':
            case 'a':
                manage_members();
                break;

            case 'B':
            case 'b':
                manage_books();
                break;

            case 'C':
            case 'c':
                issue_books();
                break;

            case 'D':
            case 'd':
                return_books();
                break;

            case 'E':
            case 'e':
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid input. Please try again.\n");
            }

        } while (portalChoice != 'E' && portalChoice != 'e');
    }
    else
    {
        printf("Login failed. Exiting the program.\n");
    }

    return 0;
}
