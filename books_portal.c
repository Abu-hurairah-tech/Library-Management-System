#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "books.h"

#define MAX_BOOKS 300
#define MAX_TITLE_LENGTH 256
#define MAX_ID_LENGTH 300

typedef struct
{
    char book_ID[MAX_ID_LENGTH];
    char title[MAX_TITLE_LENGTH];
    char author[MAX_TITLE_LENGTH];
} Book;

Book books[MAX_BOOKS];
int bookCount = 0;
int nextBookID = 1;

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

void displayMenu()
{
    printf("\n\n\t\t\t=== Book Management System ===\n\n");
    printf("What do you want to do: \n");
    // printf("1. Enter 1 for check Books List. \n");
    printf("1. Add a book. \n");
    printf("2. Search a book. \n");
    printf("3. Delete a book. \n");
    printf("4. Display all books.\n");
    printf("5. Exit. \n");
}

void loadBooksFromFile()
{
    FILE *file = fopen("books.csv", "r");
    if (file == NULL)
    {
        printf("No existing file found. A new one will be created.\n");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {
        if (strlen(line) > 1 && bookCount < MAX_BOOKS)
        {
            char *token = strtok(line, ",");
            if (token)
                strncpy(books[bookCount].book_ID, token, MAX_ID_LENGTH);

            token = strtok(NULL, ",");
            if (token)
                strncpy(books[bookCount].title, token, MAX_TITLE_LENGTH);

            token = strtok(NULL, ",\n");
            if (token)
                strncpy(books[bookCount].author, token, MAX_TITLE_LENGTH);

            bookCount++;
        }
    }

    fclose(file);
}

void addBook()
{
    FILE *file = fopen("books.csv", "a+");
    if (file == NULL)
    {
        printf("Could not open file!\n");
        return;
    }

    char newID[MAX_ID_LENGTH];
    char title[MAX_TITLE_LENGTH];
    char author[MAX_TITLE_LENGTH];
    char line[256];
    char existingID[MAX_ID_LENGTH];
    char choice;

    // Write header if file is empty
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
    {
        fprintf(file, "ID,Title,Author\n");
        fflush(file);
    }

    do
    {
        while (1)
        {

            int duplicate = 0;
            rewind(file);
            fgets(line, sizeof(line), file); // skip header

            printf("Enter desired ID for the book: ");
            fgets(newID, MAX_ID_LENGTH, stdin);
            remove_newline(newID);

            while (fgets(line, sizeof(line), file))
            {
                strncpy(existingID, strtok(line, ","), MAX_ID_LENGTH);
                if (strcmp(existingID, newID) == 0)
                {
                    duplicate = 1;
                    break;
                }
            }

            if (duplicate)
            {
                printf("ID '%s' already exists. Try another.\n", newID);
                continue;
            }

            printf("Enter book title: ");
            fgets(title, MAX_TITLE_LENGTH, stdin);
            remove_newline(title);

            printf("Enter book author: ");
            fgets(author, MAX_TITLE_LENGTH, stdin);
            remove_newline(author);

            if (strlen(newID) == 0 || strlen(title) == 0 || strlen(author) == 0)
            {
                printf("\nAll fields must be filled! Please try again.\n\n");
            }
            else
            {
                break; // all fields are filled
            }
        }

        fprintf(file, "%s,%s,%s\n", newID, title, author);
        printf("Book added successfully with ID '%s'.\n", newID);
        bookCount++;

        printf("Do you want to add another book? (y/n): ");
        choice = fgetc(stdin);
        while (fgetc(stdin) != '\n')
            ;
    } while (choice == 'y' || choice == 'Y');

    fclose(file);
}

void deleteBook()
{
    char delete_ID[MAX_ID_LENGTH];
    char line[256];
    char choice, choice_line[10];

    do
    {
        while (1)
        {
            int found = 0;
            FILE *original_File = fopen("books.csv", "r");
            FILE *temp_File = fopen("temp.csv", "w");

            if (original_File == NULL || temp_File == NULL)
            {
                printf("Could not open file.\n");
                return;
            }

            printf("Enter book ID to delete: ");
            fgets(delete_ID, sizeof(delete_ID), stdin);
            remove_newline(delete_ID);
            if (strlen(delete_ID) == 0)
            {
                printf("\nID cannot be empty! Please try again.\n\n");
                fclose(original_File);
                fclose(temp_File);
                remove("temp.csv");
                continue;
            }

            if (fgets(line, sizeof(line), original_File))
            {
                fputs(line, temp_File);
            }

            while (fgets(line, sizeof(line), original_File))
            {
                char lineCopy[MAX_TITLE_LENGTH];
                strcpy(lineCopy, line);

                char *token = strtok(line, ",");
                if (token && strcmp(token, delete_ID) == 0)
                {
                    found = 1;
                    continue; // skip writing this line
                }

                fputs(lineCopy, temp_File);
            }

            fclose(original_File);
            fclose(temp_File);

            if (found)
            {
                remove("books.csv");
                rename("temp.csv", "books.csv");
                printf("Book Deleted Successfully!\n");
            }
            else
            {
                remove("temp.csv");
                printf("Book not found!\n");
            }
            break;
        }

        printf("\nDo you want to delete another book? (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');
}

void searchBookByID()
{
    FILE *ptr = fopen("books.csv", "r");
    char line[256], searchID[MAX_ID_LENGTH], *title, *author;
    int found;
    char choice;

    if (ptr == NULL)
    {
        printf("Could not open file!\n");
        return;
    }

    do
    {
        while (1)
        {

            found = 0;
            rewind(ptr);
            fgets(line, sizeof(line), ptr); // Skip header

            printf("Enter book ID to search: ");
            fgets(searchID, MAX_ID_LENGTH, stdin);
            remove_newline(searchID);
            if (strlen(searchID) == 0)
            {
                printf("\nID cannot be empty! Please try again.\n\n");
                continue;
            }

            while (fgets(line, sizeof(line), ptr))
            {
                char *token = strtok(line, ",");
                if (token && strcmp(token, searchID) == 0)
                {
                    title = strtok(NULL, ",");
                    author = strtok(NULL, ",\n");

                    printf("\nBook found:\n");
                    printf("ID: %s\n", token);
                    printf("Title: %s\n", title);
                    printf("Author: %s\n", author);
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                printf("No book found with ID '%s'.\n", searchID);
            }
            break; // Exit the while loop after searching
        }

        printf("\nDo you want to search again? (y/n): ");
        scanf(" %c", &choice);
        getchar();
    } while (choice == 'y' || choice == 'Y');

    fclose(ptr);
}

void displayBooks()
{
    FILE *file = fopen("books.csv", "r");
    char line[512];
    int count = 0;

    if (file == NULL)
    {
        printf("Could not open file!\n");
        return;
    }

    fgets(line, sizeof(line), file); // Skip header

    printf("\n\n\t\t\t=== Book List ===\n\n");
    printf("%-10s | %-30s | %-20s\n", "Book ID", "Title", "Author");
    printf("--------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file))
    {
        char *id = strtok(line, ",");
        char *title = strtok(NULL, ",");
        char *author = strtok(NULL, ",\n");

        if (id && title && author)
        {
            printf("%-10s | %-30s | %-20s\n", id, title, author);
            count++;
        }
    }

    if (count == 0)
    {
        printf("No books found.\n");
    }

    printf("\nTotal books: %d\n", count);
    fclose(file);
}

void manage_books()
{

    int choice;
    do
    {
        displayMenu();
        printf("Enter your choice: ");
        scanf(" %d", &choice);
        getchar();
        switch (choice)
        {
        case 1:
            printf("\n\n\t\t\t=== Add a Book ===\n\n");
            loadBooksFromFile();
            addBook();
            break;
        case 2:
            printf("\n\n\t\t\t=== Search a Book ===\n\n");
            searchBookByID();
            break;
        case 3:
            printf("\n\n\t\t\t=== Delete a Book ===\n\n");
            deleteBook();
            break;
        case 4:
            printf("\n\n\t\t\t=== Display All Books ===\n\n");
            displayBooks();
            break;
        case 5:
            printf("Exiting...\n\n");
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            // scanf(" %c", &choice);
        }
    } while (choice != 5);
}
