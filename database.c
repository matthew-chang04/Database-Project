#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



Database db_create() {
        Database db;
        db.size = 0;
        db.capacity = 4;
        // memory allocation
        db.records = malloc(db.capacity * sizeof(Record));
        if (db.records == NULL) {
                printf("Error: failed the allocation of memory for database.\n");
                exit(1);
        }
        return db;
}



void db_append(Database *db, Record const *item) {
        if (db->size >= db->capacity) {
                db->capacity *= 2;
                Record *newRecords = realloc(db->records, db->capacity * sizeof(Record));
                if (newRecords == NULL) {
                        printf("Error: Failed the reallocation of memory for DAtabase recors.\n");
                        exit(1);
                }
                db->records = newRecords;
        }
        db->records[db->size] = *item;
        db->size++;
}


Record *db_index(Database *db, int index) {
        if (index < 0 || index >= db->size) {
                printf("Error: index %d is out of bounds. (needs to be between 0 and %d)\n", index, db->size - 1);
                return NULL;
        }
        return &(db->records[index]);
}



Record *db_lookup(Database *db, char const *handle) {
        for (int i = 0; i < db->size; i++) {
            if (strcmp(db->records[i].handle, handle) == 0) {
                return &(db->records[i]);
        }
}
return NULL;
}


/* frees memory allocated
* inputs db (pointer in database)
*/
void db_free(Database *db) {
free(db->records);
db->records = NULL;
db->size = 0;
db->capacity = 0;
}

/*
* parse a csv line into a record structure
* format = handle, followers, comment, last_modified
* input: line (pointer to line)
*/
Record parse_record(char const *line) {
Record rec;

// create copy of line 
char *line_copy = strdup(line);
if (line_copy == NULL){
        printf("Unable to duplicate line\n");
        exit(1);
}

//split string by comma
char *token = strtok(line_copy, ",");
if (token == NULL) {
        printf("Csv line missing the handle fied: %s\n", line);
        free(line_copy);
        exit(1);
}

// copy token
strncpy(rec.handle, token, sizeof(rec.handle) - 1);
rec.handle[sizeof(rec.handle) - 1] = '\0'; // null at the end to terminate

// second token = follower count
token = strtok(NULL, ",");
if (token == NULL) {
        printf("CSv line missing the follower field: %s\n", line);
        free(line_copy);
        exit(1);
}
rec.followers = strtoul(token, NULL, 10);

// 3rd token = comment
token = strtok(NULL, ",");
if (token == NULL) {
        printf("Csv line missing the comment field: %s\n", line);
        free(line_copy);
        exit(1);
        }
strncpy(rec.comment, token, sizeof(rec.comment) - 1);
rec.comment[sizeof(rec.comment) - 1] = '\0';

// 4th token = last_modified
token = strtok(NULL, ",");
if (token == NULL) {
        printf("Csv line missing the last_modified field: %s\n", line);
        free(line_copy);
        exit(1);
        }
rec.modified_date = strtoul(token, NULL, 10);

// free copy, not needed anymore
free(line_copy);
return rec;
}


void db_load_csv(Database *db, char const *path) {
FILE *fp = fopen(path, "r");
if (fp == NULL) {
        printf("Unable to open file %s for reading\n", path);
        exit(1);
    }

char *line = NULL;
size_t len = 0;

while (getline(&line, &len, fp) != -1) {
        // read each line
        // remove newline character
        size_t line_length = strlen(line);
        if (line_length > 0 && line[line_length - 1] == '\n') {
                line[line_length - 1] = '\0';
        }

        Record rec = parse_record(line);
        // parse line -> record
        db_append(db, &rec);
    }
free(line);
fclose(fp);
}


void db_write_csv(Database *db, char const *path) {
FILE *fp = fopen(path, "w");
if (fp == NULL) {

        printf("Unable to open file %s for writing\n", path);
        exit(1);
    }
// record -> csv format
for (int i = 0; i < db->size; i++) {
    fprintf(fp, "%s,%lu,%s,%lu\n", db->records[i].handle, db->records[i].followers, db->records[i].comment, db->records[i].modified_date);
    }
fclose(fp);
}                                                                                                                                                                1,1           Top
