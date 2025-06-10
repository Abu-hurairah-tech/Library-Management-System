#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "books.h"

#define MAX_BOOKS 300
#define MAX_TITLE_LENGTH 256

typedef struct
{
    int book_ID;
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
    printf("What do you want to do: \n");
    // printf("1. Enter 1 for check Books List. \n");
    printf("1. Add a book. \n");
    printf("2. Search a book. \n");
    printf("3. Delete a book. \n");
    printf("4. Exit. \n");
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
    int isFirstLine = 1;

    while (fgets(line, sizeof(line), file))
    {

        if (strlen(line) > 1 && bookCount < MAX_BOOKS)
        {
            char *token = strtok(line, ","); // taking the first thing before the comma in the string
            if (token)
            {
                books[bookCount].book_ID = atoi(token); // The first thing before the comma is ID but in string    format, atoi(token) will convert the string to an integer
            }

            token = strtok(NULL, ","); // taking the next thing before the next comma in the string
            if (token)
            {
                strncpy(books[bookCount].title, token, MAX_TITLE_LENGTH); // copying the string which is in the token variable to the title of the book (books[bookCount].title)
            }

            token = strtok(NULL, ",\n");
            if (token)
            {
                strncpy(books[bookCount].author, token, MAX_TITLE_LENGTH);
            }

            bookCount++;
        }
    }

    if (bookCount > 0)
    {
        nextBookID = books[bookCount - 1].book_ID + 1; // getting the next book ID
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

    int usedIDs[MAX_BOOKS] = {0};
    char line[256];
    int tempID;

    // Load existing IDs
    rewind(file);                    // ensure we read from start
    fgets(line, sizeof(line), file); // skip header if any
    while (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ","); // taking the first thing before the comma in the string
        if (token)
        {
            tempID = atoi(token); // The first thing before the comma is ID but in string format, atoi(token) will   convert the string to an integer
            if (tempID > 0 && tempID <= MAX_BOOKS)
                usedIDs[tempID - 1] = 1; // when the ID is used, it will be marked as 1 in the usedIDs array
        }
    }

    char title[MAX_TITLE_LENGTH];
    char author[MAX_TITLE_LENGTH];
    int newID;
    char choice;

    do
    {
        printf("Enter desired ID for the book: ");
        scanf("%d", &newID);
        getchar(); // clear newline

        if (newID <= 0 || newID > MAX_BOOKS) // checking if the ID is valid
        {
            printf("Invalid ID. Please enter a number between 1 and %d.\n", MAX_BOOKS);
            continue;
        }

        if (usedIDs[newID - 1] == 1) // checking if the ID is used
        {
            printf("ID %d is already occupied.\n", newID);
            printf("Please enter a different ID.\n");
        }

        printf("Enter book title: ");
        fgets(title, MAX_TITLE_LENGTH, stdin); // getting standard input from the user into the title array
        title[strcspn(title, "\n")] = '\0';    // removing the newline character from the end of the string

        printf("Enter book author: ");
        fgets(author, MAX_TITLE_LENGTH, stdin); // getting standard input from the user into the author array
        author[strcspn(author, "\n")] = '\0';

        // Ensure the file ends with a newline before appending
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        if (size > 0)
        {
            fseek(file, -1, SEEK_END);
            int lastChar = fgetc(file);
            if (lastChar != '\n')
            {
                fputc('\n', file);
            }
        }

        fprintf(file, "%d,%s,%s\n", newID, title, author);
        usedIDs[newID - 1] = 1;
        bookCount++;

        printf("Book added successfully with ID %d.\n", newID);

        printf("Do you want to add another book? (y/n): ");
        choice = fgetc(stdin);
        while (fgetc(stdin) != '\n')
            ; // flush newline
    } while (choice == 'y' || choice == 'Y');

    fclose(file);
}

void deleteBook()
{
    int delete_ID;
    char line[MAX_TITLE_LENGTH];
    char choice;
    char choice_line[10];
    do
    {
        int found = 0;
        FILE *original_File = fopen("books.csv", "r"); // opening the file in read mode
        FILE *temp_File = fopen("temp.csv", "w");      // opening the file in write mode

        if (original_File == NULL || temp_File == NULL)
        {
            printf("Could not open file.\n");
            return;
        }

        printf("Enter book ID to delete: ");
        scanf("%d", &delete_ID);
        getchar(); // clear newline from input buffer

        // Read and write header
        if (fgets(line, sizeof(line), original_File))
        {
            fputs(line, temp_File); // write header
        }

        // Read book lines
        while (fgets(line, sizeof(line), original_File))
        {
            char lineCopy[MAX_TITLE_LENGTH];
            strcpy(lineCopy, line); // in case we want to re-use

            char *token = strtok(line, ",");
            if (token == NULL)
            {
                continue;
            }
            int current_ID = atoi(token);

            if (current_ID == delete_ID)
            {
                found = 1;
                continue; // skip writing this line (delete it)
            }

            fputs(lineCopy, temp_File); // write original line back
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
        printf("\nDo you want to delete another book? (y/n): ");
        fgets(choice_line, sizeof(choice_line), stdin);
        remove_newline(choice_line);
        choice = choice_line[0];
    } while (choice == 'y' || choice == 'Y');
}

void searchBookByID()
{
    FILE *ptr;
    char line[200];
    int searchID;
    int found;
    char choice;

    ptr = fopen("books.csv", "r");
    if (ptr == NULL)
    {
        printf("Could not open file!\n");
        return;
    }

    do
    {
        found = 0;                      // Reset found flag
        rewind(ptr);                    // Rewind the file to the beginning
        fgets(line, sizeof(line), ptr); // Skip header (if any)

        printf("Enter book ID to search: ");
        scanf("%d", &searchID);
        getchar(); // Clear leftover newline

        while (fgets(line, sizeof(line), ptr))
        {
            char *token = strtok(line, ","); // Extract the ID

            if (token)
            {
                int currentID = atoi(token);
                if (currentID == searchID)
                {
                    char *title = strtok(NULL, ",");
                    char *author = strtok(NULL, ",");
                    printf("\nBook found:\n");
                    printf("ID: %d\n", currentID);
                    printf("Title: %s\n", title);
                    printf("Author: %s\n", author);
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            printf("No book found with ID %d.\n", searchID);
        }

        printf("\nDo you want to search again? (y/n): ");
        scanf(" %c", &choice); // Note the space before %c

    } while (choice == 'Y' || choice == 'y');

    fclose(ptr);
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
            loadBooksFromFile();
            addBook();
            break;
        case 2:
            searchBookByID();
            break;
        case 3:
            deleteBook();
            break;
        case 4:
            printf("Exiting...\n");
            return;
        default:
            printf("\nInvalid choice. Please try again.\n");
            // scanf(" %c", &choice);
        }
    } while (choice != 4);
}
