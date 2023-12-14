#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_CUSTOMERS 5 // number of customers
#define NUM_TRANSACTION 6 // number of transactions/threads
#define INIT_STOCK 100 // the initial value for each person stock

// type transaction. you should pass an object of this type to the function arguments while creating thread
typedef struct
{
    double amount;
    int customer_id;
} transaction_t;

double safeBox = 1000.0; // shared variable for all customers

///////// Start your code here ///////////

double customerStock[NUM_CUSTOMERS]; // array to store the stock of all customers
pthread_mutex_t customerMutex[NUM_CUSTOMERS]; // mutexes for each customer account
pthread_mutex_t safeBoxMutex = PTHREAD_MUTEX_INITIALIZER; // mutex for the safe box

void* withdraw(void* arg) {
    transaction_t* transaction = (transaction_t*)arg;
    int customerID = transaction->customer_id;

    pthread_mutex_lock(&customerMutex[customerID]);
    if (customerStock[customerID] >= transaction->amount) {
        customerStock[customerID] -= transaction->amount;
        printf("Customer %d withdrew %.2f. New balance: %.2f\t ------------ > safe-box value: %.2f\n",
           customerID, transaction->amount, customerStock[customerID], safeBox);
        pthread_mutex_unlock(&customerMutex[customerID]);
    } else {
        double amountTakenFromBalance = customerStock[customerID];
        customerStock[customerID] -= transaction->amount;
        pthread_mutex_lock(&safeBoxMutex);
        safeBox += customerStock[customerID];
        if(amountTakenFromBalance < 0)
        {
            printf("Customer %d withdrew 0 from account balance and %.2f from safe-box . New balance: %.2f\t ------------ > safe-box value: %.2f\n",
               customerID,(-1 *customerStock[customerID]),customerStock[customerID], safeBox);
        }
        else
        {
            printf("Customer %d withdrew %.2f from account balance and %.2f from safe-box means %.2f  from both . New balance: %.2f\t ------------ > safe-box value: %.2f\n",
               customerID,amountTakenFromBalance,(-1 *customerStock[customerID]),transaction->amount,customerStock[customerID], safeBox);
        }
        pthread_mutex_unlock(&safeBoxMutex);
        pthread_mutex_unlock(&customerMutex[customerID]);
        return NULL;
    }

    
    
    
    
    return NULL;
}

void* deposit(void* arg) {
    transaction_t* transaction = (transaction_t*)arg;
    int customerID = transaction->customer_id;
    
    pthread_mutex_lock(&customerMutex[customerID]);
    if(customerStock[customerID] < 0)
    {
        if(transaction->amount > (customerStock[customerID] * -1))
        {
            pthread_mutex_lock(&safeBoxMutex);
            double amountGiven = transaction->amount + customerStock[customerID];
            safeBox += (customerStock[customerID] * -1);
            customerStock[customerID] += amountGiven ;
            printf("Customer %d stock was charged by %.2f. New balance: %.2f\t ------------ > safe-box value: %.2f\n",
                customerID, transaction->amount, customerStock[customerID], safeBox);
            pthread_mutex_unlock(&safeBoxMutex);
            pthread_mutex_unlock(&customerMutex[customerID]);
        }
        else
        {   
            pthread_mutex_lock(&safeBoxMutex);
            safeBox += transaction->amount ;
            customerStock[customerID] += transaction->amount;
            printf("Customer %d stock was charged by %.2f. New balance: %.2f\t ------------ > safe-box value: %.2f\n",
                customerID, transaction->amount, customerStock[customerID], safeBox);
            pthread_mutex_unlock(&safeBoxMutex);
            pthread_mutex_unlock(&customerMutex[customerID]);
        }
    }
    else
    {
        customerStock[customerID] += transaction->amount;
        printf("Customer %d stock was charged by %.2f. New balance: %.2f\t ------------ > safe-box value: %.2f\n",
            customerID, transaction->amount, customerStock[customerID], safeBox);
        pthread_mutex_unlock(&customerMutex[customerID]);
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    // Initialize customer stocks and mutexes
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customerStock[i] = INIT_STOCK;
        pthread_mutex_init(&customerMutex[i], NULL);
    }

    // Create threads for transactions
    pthread_t threads[NUM_TRANSACTION];

    for (int i = 0; i < NUM_TRANSACTION; ++i) {
        transaction_t* transaction = (transaction_t*)malloc(sizeof(transaction_t));
        transaction->amount = rand() % 500 + 1; // Random amount between 1 and 50
        transaction->customer_id = rand() % NUM_CUSTOMERS;

        if (rand() % 2 == 0) {
            pthread_create(&threads[i], NULL, withdraw, (void*)transaction);
        } else {
            pthread_create(&threads[i], NULL, deposit, (void*)transaction);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_TRANSACTION; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutexes
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        pthread_mutex_destroy(&customerMutex[i]);
    }

    return 0;
}
