#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address {
    int id;
    int set;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

struct Database {
    struct Address rows[MAX_ROWS];
};

struct Connection {
    FILE *file;
    struct Database *db;
};

void die(const char *message)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

void Address_print(struct Address *address)
{
    printf("%d %s %s\n", address->id, address->name, address->email);
}

void Database_load(struct Connection *connection)
{
    int result = fread(connection->db, sizeof(struct Database), 1, connection->file);
    if (result != 1)
        die("Failed to load database");
}

struct Connection *Database_open(const char *filename, char mode)
{
    struct Connection *connection = malloc(sizeof(struct Connection));
    if (connection == NULL)
        die("Memory error");
    
    connection->db = malloc(sizeof(struct Database));
    if (connection->db == NULL)
        die("Memory error");
    
    if (mode == 'c') {
        connection->file = fopen(filename, "w");
    } else {
        connection->file = fopen(filename, "r+");

        if (connection->file != NULL) {
            Database_load(connection);
        }
    }

    if (connection->file == NULL)
        die("Failed to open file");
    
    return connection;
}

void Database_close(struct Connection *connection)
{
    if (connection != NULL) {
        if (connection->file != NULL)
            fclose(connection->file);
        if (connection->db != NULL)
            free(connection->db);
        free(connection);
    }
}

void Database_write(struct Connection *connection)
{
    rewind(connection->file);

    int result = fwrite(connection->db, sizeof(struct Database), 1, connection->file);
    if (result != 1)
        die("Failed to write database");
    
    result = fflush(connection->file);
    if (result == -1)
        die("Cannot flush database.");
}

void Database_create(struct Connection *connection)
{
    int i = 0;

    for (i = 0; i < MAX_ROWS; i++) {

        struct Address address = {i, 0};
        
        connection->db->rows[i] = address;
    }
}

void Database_set(struct Connection *connection, int id,
                    const char *name, const char *email)
{
    struct Address *address = &connection->db->rows[id];
    if (address->set == 1)
        die("Already set, delete it first");
    
    address->set = 1;

    char *response = strncpy(address->name, name, MAX_DATA);

    if (response == NULL)
        die("Name copy failed");
    
    response = strncpy(address->email, email, MAX_DATA);
    if (response == NULL)
        die("Email copy failed");
}

void Database_get(struct Connection *connection, int id)
{
    struct Address *address = &connection->db->rows[id];

    if (address->set == 1) {
        Address_print(address);
    } else {
        die("ID is not set");
    }
}

void Database_delete(struct Connection *connection, int id)
{
    struct Address address = {id, 0};
    connection->db->rows[id] = address;
}

void Database_list(struct Connection *connection)
{
    int i = 0;
    struct Database *db = connection->db;

    for (i = 0; i < MAX_ROWS; i++) {
        struct Address *current_address = &db->rows[i];
        
        if (current_address->set) {
            Address_print(current_address);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        die("USAGE: main <dbfile> <action> [actions params]");
    
    char *filename = argv[1];
    char action = argv[2][0];
    struct Connection *connection = Database_open(filename, action);
    int id = 0;

    if (argc > 3) id = atoi(argv[3]);
    if (id >= MAX_ROWS) die("There is not that many records.");

    switch(action) {
        case 'c':
            Database_create(connection);
            Database_write(connection);
            break;
        
        case 'g':
            if (argc != 4)
                die("Need an id to get");
            Database_get(connection, id);
            break;
        
        case 's':
            if (argc != 6)
                die("Need id, name, email to set");
            Database_set(connection, id, argv[4], argv[5]);
            Database_write(connection);
            break;
        
        case 'd':
            if (argc != 4)
                die("Need id to delete");
            
            Database_delete(connection, id);
            Database_write(connection);
            break;
        
        case 'l':
            Database_list(connection);
            break;
        
        default:
            die("Invalid action: c=create, g=get, s=set, d=del, l=list");
    }

    Database_close(connection);

    return 0;
}