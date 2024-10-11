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
int n, m, thread_count, samples, sample_index = 0;
double mMember, mInsert, mDelete;
double* execution_times;

// Function pointers for operations
typedef int (*MemberFn)(int, list_node_t*);
typedef int (*InsertFn)(int, list_node_t**);
typedef int (*DeleteFn)(int, list_node_t**);

typedef struct {
    list_node_t** head_pp;
    int operations_per_thread;
    double mMember;
    double mInsert;
    double mDelete;
    MemberFn member_fn;
    InsertFn insert_fn;
    DeleteFn delete_fn;
} thread_params_t;

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

    printf("Enter the number of samples: ");
    scanf("%d", &samples);
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

void* thread_work(void* arg) {
    thread_params_t* params = (thread_params_t*) arg;
    perform_random_operations(params->head_pp, params->operations_per_thread, params->mMember, params->mInsert, params->mDelete, params->member_fn, params->insert_fn, params->delete_fn);
    pthread_exit(NULL);
}

// General testing function
void test_linked_list(const char* list_name, list_node_t** head_pp, MemberFn member_fn, InsertFn insert_fn, DeleteFn delete_fn) {
    // Populate the list
    populate_list(head_pp, n, insert_fn);

    // Divide the operations among the threads
    int operations_per_thread = m / thread_count;
    int remaining_operations = m % thread_count; // Remaining operations that will be performed by one of the threads

    // Start timer
    clock_t start = clock();

    // Create threads
    pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
    thread_params_t* thread_params = malloc(thread_count * sizeof(thread_params_t));

    for (int i = 0; i < thread_count; i++) {
        thread_params[i].head_pp = head_pp;
        thread_params[i].operations_per_thread = operations_per_thread + (i == 0 ? remaining_operations : 0); // Distribute remaining operations to one thread
        thread_params[i].mMember = mMember;
        thread_params[i].mInsert = mInsert;
        thread_params[i].mDelete = mDelete;
        thread_params[i].member_fn = member_fn;
        thread_params[i].insert_fn = insert_fn;
        thread_params[i].delete_fn = delete_fn;

        pthread_create(&thread_handles[i], NULL, thread_work, (void*) &thread_params[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    // Stop timer
    clock_t end = clock();
    double elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    execution_times[sample_index] = elapsed;
    sample_index ++;

    // Free memory
    free(thread_handles);
    free(thread_params);
}


int main() {
    srand(time(NULL));  // Set random seed
    
    // Get inputs from the user
    get_user_inputs();

    double total_elapsed = 0;

    execution_times = malloc(samples * sizeof(double));

    for (int i = 0; i < samples; i++) {
        list_node_t* head = NULL;

        #ifdef USE_SERIAL
            test_linked_list("Serial", &head, Member, Insert, Delete);
        #elif defined(USE_MUTEX)
            test_linked_list("Mutex", &head, Member, Insert, Delete);
        #elif defined(USE_RWLOCK)
            test_linked_list("Read-Write Lock", &head, Member, Insert, Delete);
        #else
            printf("No linked list type defined! Use -DUSE_SERIAL, -DUSE_MUTEX, or -DUSE_RWLOCK.\n");
            return -1;
        #endif

        printf("Sample %d ==> %.5f seconds\n", i+1, execution_times[i]);
        total_elapsed += execution_times[i];
    }

    printf("================================================\n");
    printf("Average Time ==> %.5f seconds\n", total_elapsed / samples);

    return 0;
}
