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


//for debug purposes, prints on a new line
void print(int n)
{
    printf("%d ", n);
}

//add x to the list
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

//prints the list
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

//deletes the node that has the value x
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
    //navigate through the list to find waldo
    Node * nav = head;
    while (nav->next && nav->next->val != x)
    {
        nav = nav->next;
    }
    if(nav->next == NULL || nav->next->val != x)
    {
        return; //element not found  
    }
    //if we got this far we found waldo
    //new pointer to keep the memory address to be freed
    Node * t = nav->next;
    nav->next = nav->next->next;
    free(t);
    return;
}

//delete the whole list starting with n
void flush_list(Node * n)
{
    //recursively free all the elements from the end to the beginning
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

//global barrier for thread sync
pthread_barrier_t barrier;


void * thread1()
{
    pthread_barrier_wait(&barrier);
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
}

void * thread2()
{
    pthread_barrier_wait(&barrier);
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
}

void * thread3()
{
    pthread_barrier_wait(&barrier);
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
}


int main(void)
{
    pthread_t t1, t2, t3;
    //start the threads all at once
    pthread_barrier_init(&barrier, NULL, 3);
    pthread_create(&t1, NULL, &thread1, NULL);
    pthread_create(&t2, NULL, &thread2, NULL);
    pthread_create(&t3, NULL, &thread3, NULL);
    printf("Threads created");

    //wait for the threads to do their thing
    pthread_join(t1, NULL); 
    pthread_join(t2, NULL); 
    pthread_join(t3, NULL);
    printf("\n\nThreads done executing, here's the final list:");
    print_list();
    
    //finally, free the memory allocated for the queue
    flush_list(head);
    printf("\n\nList deleted\n\n");
    return 0;
}
