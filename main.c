#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "linked_list_node.h"

#ifdef USE_SERIAL
    #include "linked_list.h"         // Include serial version
#elif defined(USE_MUTEX)
    #include "linked_list_mutex.h"   // Include mutex-based version
#elif defined(USE_RWLOCK)
    #include "linked_list_rwlock.h"  // Include read-write lock version
#endif

// Global input parameters
int n, m, thread_count;
double mMember, mInsert, mDelete;

// Function pointers for operations
typedef int (*MemberFn)(int, list_node_t*);
typedef int (*InsertFn)(int, list_node_t**);
typedef int (*DeleteFn)(int, list_node_t**);

// Function to get user inputs
void get_user_inputs() {
    printf("Enter the number of initial values (n): ");
    scanf("%d", &n);
    
    printf("Enter the number of operations (m): ");
    scanf("%d", &m);
    
    printf("Enter the fraction of Member operations (mMember): ");
    scanf("%lf", &mMember);
    
    printf("Enter the fraction of Insert operations (mInsert): ");
    scanf("%lf", &mInsert);
    
    printf("Enter the fraction of Delete operations (mDelete): ");
    scanf("%lf", &mDelete);
    
    printf("Enter the number of threads: ");
    scanf("%d", &thread_count);
}

// Function to generate a random unique number between 0 and MAX_VALUE (65535)
int generate_unique_value(int* existing_values, int size) {
    int value;
    int found;
    do {
        found = 0;
        value = rand() % 65536; // Values between 0 and 65535
        for (int i = 0; i < size; i++) {
            if (existing_values[i] == value) {
                found = 1;
                break;
            }
        }
    } while (found);
    return value;
}

// Populate the linked list with n unique random values
void populate_list(list_node_t** head_pp, int n, InsertFn insert_fn) {
    int* existing_values = (int*) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        int value = generate_unique_value(existing_values, i);
        existing_values[i] = value;
        insert_fn(value, head_pp);  // Use the provided Insert function
    }
    free(existing_values);
}

// Function to perform random operations on the linked list
void perform_random_operations(list_node_t** head_pp, int m, double mMember, double mInsert, double mDelete, 
                               MemberFn member_fn, InsertFn insert_fn, DeleteFn delete_fn) {
    int mMember_count = m * mMember;
    int mInsert_count = m * mInsert;
    int mDelete_count = m * mDelete;
    int total_count = 0;

    while (total_count < m) {
        int operation = rand() % 3;  // Randomly choose operation (0: Member, 1: Insert, 2: Delete)
        int value = rand() % 65536;  // Random value between 0 and 65535

        if (operation == 0 && mMember_count > 0) {
            // Perform Member operation
            member_fn(value, *head_pp);
            mMember_count--;
        } else if (operation == 1 && mInsert_count > 0) {
            // Perform Insert operation
            insert_fn(value, head_pp);
            mInsert_count--;
        } else if (operation == 2 && mDelete_count > 0) {
            // Perform Delete operation
            delete_fn(value, head_pp);
            mDelete_count--;
        }
        total_count++;
    }
}

// General testing function
void test_linked_list(const char* list_name, list_node_t** head_pp, MemberFn member_fn, InsertFn insert_fn, DeleteFn delete_fn) {
    // Populate the list
    populate_list(head_pp, n, insert_fn);
    printf("List Polulated!\n");
    
    // Start timer
    clock_t start = clock();
    
    // Perform operations
    perform_random_operations(head_pp, m, mMember, mInsert, mDelete, member_fn, insert_fn, delete_fn);
    printf("Executed Operations!\n");
    
    // Stop timer
    clock_t end = clock();
    double elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    // Print the result
    printf("%s Linked List Time: %.2f seconds\n", list_name, elapsed);
}

int main() {
    srand(time(NULL));  // Set random seed
    
    // Get inputs from the user
    get_user_inputs();

    list_node_t* head = NULL;

    #ifdef USE_SERIAL
        test_linked_list("Serial", &head, Member,Insert, Delete);
    #elif defined(USE_MUTEX)
        test_linked_list("Mutex", &head, Member,Insert, Delete);
    #elif defined(USE_RWLOCK)
        test_linked_list("Read-Write Lock", &head, Member,Insert, Delete);
    #else
        printf("No linked list type defined! Use -DUSE_SERIAL, -DUSE_MUTEX, or -DUSE_RWLOCK.\n");
        return -1;
    #endif

    return 0;
}
