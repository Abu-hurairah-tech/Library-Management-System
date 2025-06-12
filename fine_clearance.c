#include <stdio.h>
#include <string.h>

void remove_newline(char *str)
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

void clear_fine_records()
{

    char member_id[200], line[200], choice_line[10], choice;
    
    do
    {
        printf("Enter User ID to clear fine: ");
        fgets(member_id, sizeof(member_id), stdin);
        remove_newline(member_id);
    
        FILE *fine = fopen("fine.csv", "r");
        FILE *temp = fopen("temp.csv", "w");
    
        if (!fine || !temp)
        {
            printf("Error opening file!\n");
            return 1;
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
            printf("Fine cleared for User ID: %s\n", member_id);
        }
        else
        {
            remove("temp.csv");
            printf("No fine records found for User ID: %s\n", member_id);
        }
    
        printf("\nDo you want to clear another fine? (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    
    } while (choice == 'y' || choice == 'Y');
}

void fine_clearance()
{
    clear_fine_records();
}
