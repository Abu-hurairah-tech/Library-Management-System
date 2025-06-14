#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "admin.h"

void get_password(char *password, int max_len)
{
    int i = 0;
    char ch;

    while (i < max_len - 1)
    {
        ch = getch(); // Read character without echo

        if (ch == '\r')
        { // Enter key pressed
            break;
        }
        else if (ch == '\b')
        { // Backspace key
            if (i > 0)
            {
                i--;
                printf("\b \b"); // Erase last '*'
            }
        }
        else if (ch >= 32 && ch <= 126)
        { // Printable characters
            password[i++] = ch;
            printf("*"); // Show mask
        }
    }
    password[i] = '\0';
    printf("\n");
}

void register_user()
{
    char username[12], password[12];

    printf("\n\n\t\t\t=== Register ===\n\n");
    printf("Enter new username: ");
    scanf("%s", username);

    printf("Enter new password: ");
    get_password(password, sizeof(password));

    // Check for existing username
    FILE *fptr = fopen("loginfile.csv", "a+");
    if (fptr != NULL)
    {
        char file_un[12], file_pass[12];
        while (fscanf(fptr, "%[^,],%[^\n]\n", file_un, file_pass) != EOF)
        {
            if (strcmp(file_un, username) == 0)
            {
                printf("Username already exists. Try a different one.\n");
                fclose(fptr);
                return;
            }
        }
        fclose(fptr);
    }

    // Append new user
    fptr = fopen("loginfile.csv", "a");
    if (fptr == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    // Add header if file is empty
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    if (size == 0)
    {
        fprintf(fptr, "UserName,Password\n");
    }

    fprintf(fptr, "%s,%s\n", username, password);
    fclose(fptr);
    printf("User registered successfully!\n");
}

//------------------------------------------------------------------------------------
int login_user() {
    char input_un[50], input_pass[50];
    char file_un[50], file_pass[50];
    int login = 0;
    int attempts = 0;

    while (attempts < 3) {
        printf("\n\n\t\t\t=== Login (Attempt %d of 3) ===\n\n", attempts + 1);
        printf("Enter username: ");
        scanf("%s", input_un);

        printf("Enter password: ");
        get_password(input_pass, sizeof(input_pass));

        FILE *fptr = fopen("loginfile.csv", "r");
        if (fptr == NULL) {
            printf("Error: Could not open loginfile.csv\n");
            return 0;
        }

        login = 0;

        while (fscanf(fptr, "%49[^,],%49[^\n]\n", file_un, file_pass) == 2) {
            if (strcmp(input_un, file_un) == 0 && strcmp(input_pass, file_pass) == 0) {
                login = 1;
                break;
            }
        }

        fclose(fptr);

        if (login) {
            printf("Access granted. Welcome, %s!\n", input_un);
            return 1;
        } else {
            printf("Access denied. Invalid username or password.\n");
            attempts++;
        }
    }

    printf("Too many failed attempts. Returning to main menu.\n");
    return 0;
}

//-------------------------------------------------------------------------------------------

int admin()
{
    int login_check;
    int option;

    printf("\n\n\t\t\t=== Welcome to the Login System ===\n\n");
    do
    {
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        while (1)
        {
            printf("Choose an option (1-3): ");
            if (scanf("%d", &option) != 1)
            {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n')
                    ; // clear buffer
                continue;
            }

            if (option < 1 || option > 3)
            {
                printf("Enter a valid option.\n");
                continue;
            }

            break; // valid input
        }

        switch (option)
        {
        case 1:
            register_user();
            break;

        case 2:
            login_check = login_user();
            if (login_check == 1)
            {
                return 0; // success
            }
            else
            {
                printf("Login failed.\n");
            }
            break;

        case 3:
            printf("Exiting admin panel...\n\n");
            return 1; // treat as failure to access library

        default:
            printf("Invalid choice. Try again.\n");
        }

    } while (1); // keep looping until login success or exit
}
