//build with -pthread
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

typedef struct node
{
    int val;
    struct node * next;
    void (*callback_print)(int);
}Node;

Node* head = NULL;

void print(int);
void add(int);
void print_list();
void delete(int);
void flush_list(Node*);
void swap(Node*, Node*);
void sort_list();

void* thread1();
void* thread2();
void* thread3();


void print(int n)
{
    printf("%d ", n);
}

void add(int x)
{
    Node* n = malloc(sizeof(Node));

    n->val = x;
    n->callback_print = &print;
    n->next = NULL;

    if(head == NULL)
    {
        head = n;
        return;
    }

    //we need a pointer to navigate through the list
    Node * nav = head;
    while(nav->next)
    {
        nav = nav->next;
    }
    nav->next = n;
    return;
}

void print_list()
{
    Node * nav = head;
    printf("\n");
    while(nav)
    {
        nav->callback_print(nav->val);
        nav = nav->next;
    }
}



void delete(int x)
{
    if(head == NULL)
        return; //the list is empty, nothing to do
    //check if the value is in the first element of the list
    if(head->val == x)
    {
        if(head->next != NULL)
        {
            Node * t = head;
            head = head->next;
            free(t);
            return;
        }
        else
        {
            free(head);
            head = NULL;
            return;
        }
        
    }
    
    Node * nav = head;
    while (nav->next && nav->next->val != x)
    {
        nav = nav->next;
    }
        if(nav->next == NULL || nav->next->val != x)
            return; //element not found
            
    //new pointer to keep the memory address to be freed
    Node * t = nav->next;
    nav->next = nav->next->next;
    free(t);
    return;
}

void flush_list(Node * n)
{
    if(n->next)
        flush_list(n->next);
    Node * t = n;
    n = NULL;
    free(t);
}

void swap(Node * a, Node * b)
{
    //we don't actually need to swap the whole nodes, just the data
    int temp = a->val;
    a->val = b->val;
    b->val = temp;
}

void sort_list()
{
    //bubble sort
    Node * thead = head; //temporary head for navigation
    Node * nav;          //compare index
    while(thead != NULL)
    {
        nav = thead->next;
        while(nav != NULL)
        {
            if(thead->val > nav->val)
            {
                swap(thead, nav);
            }
            nav = nav->next;
        }
        thead = thead->next;
    }
}

//global variables for thread sync
uint8_t threadsReady = 0;
uint8_t readyThread1 = 0, readyThread2 = 0, readyThread3 = 0;

void * thread1()
{
    readyThread1 = 1;
    while(threadsReady == 0);
    printf("\nThread %lu STARTED", pthread_self());
    printf("\nThread %lu: add(2)", pthread_self());
    add(2);
    printf("\nThread %lu: add(4)", pthread_self());
    add(4);
    printf("\nThread %lu: delete(2)", pthread_self());
    delete(2);
    printf("\nThread %lu: sort_list()", pthread_self());
    sort_list();
    printf("\nThread %lu: delete(10)", pthread_self());
    delete(10);
    printf("\nThread %lu: delete(5)", pthread_self());
    delete(5);
    printf("\nThread %lu FINISHED", pthread_self());
    readyThread1 = 1;
}

void * thread2()
{
    readyThread2 = 1;
    while(threadsReady == 0);
    printf("\nThread %lu STARTED", pthread_self());
    printf("\nThread %lu: add(11)", pthread_self());
    add(11);
    printf("\nThread %lu: add(1)", pthread_self());
    add(1);
    printf("\nThread %lu: delete(11)", pthread_self());
    delete(11);
    printf("\nThread %lu: add(8)", pthread_self());
    add(8);
    printf("\nThread %lu: print_list()", pthread_self());
    print_list();
    printf("\nThread %lu FINISHED", pthread_self());
    readyThread2 = 1;
}

void * thread3()
{
    readyThread3 = 1;
    while(threadsReady == 0);
    printf("\nThread %lu STARTED", pthread_self());
    printf("\nThread %lu: add(30)", pthread_self());
    add(30);
    printf("\nThread %lu: add(25)", pthread_self());
    add(25);
    printf("\nThread %lu: add(100)", pthread_self());
    add(100);
    printf("\nThread %lu: sort_list()", pthread_self());
    sort_list();
    printf("\nThread %lu: print_list()", pthread_self());
    print_list();
    printf("\nThread %lu: delete(100)", pthread_self());
    delete(100);
    printf("\nThread %lu FINISHED", pthread_self());
    readyThread3 = 1;
}

int main(void)
{
    pthread_t t1, t2, t3;
    threadsReady = 0;
    pthread_create(&t1, NULL, &thread1, NULL);
    pthread_create(&t2, NULL, &thread2, NULL);
    pthread_create(&t3, NULL, &thread3, NULL);
    printf("Threads created, waiting for initialization...");

    //wait until the threads are loaded, then start them
    while(!readyThread1 || !readyThread2 || !readyThread3)
    {
        ;
    }
    printf("done.\n");

    //start the threads
    threadsReady = 1;

    //wait for the threads to do their thing
    pthread_join(t1, NULL); 
    pthread_join(t2, NULL); 
    pthread_join(t3, NULL);
    printf("\n\nThreads done executing, here's the final list:");
    print_list();
    
    //finally, free the memory allocated for the queue
    flush_list(head);
    printf("\n\n");
    return 0;
}
