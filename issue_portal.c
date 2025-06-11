#include <stdio.h>
#include <string.h>
#include "issue.h"

typedef struct
{
    int day, month, year;
} Date;

void format_date(Date d, char *buffer, size_t size)
{
    sprintf(buffer, "%02d-%02d-%04d", d.day, d.month, d.year);
}

// Removes newline character from input strings
void remove_newLine(char *str)
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

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Registers new members and writes their info to member.csv
int application()
{
    FILE *member;
    char name[200], id[200], contact[200], dep[200], session[200];
    char choice;

    member = fopen("member.csv", "a"); // open for appending
    if (member == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }

    // Write header if file is empty
    fseek(member, 0, SEEK_END);
    long size = ftell(member);
    if (size == 0)
    {
        fprintf(member, "Name,ID,Department,Session,Contact\n");
    }

    printf("Provide the following details to get membership\n");

    do
    {
        while (1)
        {
            printf("1. Name: ");
            fgets(name, sizeof(name), stdin);
            remove_newLine(name);

            printf("2. User ID: ");
            fgets(id, sizeof(id), stdin);
            remove_newLine(id);

            printf("3. Department: ");
            fgets(dep, sizeof(dep), stdin);
            remove_newLine(dep);

            printf("4. Session: ");
            fgets(session, sizeof(session), stdin);
            remove_newLine(session);

            printf("5. Contact: ");
            fgets(contact, sizeof(contact), stdin);
            remove_newLine(contact);

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

        fprintf(member, "%s,%s,%s,%s,%s\n", name, id, dep, session, contact);
        printf("Membership added successfully!\n");

        printf("Do you want to enter another user? (y/n): ");
        scanf(" %c", &choice);
        while (getchar() != '\n')
            ; // clear input buffer

    } while (choice == 'y' || choice == 'Y');

    fclose(member);
    return 0;
}

// Verifies if a given ID exists in member.csv, otherwise offers membership
int member_verification(char *id_verify)
{
    char data[200];
    char choice, membership_choice;
    int found;
    FILE *member = fopen("member.csv", "r"); // open for reading

    if (!member)
    {
        printf("Unable to open member file.\n");
        return 0;
    }

    found = 0;
    printf("Enter member ID to verify: ");
    // fgets(id_verify, 200, stdin);
    scanf("%s", id_verify);
    remove_newLine(id_verify);

    rewind(member);                    // move file pointer to start
    fgets(data, sizeof(data), member); // Skip header

    // Search through member records
    while (fgets(data, sizeof(data), member))
    {
        char *name = strtok(data, ",");
        char *user_id = strtok(NULL, ",");
        if (user_id && strcmp(user_id, id_verify) == 0)
        {
            printf("Member found\n");
            found = 1;
            break;
        }
    }

    // If not found, offer registration
    if (!found)
    {
        printf("You have to buy membership to issue books\n\n");
        printf("Do you want to buy membership (yes/no): ");
        // fgets(choice, sizeof(choice), stdin);
        scanf(" %c", &membership_choice);
        // remove_newLine(choice);
        if (membership_choice == 'y' || membership_choice == 'Y')
        {
            clear_input_buffer();
            application();
            return 1;
        }
        else
        {
            printf("Unable to issue a book !\n");
            return 0;
        }
    }

    fclose(member); // Close file
    return 1;
}

// Handles book issuing process
int issue()
{
    char id[200], title[200], author[200];
    char data_book[200], data_member[200];
    int found = 0;
    char date_str[20];
    Date issue_date;
    char choice;
    char id_verify[200];
    int is_verified = 0;
    int any_issue_done = 0; // Track if anything was issued at all

    
    
    do
    {
        FILE *stock = fopen("books.csv", "r");
        FILE *issue_file = fopen("issue.csv", "a+");
        FILE *member = fopen("member.csv", "r");
        
        if (!stock || !issue_file || !member)
        {
            printf("Unable to open file!\n");
            return 1;
        }
        rewind(stock);
        rewind(member);
        fseek(issue_file, 0, SEEK_END);
        long size = ftell(issue_file);
        if (size == 0)
        {
            fprintf(issue_file, "Member ID ,Book Title, Book ID, Issue Date\n");
            fflush(issue_file);
        }
    
        FILE *temp = fopen("temp.csv", "w");
        if (!temp)
        {
            printf("Unable to create temp file.\n");
            return 1;
        }
        // fgets(data_member, sizeof(data_member), member); // Skip headers
        fgets(data_book, sizeof(data_book), stock);

        found = 0;

        printf("To issue a book, you have to be a member\n");
        printf("Enter your member ID: ");
        scanf("%s", id_verify);
        remove_newLine(id_verify);

        is_verified = member_verification(id_verify);
        if (!is_verified)
        {
            printf("Member verification failed.\n");
            printf("Do you want to try again? (y/n): ");
            scanf(" %c", &choice);
            continue;
        }

        printf("Member verified.\n");

        printf("Enter book ID to issue: ");
        scanf("%s", id);
        remove_newLine(id);

        while (fgets(data_book, sizeof(data_book), stock))
        {
            char original_line[200];
            strcpy(original_line, data_book); // Backup

            char *book_id = strtok(data_book, ",");
            char *book_title = strtok(NULL, ",");
            char *book_author = strtok(NULL, ",");

            if (book_id && strcmp(book_id, id) == 0)
            {
                printf("Book available: %s by %s\n", book_title, book_author);
                found = 1;

                rewind(member);
                fgets(data_member, sizeof(data_member), member); // Skip header again

                int user_found = 0;
                while (fgets(data_member, sizeof(data_member), member))
                {
                    char *name = strtok(data_member, ",");
                    char *user_id = strtok(NULL, ",");

                    if (user_id && strcmp(id_verify, user_id) == 0)
                    {
                        user_found = 1;
                        printf("Enter date of issue (DD MM YYYY): ");
                        scanf("%d %d %d", &issue_date.day, &issue_date.month, &issue_date.year);
                        while (getchar() != '\n')
                            ; // Clear input buffer

                        format_date(issue_date, date_str, sizeof(date_str));

                        printf("Book issued to: %s, %s\n", name, user_id);
                        fprintf(issue_file, "%s,%s,%s,%s\n", user_id, book_title, book_id, date_str);
                        any_issue_done = 1; // Something was issued
                        break;
                    }
                }

                if (!user_found)
                {
                    printf("Member ID not found in record even after verification.\n");
                }
            }
            else
            {
                fputs(original_line, temp); // Keep book in temp
            }
        }

        if (is_verified && !found)
        {
            printf("Book not found in stock.\n");
        }

        fclose(stock);
        fclose(issue_file);
        fclose(member);
        fclose(temp);
    
        // Update only if something was issued
        if (any_issue_done)
        {
            remove("books.csv");
            rename("temp.csv", "books.csv");
        }
        else
        {
            remove("temp.csv");
        }
        printf("Do you want to issue another book? (y/n): ");
        scanf(" %c", &choice);

    } while (choice == 'y' || choice == 'Y');


    return 0;
}

// Main menu: allows repeating the issue process
void issue_books()
{
    char choice;
    do
    {
        issue(); // call issuing function
        printf("Do you want issue again (yes/no) : ");
        // fgets(choice, sizeof(choice), stdin);
        scanf(" %c", &choice);
        // remove_newLine(choice);
    } while (choice == 'y' || choice == 'Y');
}
