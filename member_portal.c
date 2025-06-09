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
    printf("Membership portal");
    printf("\nOptions : ");
    printf("\n1. Add Member\n2. Search member\n3. Delete membership\n4. Exit");
}

// Function to add new member details
int member_addition()
{
    FILE *member;
    char name[200], id[200], contact[200], dep[200], session[200], choice;

    // Open file in append mode to add new data
    member = fopen("member.csv", "a");

    if (member == NULL)
    {
        printf("Unable to open file");
        return 1;
    }

    // Check if file is empty, if so write header row
    fseek(member, 0, SEEK_END);
    long size = ftell(member);
    if (size == 0)
    {
        fprintf(member, "Sr,Name,ID,Department,Session,Contact\n");
    }

    printf("Provide following details to get membership\n");

    do
    {
        // Taking member details
        printf("1. Name : ");
        fgets(name, sizeof(name), stdin);
        remove_newline(name);

        printf("2. User ID : ");
        fgets(id, sizeof(id), stdin);
        remove_newline(id);

        printf("3. Department : ");
        fgets(dep, sizeof(dep), stdin);
        remove_newline(dep);

        printf("4. Session : ");
        fgets(session, sizeof(session), stdin);
        remove_newline(session);

        printf("5. Contact : ");
        fgets(contact, sizeof(contact), stdin);
        remove_newline(contact);

        // Writing member data
        fprintf(member, "%s,%s,%s,%s,%s\n", name, id, dep, session, contact);

        // Ask if user wants to enter another member
        printf("Do you want to enter user again?(yes/no) : ");
        scanf(" %c", &choice);

    } while (choice == 'y' || choice == 'Y');

    fclose(member); // Close the file
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

    char id[200], data[300], choice;
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
        scanf(" %c", &choice);
        rewind(member); // Reset file pointer to start

    } while (choice == 'y' || choice == 'Y');

    fclose(member); // Close the file
    return 0;
}

// Function to delete a member by ID
int delete()
{
    char id[200], data[200], choice;

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
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');
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

        // Call respective functions based on user choice
        switch (option)
        {
        case 1:
            member_addition();
            break;
        case 2:
            search();
            break;
        case 3:
            delete();
            break;
        case 4:
            printf("Exiting...");
            return; // Exit program
        default:
            printf("Invalid choice. Please choose a valid option.\n");
        }

    } while (option != 4);
}
