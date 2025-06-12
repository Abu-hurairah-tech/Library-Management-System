#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
    
        // Input user ID
        printf("Enter User ID: ");
        fgets(member_id, sizeof(member_id), stdin);
        remove_newline(member_id);
    
        FILE *fine = fopen("fine.csv", "r");
        if (!fine) {
            printf("Unable to open file!\n");
            return 1;
        }
    
        // Print header
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("| %-8s | %-10s | %-15s | %-15s | %-10s | %-6s |\n", 
               "Book ID", "User ID", "Date Of Issue", "Date Of Return", "Days Late", "Fine");
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
            printf("Total Fine for user %s = %d\n", member_id, total_fine);
        } else {
            printf("No records found for User ID: %s\n", member_id);
        }
    
        printf("\nDo you want to generate another (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    
    } while (choice == 'y' || choice == 'Y');
}

void fine_slip() {
    printf("Welcome to Fine Slip Generation!\n");
    printf("This will generate a fine slip for the user.\n");
    
    // Call the slip generation function
    slip_generation();
}
