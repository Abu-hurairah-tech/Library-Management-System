#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "fine.h"

void remove_newline(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

void slip_generation()
{

    char member_id[200], line[200], choice_line[10], choice;
    
    do {
        int total_fine = 0;
    
        // Input Member ID
        printf("Enter Member ID: ");
        fgets(member_id, sizeof(member_id), stdin);
        remove_newline(member_id);
    
        FILE *fine = fopen("fine.csv", "r");
        if (!fine) {
            printf("Unable to open file!\n");
            return;
        }
    
        // Print header
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("| %-8s | %-10s | %-15s | %-15s | %-10s | %-6s |\n", 
               "Book ID", "Member ID", "Date Of Issue", "Date Of Return", "Days Late", "Fine");
        printf("----------------------------------------------------------------------------------------\n");
    
        int found = 0;
    
        while (fgets(line, sizeof(line), fine)) {
            // Copy original line for safety
            char line_copy[200];
            strcpy(line_copy, line);
    
            // Tokenize
            char *book_id = strtok(line, ",");
            char *user_id = strtok(NULL, ",");
            char *doi = strtok(NULL, ",");
            char *dor = strtok(NULL, ",");
            char *days_late = strtok(NULL, ",");
            char *fine_amount = strtok(NULL, ",");
    
            // Clean tokens
            if (user_id) remove_newline(user_id);
            if (fine_amount) remove_newline(fine_amount);
    
            // Match user
            if (user_id && strcmp(member_id, user_id) == 0) {
                printf("| %-8s | %-10s | %-15s | %-15s | %-10s | %-6s |\n",
                       book_id, user_id, doi, dor, days_late, fine_amount);
    
                if (fine_amount) {
                    total_fine += atoi(fine_amount);
                }
                found = 1;
            }
        }
    
        fclose(fine);
    
        if (found) {
            printf("----------------------------------------------------------------------------------------\n");
            printf("Total Fine for Member %s = %d\n", member_id, total_fine);
        } else {
            printf("No records found for Member ID: %s\n", member_id);
        }
    
        printf("\nDo you want to generate another (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    
    } while (choice == 'y' || choice == 'Y');
}

void clear_fine_records()
{

    char member_id[200], line[200], choice_line[10], choice;
    
    do
    {
        printf("Enter Member ID to clear fine: ");
        fgets(member_id, sizeof(member_id), stdin);
        remove_newline(member_id);
    
        FILE *fine = fopen("fine.csv", "r");
        FILE *temp = fopen("temp.csv", "w");
    
        if (!fine || !temp)
        {
            printf("Error opening file!\n");
            return;
        }
    
        int found = 0;
    
        while (fgets(line, sizeof(line), fine))
        {
            // Copy line before tokenizing
            char line_copy[200];
            strcpy(line_copy, line);
    
            // Get user ID from line
            char *book_id = strtok(line, ",");
            char *user_id = strtok(NULL, ",");
    
            if (user_id)
                remove_newline(user_id);
    
            // Keep only lines NOT matching the entered user ID
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
            printf("Fine cleared for Member ID: %s\n", member_id);
        }
        else
        {
            remove("temp.csv");
            printf("No fine records found for Member ID: %s\n", member_id);
        }
    
        printf("\nDo you want to clear another fine? (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    
    } while (choice == 'y' || choice == 'Y');
}

void fine()
{
    int choice;
    char cont;
    printf("\n\n\t\t\t=== Welcome to the Fine Portal! ===\n\n");
        printf("Please select an option:\n");
        printf("1. Fine Slip\n");
        printf("2. Fine Clearance\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline character from input buffer
        switch (choice)
        {
        case 1:
            printf("\n\n\t\t\t=== Fine Slip Generation ===\n\n");
            slip_generation();
            break;
        case 2:
            printf("\n\n\t\t\t=== Fine Clearance ===\n\n");
            clear_fine_records();
            break;
        case 3:
            printf("Exiting Fine Portal.\n\n");
            return;
            break;
        default:

            printf("Invalid choice. Please try again.\n");
            break;
        }
    printf("Thank you for using the Fine Portal!\n");
}