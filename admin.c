#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "admin.h"

void register_user()
{
    char username[12], password[12];

    printf("=== Register ===\n");
    printf("Enter new username: ");
    scanf("%s", username);
    printf("Enter new password: ");
    scanf("%s", password);

    FILE *fptr = fopen("loginfile.csv", "r");
    char file_un[12], file_pass[12];
    while (fscanf(fptr, "%s %s", file_un, file_pass) != EOF)
    {
        if (strcmp(file_un, username) == 0)
        {
            printf("Username already exists. Try a different one.\n");
            fclose(fptr);
            return;
        }
    }
    fclose(fptr);
    fptr = fopen("loginfile.csv", "a");
    if (fptr == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fptr, "%s %s\n", username, password);
    fclose(fptr);
    printf("User registered successfully!\n");
}

//------------------------------------------------------------------------------------

int login_user()
{
    char input_un[12], input_pass[12];
    char file_un[12], file_pass[12];
    int login = 0;
    int attempts = 0;

    while (attempts < 3)
    {
        printf("=== Login (Attempt %d of 3) ===\n", attempts + 1);
        printf("Enter username: ");
        scanf("%s", input_un);
        printf("Enter password: ");
        scanf("%s", input_pass);

        FILE *fptr = fopen("loginfile.csv", "r");
        if (fptr == NULL)
        {
            printf("Error: Could not open users file.\n");
            return 0;
        }

        login = 0; // reset for each attempt
        while (fscanf(fptr, "%s %s", file_un, file_pass) != EOF)
        {
            if (strcmp(input_un, file_un) == 0 &&
                strcmp(input_pass, file_pass) == 0)
            {
                login = 1;
                break;
            }
        }

        fclose(fptr);

        if (login)
        {
            printf("Access granted. Welcome, %s!\n", input_un);
            return 1;
        }
        else
        {
            printf("Access denied. Invalid username or password.\n");
            attempts++;
        }
    }

    // If all 3 attempts fail
    printf("Too many failed attempts. Returning to main menu.\n");
    return 0;
}


//-------------------------------------------------------------------------------------------

int admin()
{
    char choice;
    int login_check;
    
    do
    {
        printf("=== Welcome to the Login System ===\n");
        printf("A. Register\n");
        printf("B. Login\n");
        printf("E. Exit\n");
        printf("Choose an option: ");
        scanf(" %c", &choice);
        // Note the space before %c to skip whitespace

        switch (choice)
        {
            case 'A':
            case 'a':
                register_user();
                break;

            case 'B':
            case 'b':
                login_check = login_user();
                break;

            case 'E':
            case 'e':
                printf("Exiting...");
                break;

            default:
                printf("Invalid Choice. Try again\n");
        }

    } while (choice != 'A' && choice != 'a' && choice != 'B' && choice != 'b' && choice != 'e' && choice != 'E');
    
    return 0;
}
