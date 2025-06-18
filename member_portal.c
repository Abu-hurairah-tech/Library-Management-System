#include <stdio.h>
#include <string.h>
#include "members.h"

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

void displaymembermenu()
{
    printf("\n\n\t\t\t=== Membership portal ===\n\n");
    printf("\nOptions : ");
    printf("\n1. Add Member\n2. Search member\n3. Delete membership\n4. Display All Members\n5. Exit\n");
}

// Function to add new member details
int member_addition()
{
    FILE *member;
    char name[200], id[200], contact[200], dep[200], session[200], choice_line[10], data[200];
    char choice;

    // Open file in append mode to add new data
    member = fopen("member.csv", "a+");
    if (member == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }

    char *user_id;
    // Check if file is empty, if so write header row
    fseek(member, 0, SEEK_END);
    long size = ftell(member);
    if (size == 0)
    {
        fprintf(member, "Name,ID,Department,Session,Contact\n");
    }

    printf("Provide the following details to get membership\n");

    do
    {
        // Taking member details
        while (1)
        {
            printf("1. Name : ");
            fgets(name, sizeof(name), stdin);
            remove_newline(name);

            int is_unique;

            do
            {
                is_unique = 0;
                printf("2. User ID : ");
                fgets(id, sizeof(id), stdin);
                remove_newline(id);
                rewind(member);
                while (fgets(data, sizeof(data), member))
                {
                    char *name = strtok(data, ",");
                    char *user_id = strtok(NULL, ",");

                    if (id && strcmp(id, user_id) == 0)
                    {
                        printf("User ID already exists\n");
                        is_unique = 1;
                        break;
                    }
                }
            } while (is_unique);

            printf("3. Department : ");
            fgets(dep, sizeof(dep), stdin);
            remove_newline(dep);

            printf("4. Session : ");
            fgets(session, sizeof(session), stdin);
            remove_newline(session);

            printf("5. Contact : ");
            fgets(contact, sizeof(contact), stdin);
            remove_newline(contact);

            if (strlen(name) == 0 || strlen(id) == 0 || strlen(dep) == 0 ||
                strlen(session) == 0 || strlen(contact) == 0)
            {
                printf("\nAll fields must be filled! Please try again.\n\n");
            }
            else
            {
                break; // all fields are filled
            }
        }

        // Writing member data
        fprintf(member, "%s,%s,%s,%s,%s\n", name, id, dep, session, contact);

        // Ask if user wants to enter another member
        printf("Do you want to enter user again? (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');

    fclose(member); // Close the file
    return 0;
}

// Function to search a member using ID

int search()
{
    FILE *member;
    member = fopen("member.csv", "r");

    if (member == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }

    char id[200], data[300], choice, choice_line[10];
    int found;

    do
    {
        printf("Enter ID of member: ");
        fgets(id, sizeof(id), stdin);
        remove_newline(id);

        found = 0;

        // Skip header line
        fgets(data, sizeof(data), member);

        while (fgets(data, sizeof(data), member))
        {
            remove_newline(data);

            // Use strtok to parse the CSV fields

            char *name = strtok(data, ",");
            char *user_id = strtok(NULL, ",");
            char *dep = strtok(NULL, ",");
            char *session = strtok(NULL, ",");
            char *contact = strtok(NULL, ",");

            if (user_id && strcmp(user_id, id) == 0)
            {
                printf("Member found:\n");
                printf("Name: %s\n", name);
                printf("ID: %s\n", user_id);
                printf("Department: %s\n", dep);
                printf("Session: %s\n", session);
                printf("Contact: %s\n", contact);
                found = 1;
                break;
            }
        }

        if (!found)
        {
            printf("Member with ID %s not found.\n", id);
        }

        printf("Do you want to find another member (yes/no) : ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
        rewind(member); // Reset file pointer to start

    } while (choice == 'y' || choice == 'Y');

    fclose(member); // Close the file
    return 0;
}

// Function to delete a member by ID
int delete()
{
    char id[200], data[200], choice, choice_line[10];

    do
    {
        int found = 0;

        FILE *Member = fopen("member.csv", "r");
        if (Member == NULL)
        {
            printf("Unable to open file");
            return 1;
        }

        FILE *temp = fopen("temp.csv", "w");

        printf("Enter the ID of member : ");
        fgets(id, sizeof(id), stdin);
        remove_newline(id);

        // Copy lines that don't contain the ID to temp file
        while (fgets(data, sizeof(data), Member))
        {
            char *name = strtok(data, ",");
            char *user_id = strtok(NULL, ",");
            char *dep = strtok(NULL, ",");
            char *session = strtok(NULL, ",");
            char *contact = strtok(NULL, ",");

            if (user_id && strcmp(user_id, id) != 0)
            {
                fprintf(temp, "%s,%s,%s,%s,%s", name, user_id, dep, session, contact);
            }
            else
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
            printf("Data successfully deleted\n");
        }
        else
        {
            // If ID not found, delete temp file and notify user
            printf("ID not found\n");
            remove("temp.csv");
        }

        // Ask if user wants to delete another entry
        printf("Do you want to delete membership again (yes/no) : ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');
}

void display_members()
{
    FILE *member = fopen("member.csv", "r");
    char line[300];
    int isEmpty = 1;

    if (member == NULL)
    {
        printf("Unable to open file\n");
        return;
    }

    printf("\n\n\t\t\t=== Member List ===\n\n");

    // Print header
    if (fgets(line, sizeof(line), member))
    {
        printf("%-20s %-10s %-15s %-10s %-15s\n", "Name", "ID", "Department", "Session", "Contact");
        printf("--------------------------------------------------------------------------------\n");
        isEmpty = 0;
    }

    while (fgets(line, sizeof(line), member))
    {
        char *name = strtok(line, ",");
        char *id = strtok(NULL, ",");
        char *dep = strtok(NULL, ",");
        char *session = strtok(NULL, ",");
        char *contact = strtok(NULL, ",\n");

        if (name && id && dep && session && contact)
        {
            printf("%-20s %-10s %-15s %-10s %-15s\n", name, id, dep, session, contact);
            isEmpty = 0;
        }
    }

    if (isEmpty)
    {
        printf("No member records found.\n");
    }

    fclose(member);
}

void manage_members()
{
    int option;

    do
    {

        // Menu display
        displaymembermenu();
        printf("Select an option: ");
        scanf("%d", &option);
        while (getchar() != '\n')
            ; // Clear input buffer

        if (option < 1 || option > 5)
        {
            printf("Invalid option. Please choose a valid option.\n");
        }

        else
        { // Call respective functions based on user choice
            switch (option)
            {
            case 1:
                printf("\n\n\t\t\t=== Adding Member ===\n\n");
                member_addition();
                break;
            case 2:
                printf("\n\n\t\t\t=== Search Member ===\n\n");
                search();
                break;
            case 3:
                printf("\n\n\t\t\t=== Delete Member ===\n\n");
                delete();
                break;
            case 4:
                printf("\n\n\t\t\t=== Display Members ===\n\n");
                display_members();
                break;
            case 5:
                printf("Exiting...\n\n");
                return; // Exit program
            default:
                printf("Invalid choice. Please choose a valid option.\n");
            }
        }
    } while (option != 5);
}
