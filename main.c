#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP_SIZE 512
#define MAX_INPUT 2

typedef struct carsPark_s {
    int * array;
    int size;
}carsPark;

typedef struct tree_s{
    struct tree_s * right;
    struct tree_s * left;
    carsPark * park;
    int val;
}tree;

typedef struct heapNode_s{
    int value;
    int car;
    int predecessor;
}heapNode;

typedef struct heap_s{
    heapNode *node;
    int size;
}heap;

typedef struct list_s{
    struct list_s * next;
    int val;
}list;

/**
 * Functions used to compute the shortest path
 */
void findPathNeighbours (tree * , heap **, int );
void findPathOthers (tree * , heap **, int *, int );
void carsPath (tree* , heap **, int * , int );
void setRoot (tree*, heap **, int* );
void printPath(heap *);
void carsPathReverse (tree* , heap ** , int * , int );
void setRootReverse(tree* , heap ** , int * );
void findPathOthersReverse (tree * , heap **, int * , int );

/**
 * Data structure support functions
 */
tree* createTreeNode(int, int, int *);
carsPark * createCarsPark();
heap * createHeap();
void insertTreeWithCars(tree** , int, int, int *);
void insertCar(carsPark * , int );
int treeSearchForInsertion(tree* , tree** , int );
tree* treeSearch(tree* , int );
tree* deleteNode(tree* , int );
int deleteCar (carsPark*, int );
void freeCarsPark(carsPark *);
void inOrderTree (tree* );
list * push (list * , int );

/**
 * Utility functions
 */
void aggiungiStazione (tree **, const int * , const char * );
tree* findMin(tree* );
void swap(int *, int *);
void setupNewPath ();
int str_cmp (const char *, const char * );

int stations = 0, predec = -1, endOfPath = 0, pointer = 1,
        endPointer = 0, c = 0, lastPosition = 0;

int main() {

    tree *root = NULL, *closeNode = NULL, *carPosition = NULL;
    heap * heap;
    char *input = NULL, *firstToken = NULL, *token = NULL;
    int arr[MAX_INPUT];
    size_t input_size = 0;

    // Split stdin input into space-separated tokens
    while (getline(&input, &input_size, stdin) != -1){
        heap = NULL;
        firstToken = strtok(input, " ");
        token = strtok(NULL, " ");
        for (int i = 0; i < MAX_INPUT; i++){
            if (token != NULL){
                arr[i] = atoi(token);
                token = strtok(NULL, " ");
            }else{
                arr[i] = -1;
            }
        }

        if (str_cmp(firstToken, "aggiungi-stazione") == 0){

            if(!treeSearchForInsertion(root, &closeNode, arr[0])) { // Locate the insertion point and store it in closeNode
                if (stations == 0){
                    aggiungiStazione(&root, arr, token);
                    closeNode = root;
                }else{
                    aggiungiStazione(&closeNode, arr, token);
                }
                printf("aggiunta\n");
                stations++;
            }else printf("non aggiunta\n");

        }else if(str_cmp(firstToken, "demolisci-stazione") == 0){

            tree * treePosition = treeSearch(root, arr[0]);
            if (treePosition != NULL){
                freeCarsPark(treePosition->park);
                root = deleteNode(root, arr[0]);
                printf("demolita\n");
                stations--;
            }else printf("non demolita\n");

        }else if (str_cmp(firstToken, "aggiungi-auto") == 0) {

            if (carPosition != NULL && carPosition->val == arr[0]){ // Cache the last accessed station to improve runtime performance
                insertCar(carPosition->park, arr[1]);
                printf("aggiunta\n");
            }else{
                tree * treePosition = treeSearch(root, arr[0]); // Store the station node for possible reuse in the next insertion
                if (treePosition != NULL){
                    insertCar(treePosition -> park, arr[1]);
                    printf("aggiunta\n");
                    carPosition = treePosition;
                }else printf("non aggiunta\n");
            }
        }else if (str_cmp(firstToken, "rottama-auto") == 0){

            tree * treePosition = treeSearch(root, arr[0]); // Locate the station node
            if (treePosition != NULL){
                if (deleteCar(treePosition -> park, arr[1]))
                    printf("rottamata\n");
                else printf("non rottamata\n");
            }else printf("non rottamata\n");

        }else{ // add-route
            if (arr[0] < arr[1]){ // Forward direction
                carsPath(root, &heap, &arr[0], arr[1]);
                setupNewPath();
                free(heap -> node);
                free(heap);
            }else if(arr[0] > arr[1]){ // Backward direction
                carsPathReverse(root, &heap, &arr[0], arr[1]);
                setupNewPath();
                c = 0;
                endPointer = 0;
                lastPosition = 0;
                free(heap -> node);
                free(heap);
            }else printf("%d\n", arr[0]); // Start and destination are the same node
        }
    }

    free(input);
    exit(0);
}

void aggiungiStazione (tree ** t, const int * arr, const char * token){

    int * cars, i;

    if (arr[1] != 0){
        cars = malloc(arr[1] * sizeof (int));
        if (cars){
            i = 0;
            while(token != NULL){ // Store car values into the array
                cars[i] = atoi(token);
                token = strtok(NULL, " ");
                i++;
            }
            insertTreeWithCars(t, arr[0], arr[1], cars);
        }else{
            printf("error: addStation\n");
            exit(0);
        }
    }else {
        insertTreeWithCars(t, arr[0], 0, NULL);
    }
}

/**
 * Main shortest-path function for forward traversal
 * */
void carsPath (tree* t, heap ** heap, int * start, int stop) {

    if (t == NULL || endOfPath) {
        return;
    }
    carsPath(t->left, heap, start, stop);
    if (predec == -1 && t->val == *start){ // Starting node
        predec = *start;
        setRoot(t, heap, start);
    }else if(predec != -1 && t -> val <= *start){ // Node directly reachable from the current position
        findPathNeighbours(t, heap, stop);
    }else if (predec != -1 && t -> val > *start){ // Node not directly reachable, try extending through previous nodes
        findPathOthers(t, heap, start, stop);
    }
    carsPath(t->right, heap, start, stop);
}

void findPathNeighbours (tree * t, heap **heap, int stop){

    (*heap) -> node[(*heap) -> size].value = t -> val;
    (*heap) -> node[(*heap) -> size].predecessor = predec;
    (*heap) -> node[(*heap) -> size].car = t ->park ->array[0];
    if (t -> val == stop){
        printPath(*heap);
        endOfPath = 1;
        return;
    }
    (*heap) -> size++;
}

void findPathOthers (tree * t, heap **heap, int * value, int stop){

    (*heap) -> node[(*heap) -> size].value = t -> val;
    predec = (*heap) -> node[pointer].value;
    while ((*heap) -> node[pointer].value < t -> val){
        int path = (*heap) -> node[pointer].value + (*heap) -> node[pointer].car;
        if (path < t -> val){
            pointer++;
            predec = (*heap) -> node[pointer].value;
        }else{
            (*heap) -> node[(*heap) -> size].predecessor = predec;
            (*heap) -> node[(*heap) -> size].car = t ->park ->array[0];
            *value = path;
            if (t -> val == stop){
                printPath(*heap);
                endOfPath = 1;
                return;
            }
            break;
        }
    }
    (*heap) -> size++;
}

void setRoot(tree* t, heap** heap, int * start){

    (*heap) = createHeap();
    (*heap) -> node[0].value = *start;
    (*heap) -> node[0].predecessor = -1;
    (*heap) -> size++;
    predec = (*heap) -> node[0].value;
    if(t -> park -> array[0] != 0)
        *start += t -> park -> array[0];
    else{
        printf("nessun percorso\n");
        endOfPath = 1;
    }
}

/**
 * Main shortest-path function for backward traversal
 * */
void carsPathReverse (tree* t, heap ** heap, int * start, int stop) {

    if (t == NULL || endOfPath) {
        return;
    }

    carsPathReverse(t->right, heap, start, stop);
    if (predec == -1 && t->val == *start){
        predec = *start;
        setRootReverse(t, heap, start);
    }else if(predec != -1 && t -> val >= *start){
        findPathNeighbours(t, heap, stop);
    }else if (predec != -1 && t -> val < *start){
        if (!endOfPath)
            findPathOthersReverse(t, heap, start, stop);
    }
    carsPathReverse(t->left, heap, start, stop);
}

void setRootReverse(tree* t, heap** heap, int * start){

    (*heap) = createHeap();
    (*heap) -> node[0].value = *start;
    (*heap) -> node[0].predecessor = -1;
    (*heap) -> size++;
    predec = (*heap) -> node[0].value;
    if(t -> park -> array[0] != 0)
        *start -= t -> park -> array[0];
    else{
        printf("nessun percorso\n");
        endOfPath = 1;
    }
}

void findPathOthersReverse (tree * t, heap **heap, int * value, int stop){

    if (c == 0){
        endPointer = (*heap) -> size - 1;
        lastPosition = endPointer;
        c =  1;
    }

    (*heap) -> node[(*heap) -> size].value = t -> val;
    (*heap) -> node[(*heap) -> size].car = t -> park -> array[0];

    while (endPointer >= pointer){
        int path = (*heap) -> node[endPointer].value - (*heap) -> node[endPointer].car;
        predec = (*heap) -> node[endPointer].value;
        if (path > t -> val){
            endPointer--;
        }else{
            (*heap) -> node[(*heap) -> size].predecessor = predec;
            *value = path;
            if (t -> val == stop){
                printPath(*heap);
                endOfPath = 1;
                return;
            }
            (*heap) -> size ++;
            return;
        }
    }

    if (lastPosition + 1 == (*heap) -> size){
        printf("nessun percorso\n");
        endOfPath = 1;
        return;
    }else{
        pointer = lastPosition + 1;
        endPointer = (*heap) -> size - 1;
        lastPosition = endPointer;

        int check = 0;
        for (int i = endPointer; i >= pointer && !check; i--){
            int path = (*heap) -> node[i].value - (*heap) -> node[i].car;
            if (path <= t -> val){
                predec = (*heap) -> node[i].value;
                (*heap) -> node[(*heap) -> size].predecessor = predec;
                check = 1;
            }
        }
        if (t -> val == stop){
            printPath(*heap);
            endOfPath = 1;
            return;
        }
        if (check == 0){
            printf("nessun percorso\n");
            endOfPath = 1;
            return;
        }

        (*heap) -> size++;
    }

}

void printPath(heap * heap){

    list * printValue = NULL;
    int predecessor = heap -> node[heap -> size].predecessor;

    for (int i = heap -> size - 1; i > 0; i--){
        if (heap -> node[i].value == predecessor){
            printValue = push (printValue, heap -> node[i].value);
            predecessor = heap ->node[i].predecessor;
        }
    }

    printf ("%d ", heap -> node[0].value);
    while (printValue != NULL){
        printf ("%d ", printValue -> val);
        printValue = printValue -> next;
    }
    printf ("%d\n", heap -> node[heap -> size].value);
    free(printValue);
}

tree* createTreeNode(int key, int carsSize, int * cars) {
    tree* newNode = (tree*)malloc(sizeof(tree));
    if (newNode){
        newNode->val = key;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode -> park = createCarsPark();
        if (cars != NULL){
            for (int i = 0; i < carsSize; i++){
                insertCar(newNode -> park, cars[i]);
            }
        }
        return newNode;
    }else{
        printf("error: tree node\n");
        exit(0);
    }
}

carsPark * createCarsPark() {
    carsPark * heap = (carsPark *)malloc(sizeof(carsPark));
    if (heap){
        heap -> array = (int *) malloc(MAX_HEAP_SIZE * sizeof (int));
        if (heap -> array){
            heap -> size = 0;
            return heap;
        }else{
            printf("error: heap->array\n");
            exit(0);
        }
    }else{
        printf("error: heap\n");
        exit(0);
    }
}

heap *createHeap(){

    heap * h = (heap *)malloc(sizeof(heap));
    if (h){
        h -> node = (heapNode *) malloc(stations *  sizeof (heapNode));
        if (h -> node){
            h -> size = 0;
            return h;
        }else{
            printf("error: heap->array\n");
            exit(0);
        }
    }else{
        printf("error: heap\n");
        exit(0);
    }
}

void insertTreeWithCars(tree** t, int num, int carsSize, int * cars) {

    tree* newNode = createTreeNode(num, carsSize, cars);

    if (*t == NULL)
        *t = newNode;
    else if (num < (*t)->val)
        (*t)->left = newNode;
    else
        (*t)->right = newNode;
}

int treeSearchForInsertion(tree* t, tree ** closeNode, int key){

    int check = 0;
    tree * curr = t;

    while (curr != NULL && !check){
        if (key < curr -> val){
            *closeNode = curr;
            curr = curr -> left;
        } else if (key > curr -> val){
            *closeNode = curr;
            curr = curr -> right;
        } else {
            check = 1;
        }
    }

    return check;
}

tree* treeSearch(tree* t, int key){

    tree * curr = t;

    while (curr != NULL){
        if (key < curr -> val){
            curr = curr -> left;
        } else if (key > curr -> val){
            curr = curr -> right;
        } else {
            return curr;
        }
    }

    return NULL;
}

void insertCar(carsPark * heap, int value) {

    heap -> array [heap -> size] = value;
    if(heap -> array [0] < value){
        swap(&heap -> array [0], &heap -> array[heap -> size]);
    }
    heap -> size++;

}

int deleteCar(carsPark * heap, int value){

    for (int i = 0; i < heap -> size; i++){
        if (heap -> array[i] == value){
            heap -> size--;
            swap(&heap -> array[i], &heap -> array[heap -> size]);
            heap -> array[heap -> size] = 0;
            return 1;
        }
    }
    return 0;
}

void inOrderTree (tree* t) {
    if (t == NULL)
        return;
    inOrderTree(t->left);
    printf("%d ", t->val);
    inOrderTree(t->right);
}

tree* deleteNode(tree* t, int data) {

    if (t == NULL)
        return t;

    if (data < t->val) {
        t->left = deleteNode(t->left, data);
    } else if (data > t->val) {
        t->right = deleteNode(t->right, data);
    } else {
        // Node has at most one child
        if (t->left == NULL) {
            tree *temp = t->right;
            free(t);
            return temp;
        } else if (t->right == NULL) {
            tree *temp = t->left;
            free(t);
            return temp;
        }

        // Node has two children
        tree *temp = findMin(t->right);
        t->val = temp->val;
        t->park = temp->park;
        t->right = deleteNode(t->right, temp->val);

    }
    return t;
}

void freeCarsPark(carsPark * park){
    free(park->array);
    free(park);
}

list * push (list * h, int val){

    list * tmp = (list *)malloc (sizeof (list ));
    if (tmp){
        tmp -> val = val;
        tmp -> next = h;
        h = tmp;
    }else{
        printf ("error: push\n ");
        exit(0);
    }
    return h;

}


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

tree* findMin(tree* t) {
    if (t == NULL) return NULL;
    while (t->left != NULL) {
        t = t->left;
    }
    return t;
}

void setupNewPath (){
    if (!endOfPath)
        printf("nessun percorso\n");
    else endOfPath = 0;
    pointer = 1;
    predec = -1;
}

int str_cmp (const char *s1, const char * s2){
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}