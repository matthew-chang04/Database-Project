#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "database.h"



/*
 * Quicksort Algo for arranging database in alphabetical order
*/

int placeAndDivide(Record *records, int size, int left, int right) {
        // Generate a random pivot within the range
        int pivotIndex = left + (rand() % (right - left + 1));
        Record pivotRecord = records[pivotIndex];


        records[pivotIndex] = records[right];
        records[right] = pivotRecord;

        char pivotHandle[100];
        strcpy(pivotHandle, pivotRecord.handle);

        int wall = left -1;
        for (int i = left; i < right; i++) {
                if (strcmp(records[i].handle, pivotHandle) < 0){

                        wall++;

                        Record temp = records[i];
                        records[i] = records[wall];
                        records[wall] = temp;
                }

        }

        wall++;
        records[right] = records[wall];
        records[wall] = pivotRecord;
        return wall;
}

void quicksort(Record *records, int size, int left, int right){

        if (left >= right) {
                return;
        } else {
                int pivot = placeAndDivide(records,size, left, right);
                quicksort(records, size, left, pivot - 1);
                quicksort(records, size, pivot + 1, right);
        }
}



int main_loop(Database * db) {

    do {
            char buffer[300];
            char first[100];
            char second[100];
            char third[100];
            char *trash; // String to catch any extra (invalid arguments)
            printf("> ");
            fgets(buffer, 300, stdin);

            // number of arguments given
            int args = sscanf(buffer, "%s %s %s %s",first, second, third, trash);


            /* command functionalities
            */

            if (!strcmp(first, "list")) {

                if (args != 1) {
                    printf("Error: list command takes no additional arguments\n");
                } else if (db->size == 0) {
                    printf("There are no items to list\n");
                } else {

                    printf("| %-31s | %10s | %-16s | %-63s |\n", "HANDLE", "FOLLOWERS", "LAST MODIFIED", "COMMENT");

                    for (int i = 0; i < db->size; i++) {

                            char followerStr[20];
                            snprintf(followerStr, 20,"%lu", db->records[i].followers);

                            struct tm *timestamp = localtime(&(db->records[i].modified_date));

                            char lastModified[20];
                            strftime(lastModified, 20, "%Y-%m-%d %H:%M", timestamp);

                            printf("| %-31s | %10lu | %-16s | %-63s |\n", db->records[i].handle, db->records[i].followers, lastModified, db->records[i].comment);
                            }
                    }
                continue;

            } else if (!strcmp(first, "sort")) {

                if (args != 1) {
                    printf("Error: sort command takes no additional arguments\n");
                }
                else {
                    // Sort in alphabetical order
                    quicksort(db->records, db->size, 0, db->size - 1);

                    char const *path = "database.csv";
                    db_write_csv(db, path);
                    db->modified = 0; // Set false
                    printf("Error: you did not save your changes. Use `exit fr` to exit anyway\n");
                } else {
                    break;
                }
    
    
            } else if (!strcmp(first, "add") || !strcmp(first, "update")){
    
                if (args != 3) {
                    printf("Error: usage: %s HANDLE FOLLOWERS\n", first);
                    continue;
                }
    
                // Parse followers number and validate
                char *lastDigit;
                unsigned long followers = strtoul(third, &lastDigit, 10);
                char username[32];
    
                if (lastDigit == third){ // Input does not contain a valid number
    
                    printf("Follower count must be a valid integer\n");
                    continue;
                }
    
                // Initialize a pointer to the record we're altering or adding (needed when adding a comment
                Record *modifiedRecord;
    
    
                //Username Validation           
                if (!strcmp(first, "update")) {
                    if (!strchr(second, '@')){
                        printf("All valid usernames are preceeded by the @ symbol\n");
                        continue;
                        }
    
                    int userFound = 0;
                    for (int i = 0; i < db->size; i++) {
                        if (!(strcmp(db->records[i].handle, second))){
                            modifiedRecord = &(db->records[i]);
                            db->records[i].followers = followers;
                            db->records[i].modified_date = time(NULL);
                            db->modified = 1;
                            userFound = 1;
                            break;
                            }
                        }
                    if (!userFound) {
                        printf("No record exists with the given username\n");
                        continue;
                        }

                } else { // if the command was add
            
                    if (strchr(second, '\n') || strchr(second, ',') || strchr(second + 1, '@')){
            
                        printf("Username cannot contain commas or embedded @ symbol\n");
                        continue;
                        }
            
                    if (second[0] != '@') {
                        if (strlen(second) > 30) {
                            printf("Error: Username must be at most 30 characters + @ symbol at the beginning (total 31 characters)\n");
                            continue;
                        } else {
                            username[0] = '@';
                            strcpy(username + 1, second);
                        }
                    } else {
            
                        if (strlen(second) > 31) {
                            printf("Error: Username too long\n");
                                continue;
                        } else if (strlen(second) == 1) {
                            printf("Error: Username is empty\n");
                                continue;
                        } else {
                            strcpy(username, second);
                        }
                    }
            
                    // Check exisiting usernames
            
                    int taken = 0; // flag to check if the username was found already)      
                    for (int i = 0; i < db->size; i++) {
                        if (!strcmp(db->records[i].handle, username)){
                            printf("Error: Username is taken\n");
                            taken = 1;
                            break;
                            }
                        }
                    if (taken) {
                        continue;
                        }
            
            
            
                    // Adding Record
                    Record *newRecord;
                    newRecord = malloc(sizeof(Record));
                    strcpy(newRecord->handle, username);
                    newRecord->followers = followers;
                    newRecord->comment[0] = '\0';
                    newRecord->modified_date = (unsigned long) time(NULL); // current time (downcasted)
            
                    db->modified = 1;
                    db_append(db, newRecord);
                    free(newRecord);
            
                    modifiedRecord = &(db->records[db->size - 1]);

                    }

                // Prompting comment (same for both update and add)     
                char buffer[100];
                char comment[64];
                printf("Comment> ");
                fgets(buffer, 100, stdin);
                
                // Input validation for comment 
                if (strlen(buffer) > 63) {
                    printf("Comment is too long.\n");
                } else {
                    char *newlineptr = strchr(buffer, '\n');
                    if (*(newlineptr + 1) != '\0'){ // first newline is not at the end
                
                        printf("Error: comment cannot contain newline characters\n");
                
                    } else if (strchr(buffer, ',')) {
                
                        printf("Error: comment cannot contain commas.\n");
                    } else {
                        *newlineptr = '\0';
                        strcpy(modifiedRecord->comment, buffer);
                        modifiedRecord->modified_date = (unsigned long) time(NULL);
                        }
                    }
                
                continue;
                }
            } while (1);
    }
                
                
int main(void) {
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
    }
                                