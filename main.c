#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//this structure will be used to add start and end dates
typedef struct Date {
    int year, month, day;
} Date;

//all information required for a task is entered here
typedef struct taskData {
    char name[20], description[200], dependency[20];
    char* category;
    int priority;
    Date startdDate, endDate;
} taskData;

typedef struct Node {
    taskData data;
    struct Node* next;
} Node;

//the head node always points to NULL

//node for storing categories
typedef struct CategoryNode {
    char categoryName[20];
    struct CategoryNode* next;
} CategoryNode;


void addTask(Node** head, CategoryNode** categoriesHead); // option 1 of menu
void viewTask(Node* head); // option 2 of menu
void editTask(Node* head); // option 3 of menu
void deleteTask(Node** head); // option 4 of menu
void exitProgram(Node* head, CategoryNode* categoriesHead); // option 5 of menu
void newCategory(CategoryNode** categoriesHead, char* categoryName); //used to add new category to the list
void displayCategories(CategoryNode* categoriesHead); // displays all the available categories
void saveTasksToFile(Node* head); //it overwrites the whole list into the file
void loadTasksFromFile(Node** head); //all the tasks from the file are loaded into memory at the start of the program
void saveCategoriesToFile(CategoryNode* categoriesHead); //it overwrites the whole list of categories into the file
void loadCategoriesFromFile(CategoryNode** categoriesHead); //all the categories from the file are loaded into memory at the start of the program


int main() {
    Node* head = NULL;
    CategoryNode* categoriesHead = NULL;

    loadTasksFromFile(&head);
    loadCategoriesFromFile(&categoriesHead);
    int choice;

    //the menu is placed inside a do while loop so that it gets reinstated again after every task up until the exit program function is selected.
    do {
        //tab spaces given in menu to look nice in the console
        printf("\tMenu:\n");
        printf("\t1. Add task\n");
        printf("\t2. View tasks\n");
        printf("\t3. Edit task\n");
        printf("\t4. Delete task\n");
        printf("\t5. Exit\n");

        printf("\tEnter your choice (1-5): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addTask(&head, &categoriesHead);
                break;
            case 2:
                viewTask(head);
                break;
            case 3:
                editTask(head);
                break;
            case 4:
                deleteTask(&head);
                break;
            case 5:
                exitProgram(head, categoriesHead);
                printf("\nExiting program. Goodbye!\n");
                break;
            default:
                printf("\nWrong input. Please enter a number between 1 and 5.\n");
        }
    } while (choice != 5);

    // Free memory for tasks
    while (head != NULL) {
        //it will start from the first element and one by one free each element in the list
        Node* temp = head;
        head = head->next;
        free(temp);
    }

    // Free memory for categories
    CategoryNode* currentCategory = categoriesHead;
    while (currentCategory != NULL) {
        CategoryNode* temp = currentCategory;
        currentCategory = currentCategory->next;
        free(temp);
    }

    return 0;
}

void addTask(Node** head, CategoryNode** categoriesHead) {
    struct taskData addTaskData = {"", "", "", ""}; // initializing members individually
    char descriptionChoice, dependecyChoice, priorityChoice; //used to ask user if they want to give a description, dependency or priority

    printf("\nOption 1: Add task selected.\n");

    printf("Enter the name of the task: ");
    scanf(" %[^\n]", addTaskData.name);

    printf("\nDo you want to give a short description of the task? (y/n): ");
    scanf(" %c", &descriptionChoice);
    if (tolower(descriptionChoice) == 'y') {
        printf("\nWrite a description of your task in less than 200 characters: ");
        scanf(" %[^\n]", addTaskData.description);
    } else {
        strcpy(addTaskData.description, "No_Description"); //if user decides to not give a description for their task
    }

    displayCategories(*categoriesHead); //displays all the categories that are already present for the user to choose from
    printf("\nEnter the category for your task: ");
    char category[20];
    scanf(" %[^\n]", category);

    // Check if the category already exists. If it is already exists the category is not duplicated
    CategoryNode* currentCategory = *categoriesHead;
    bool categoryExists = false;
    while (currentCategory != NULL) {
        if (strcmp(currentCategory->categoryName, category) == 0) {
            categoryExists = true;
            break;
        }
        currentCategory = currentCategory->next;
    }

    // If the category does not exist, we add it to the list of categories.
    if (!categoryExists) {
        newCategory(categoriesHead,category);
    }

    // Allocate memory for the category and copy the name of the category into the newly allocated memory
    addTaskData.category = (char*)malloc(strlen(category) + 1);
    strcpy(addTaskData.category, category);

    printf("\nDoes this task depend on any other task? (enter y or n) ");
    scanf(" %c", &dependecyChoice);
    if (tolower(dependecyChoice) == 'y') {
        printf("Please enter the name of the task, which it depends upon: ");
        scanf(" %[^\n]", addTaskData.dependency);
    } else {
        strcpy(addTaskData.dependency, "No_Dependencies"); //if user decides to not create dependencies.
    }

    printf("\nDo you want to set a priority to this task? (enter y or n) ");
    scanf(" %c", &priorityChoice);
    if (tolower(priorityChoice) == 'y') {
        printf("Please enter the priority of this task ");
        scanf(" %d", &addTaskData.priority);
    } else {
        // If no priority is given, assign the last priority.
        Node* temp = *head;
        while (temp != NULL && temp->next != NULL) {
            temp = temp->next;
        }
        if (temp != NULL) {
            addTaskData.priority = temp->data.priority + 1;
        } else {
            addTaskData.priority = 1; // If the list is empty, assign priority 1
        }
    }

    // Check if the priority already exists
    Node* current = *head;

    // If the priority already exists, adjust priorities for tasks with the same priority or greater
    if (current != NULL && current->data.priority == addTaskData.priority) {
        while (current != NULL) {
            current->data.priority=current->data.priority+1;
            current = current->next;
        }
    }

    printf("\nWhat is the start date of this task? (write it in the format: yyyy mm dd) ");
    scanf("%d %d %d", &addTaskData.startdDate.year, &addTaskData.startdDate.month, &addTaskData.startdDate.day);

    printf("What is the end date of this task? (write it in the format: yyyy mm dd) ");
    scanf("%d %d %d", &addTaskData.endDate.year, &addTaskData.endDate.month, &addTaskData.endDate.day);

    Node* newCurrent = *head;
    Node* previous = NULL;
    // Insert the new task into the correct position
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = addTaskData;
    newNode->next = newCurrent;
    *head = newNode;

    if (previous != NULL) {
        previous->next = newNode;
    } else {
        //head = newNode;
        newNode = *head;
    }

    printf("\nTask added successfully! Please continue with the menu.\n");
}

void viewTask(Node* head) {

    printf("\nOption 2: View task selected.\n");

    if (head == NULL) {
        printf("No tasks available.\n");
    } else {
        printf("Task List:\n\n");

        Node* temp = head;
        while (temp != NULL) {
            printf("Name: %s\n", temp->data.name);
            printf("Description: %s\n", temp->data.description);
            printf("Category: %s\n", temp->data.category);
            printf("Dependency: %s\n", temp->data.dependency);
            printf("Priority: %d\n", temp->data.priority);
            printf("Start Date: %d / %02d / %02d \n", temp->data.startdDate.year, temp->data.startdDate.month, temp->data.startdDate.day);
            printf("End Date: %d / %02d / %02d \n", temp->data.endDate.year, temp->data.endDate.month, temp->data.endDate.day);
            printf("--------------------------\n");

            temp = temp->next;
        }
    }
}



void editTask(Node* head) {
    printf("\nOption 3: Edit task selected.\n");
    int editChoice;

    viewTask(head);
    int taskPriority;
    printf("Enter the priority number of the task you want to edit: ");
    scanf("%d", &taskPriority);

    Node* temp = head;
    int foundFlag=0;

    while (temp != NULL) {
        if (temp->data.priority == taskPriority) {
            foundFlag=1;
            break;
        }
         temp = temp->next;
    }


    if (foundFlag == 0) {
        printf("Task not found.\n");
        return;
    }
    do{
        printf("Choose what you want to edit:\n(press 7 when you are done editing)\n");
        printf("1. Priority\n");
        printf("2. Description\n");
        printf("3. Category\n");
        printf("4. Dependency\n");
        printf("5. Start Date\n");
        printf("6. End Date\n");
        printf("Enter your choice (1-6): ");
        scanf("%d", &editChoice);

        switch (editChoice) {
            case 1:
                printf("Enter the new priority: ");
                scanf("%d", &temp->data.priority);
                break;
            case 2:
                printf("Enter the new description: ");
                scanf(" %[^\n]", temp->data.description);
                break;
            case 3:
                printf("Enter the new category: ");
                scanf(" %[^\n]", temp->data.category);
                break;
            case 4:
                printf("Enter the new dependency: ");
                scanf(" %[^\n]", temp->data.dependency);
                break;
            case 5:
                printf("Enter the new start date (yyyy mm dd): ");
                scanf("%d %d %d", &temp->data.startdDate.year, &temp->data.startdDate.month, &temp->data.startdDate.day);
                break;
            case 6:
                printf("Enter the new end date (yyyy mm dd): ");
                scanf("%d %d %d", &temp->data.endDate.year, &temp->data.endDate.month, &temp->data.endDate.day);
                break;
            case 7:
                printf("Task edited successfully!\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    }while(editChoice!=7);

    return;
}

void deleteTask(Node** head) {
    printf("\nOption 4: Delete task selected.\n");
    Node* newhead = *head;
    viewTask(newhead);

    int taskPriority;
    printf("Enter the priority number of the task you want to delete: ");
    scanf("%d", &taskPriority);

    Node* current = *head;
    Node* previous = NULL;

    while (current != NULL && current->data.priority != taskPriority) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Task with priority %d not found.\n", taskPriority);
        return;
    }

    // Remove the task from the linked list
    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    // Move all tasks below the deleted task one priority up
    Node* temp = *head;
    while (temp != NULL) {
        if (temp->data.priority > taskPriority) {
            temp->data.priority--;
        }
        temp = temp->next;
    }

    free(current);

    printf("Task with priority %d deleted successfully!\n", taskPriority);
}

void saveTasksToFile(Node* head) {
    FILE* file = fopen("tasks.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    Node* temp = head;
    while (temp != NULL) {
        fprintf(file, "\"%s\" \"%s\" \"%s\" \"%s\" %d %d %d %d %d %d %d\n", temp->data.name
                                                           , temp->data.description
                                                           , temp->data.category
                                                           , temp->data.dependency
                                                           , temp->data.priority
                                                           , temp->data.startdDate.year
                                                           , temp->data.startdDate.month
                                                           , temp->data.startdDate.day
                                                           , temp->data.endDate.year
                                                           , temp->data.endDate.month
                                                           , temp->data.endDate.day);

        temp = temp->next;
    }

    fclose(file);
}


void loadTasksFromFile(Node** head) {
    FILE* file = fopen("tasks.txt", "r");
    if (file == NULL) {
        printf("No previous tasks found.\n");
        return;
    }

    taskData loadedTask;
    char temp[1000];  // Temporary buffer for reading lines

    while (fgets(temp, sizeof(temp), file) != NULL) {
        // Use appropriate format specifiers based on the structure of your file
        loadedTask.category = (char*)malloc(sizeof(char) * 50);
        sscanf(temp, "\"%[^\"]\" \"%[^\"]\" \"%[^\"]\" \"%[^\"]\" %d %d %d %d %d %d %d"  , loadedTask.name
                                                           , loadedTask.category
                                                           , loadedTask.dependency
                                                           , loadedTask.description
                                                           , &loadedTask.priority
                                                           , &loadedTask.startdDate.year
                                                           , &loadedTask.startdDate.month
                                                           , &loadedTask.startdDate.day
                                                           , &loadedTask.endDate.year
                                                           , &loadedTask.endDate.month
                                                           , &loadedTask.endDate.day);


        // Create a new node and add it to the linked list
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->data = loadedTask;
        newNode->next = NULL;

        // Add the new node to the linked list in ascending order based on priority
        if (*head == NULL || newNode->data.priority < (*head)->data.priority) {
        newNode->next = *head;
        *head = newNode;
        } else {
            Node* current = *head;
            while (current->next != NULL && current->next->data.priority < newNode->data.priority) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }
     fclose(file);

}


void exitProgram(Node* head, CategoryNode* categoriesHead) {
    saveTasksToFile(head);
    saveCategoriesToFile(categoriesHead);
}

void newCategory(CategoryNode** categoriesHead, char* categoryName){
    CategoryNode* newCategoryNode = (CategoryNode*)(malloc(sizeof(CategoryNode)));
    strcpy(newCategoryNode->categoryName, categoryName);
    newCategoryNode->next=*categoriesHead;
    *categoriesHead=newCategoryNode;
}

void displayCategories(CategoryNode* categoriesHead) {
    CategoryNode* current = categoriesHead;
    printf("Categories:\n");
    while (current != NULL) {
        printf("%s\n", current->categoryName);
        current = current->next;
    }
}

void saveCategoriesToFile(CategoryNode* categoriesHead) {
    FILE* file = fopen("categories.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing categories.\n");
        return;
    }

    CategoryNode* current = categoriesHead;
    while (current != NULL) {
        fprintf(file, "%s\n", current->categoryName);
        current = current->next;
    }

    fclose(file);
}

void loadCategoriesFromFile(CategoryNode** categoriesHead) {
    FILE* file = fopen("categories.txt", "r");
    if (file == NULL) {
        printf("No previous categories found.\n");
        return;
    }

    char categoryName[20];
    while (fscanf(file, "%s\n", categoryName) == 1) {
        newCategory(categoriesHead,categoryName);
    }

    fclose(file);
}
