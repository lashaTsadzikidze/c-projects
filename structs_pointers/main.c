#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    char *name;
    int age;
    int height;
    int weight;
};

struct Person *create_person(char *name, int age, int height, int weight)
{
    struct Person *person = malloc(sizeof(struct Person));
    assert(person != NULL);

    person->name = strdup(name);
    person->age = age;
    person->height = height;
    person->weight = weight;

    return person;
}

void Person_destroy(struct Person *person)
{
    assert(person != NULL);

    free(person->name);
    free(person);
}

void Person_print(struct Person *person)
{
    printf("\nName: %s", person->name);
    printf("\nage: %d", person->age);
    printf("\nheight: %d", person->height);
    printf("\nweight: %d", person->weight);
}

int main(int argc, char *argv[])
{
    struct Person *joe = create_person("Joe Alex", 32, 64, 140);

    struct Person *frank = create_person("Frank Blank", 20, 72, 180);

    // print them out and were they ar in memory
    printf("Joe is at memory location %p:\n", joe);
    Person_print(joe);

    printf("\nFrank is at memory location %p:\n", frank);
    Person_print(frank);

    // change some data and print them again
    joe->age += 20;
    joe->height -= 2;
    joe->weight += 40;
    Person_print(joe);

    frank->age += 20;
    frank->weight += 20;
    Person_print(frank);

    // destroy them both so we clean up
    Person_destroy(joe);
    Person_destroy(frank);

    return 0;
}