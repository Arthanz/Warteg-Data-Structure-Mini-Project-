// By Arya Nathan Bara - 2602064222
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#define maxBuckets 999

void logoWarteg(){
    FILE *fpLW = fopen("WartegLogo.txt", "r");
    char logo;
    while(!feof(fpLW)){
        logo=getc(fpLW);
		printf("%c", logo);
    }
    fclose(fpLW);
}

void logoExit(){
    FILE *fpLE = fopen("ExitLogo.txt", "r");
    char logo;
    while(!feof(fpLE)){
        logo=getc(fpLE);
		printf("%c", logo);
    }
    fclose(fpLE);
}

struct Dish{
    char dish[255];
    long long int prc, quantity;
    struct Dish *next, *prev;
}*head=NULL, *tail=NULL;

struct Dish *createDish(char dish[], long long int prc, long long int quantity){
    struct Dish *newDish = (struct Dish*)malloc(sizeof(struct Dish));
    strcpy(newDish->dish,dish);
    newDish->prc=prc;
    newDish->quantity=quantity;
    newDish->next = NULL;
    newDish->prev = NULL;
    return newDish;
}

int menuCounter=0;

void insertLL(char dish[], long long int prc, long long int quantity){
    struct Dish *newDish = createDish(dish, prc, quantity);
    if(!head) head=tail=newDish;
    else if(head->quantity<=newDish->quantity){
        newDish->next=head;
        head->prev=newDish;
        head=newDish;
    }
    else if(tail->quantity>=newDish->quantity){
        tail->next=newDish;
        newDish->prev=tail;
        tail=newDish;
    }
    else{
        struct Dish *temp = head;
        while(temp->next->quantity>=newDish->quantity){
            temp=temp->next;
        }
        newDish->next=temp->next;
        newDish->prev=temp;
        temp->next->prev=newDish;
        temp->next=newDish;
    }
    menuCounter++;

    // Update data in file
    FILE *fp = fopen("menu.txt", "w");
    struct Dish *curr = head;
    while(curr){
        fprintf(fp, "%s#%lld#%lld\n", curr->dish, curr->prc, curr->quantity);
        curr = curr->next;
    }
    fclose(fp);
}

void removeLL(char dish[]){
    int cek = 1;
    if(!head){
        puts("There are no dishes!");
        return;
    }
    else if(strcmp(head->dish,dish)==0){
        struct Dish *newHead = head->next;
        head->next=NULL;
        free(head);
        head = newHead;
        puts("The dish has been removed!");
    }
    else if(strcmp(tail->dish, dish)==0){
        struct Dish *newTail = tail->prev;
        tail->prev=NULL;
        free(tail);
        tail = newTail;
        puts("The dish has been removed!");
    }
    else{
        struct Dish *temp = head;
        while(strcmp(temp->dish, dish)!=0){
            if(temp==tail && strcmp(temp->dish, dish)!=0){
                cek = -1;
                break;
            }
            temp = temp->next;
        }
        if(cek!=1){
            puts("The dish does not exist!");
        }
        else{
            temp->next->prev=temp->prev;
            temp->prev->next=temp->next;
            temp->next = temp->prev = NULL;
            free(temp);
            puts("The dish has been removed!");
        }
    }

    // Update data in file
    FILE *fp = fopen("menu.txt", "w");
    struct Dish *curr = head;
    while(curr){
        fprintf(fp, "%s#%lld#%lld\n", curr->dish, curr->prc, curr->quantity);
        curr = curr->next;
    }
    fclose(fp);
}

void readFromFileAndPushToList(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    char dishName[100];
    long long int price, quantity;
    while (fscanf(fp, "%[^#]#%lld#%lld\n", dishName, &price, &quantity) != EOF) {
        insertLL(dishName, price, quantity);
    }

    fclose(fp);
}

char foundDish[255];
long long int foundPrc=0, foundQty=0;

int searchMenu(char dish[]){
    if(!head){
        return -1;
    }
    else{
        struct Dish *curr = head;
        while(curr && strcmp(curr->dish,dish)!=0){
            if(curr==tail&&strcmp(curr->dish,dish)!=0){
                return -2;
            }
            curr=curr->next;
        }
        strcpy(foundDish,curr->dish);
        foundPrc = curr->prc;
        foundQty = curr->quantity;
        return 1;
    }
}

void printLL(){
    if(!head){
        puts("There are no dishes!");
        return;
    }
    else{
        int i=0;
        struct Dish *curr = head;
        puts(" =================================================");
        puts("|                   Arya's Menu                   |");
        puts(" =================================================");
        puts("| No. | Name           | Quantity     | Price     |");
        puts(" =================================================");
        while(curr){
            printf("| %d.  | %-13s  | %03lld          | Rp%-7lld |\n", i+1, curr->dish, curr->quantity, curr->prc);
            i++;
            curr=curr->next;
        }
        puts(" =================================================");
    }
}

void viewMenu(){
    system("cls");
    if(!head){
        puts("There are no dishes!\nPress enter to continue...");
        getch();
        return;
    }
    else{
        int i=0;
        struct Dish *curr = head;
        puts(" =================================================");
        puts("|                   Arya's Menu                   |");
        puts(" =================================================");
        puts("| No. | Name           | Quantity     | Price     |");
        puts(" =================================================");
        while(curr){
            printf("| %d.  | %-13s  | %03lld          | Rp%-7lld |\n", i+1, curr->dish, curr->quantity, curr->prc);
            i++;
            curr=curr->next;
        }
        puts(" =================================================");
        printf("Press enter to continue...");
        getch();
    }
}

struct Customers{
    char name[255];
    struct Dish *foodOrder;
};

struct Customers *table[maxBuckets];

struct Customers *createCust(char name[]){
   struct  Customers *newCust = (struct Customers*)malloc(sizeof(struct Customers));
    strcpy(newCust->name, name);
    newCust->foodOrder=NULL;
    return newCust;
}

unsigned long djb2(char name[]){
    unsigned long key = 5381;
    int i;
    for(i=0; name[i]!='\0'; i++){
        int ascii = (int)(name[i]);
        key = (key << 5) + key + ascii;
    }
    return key%maxBuckets;
}
int custCounter=0;

void linearProbing(int idx, char name[]){
    struct Customers *newCust = createCust(name);
    int  i;
    for(i=(idx+1)%maxBuckets; i!=idx; i=(i+1)%maxBuckets){
        if(!table[i]){
            table[i]=newCust;
            custCounter++;
            return;
        }
    }
}

void insertCust(char name[]){
    struct Customers *newCust = createCust(name);
    int idx = djb2(name);
    if(!table[idx]){
        table[idx]=newCust;
        custCounter++;
    }
    else{
        linearProbing(idx, name);
    }
}

int searchCust(char name[]){
    int idx = djb2(name);
    
    if(!table[idx]){
        return -1;
    }
    else {
        if(strcmp(table[idx]->name, name)==0){
            return idx;
        }
        else {
            int i;
            for(i=(idx+1)%maxBuckets; i!=idx; i=(i+1)%maxBuckets){
                if(strcmp(table[i]->name, name)==0){
                idx=i;
                break;
                }
                else if(i==idx-1 && strcmp(table[idx]->name,name)!=0){
                    return -1;
                }
            }
            return idx;
        }
    }
}

void *insertOrder(int idx, char name[], long long int prc, long long int qty){
   struct  Dish  *newOrder = createDish(name, prc, qty);
    if(!table[idx]->foodOrder){
        table[idx]->foodOrder=newOrder;
    }
    else{
        struct Dish *curr = table[idx]->foodOrder;
        while(curr->next){
            curr=curr->next;
        }
        curr->next=newOrder;
        newOrder->prev=curr;
    }
}

void removeCust(int idx){
    if (table[idx]->foodOrder != NULL) {
        free(table[idx]->foodOrder);
    }
    free(table[idx]);
    table[idx] = NULL;
}

void removeOrder(int idx){
    struct Dish *curr = table[idx]->foodOrder;
    while(curr){
        struct Dish *temp = curr;
        curr = curr->next;
        free(temp);
    }
    table[idx]->foodOrder = NULL;
}

void cls(){
    system("cls");
}

void addDish(){
    cls();
    int cek = 1;
    char dish[255];
    long long int price, quantity, len=0;
    do{
        cek=1;
        int trueCounter=0, falseCounter=0;
        printf("Insert the name of the dish [Lowercase letters]: ");
        scanf("%[^\n]", dish);
        getchar();
        len=strlen(dish);
        int i;
        for(i=0; i<len; i++){
            if(dish[i]>='a'&&dish[i]<='z'){ 
                trueCounter++;
            }
            else if(dish[i]==' '){
                trueCounter++;
            }
            else{
                falseCounter++;
            }
        }
        if(falseCounter!=0) cek=-1;
    }while(cek==-1);
    
    do{
        printf("Insert the price of the dish [1000..50000]: ");
        scanf("%lld", &price);
        getchar();
    }while(price<1000||price>50000);

    do{
        printf("Insert the quantity of the dish [1..999]: ");
        scanf("%lld", &quantity);
        getchar();
    }while(quantity<1||quantity>999);

    insertLL(dish, price, quantity);
    puts("The dish has been added!");
    printf("Press enter to continue...");
    getchar();
}

void removeDish(){
    char rmDish[255];
    int len=0, trueCounter=0, falseCounter=0;
    int cek=1;
    cls();
    printLL();
    puts("");
    do{
        cek = 1;
        trueCounter=0;
        falseCounter=0;
        printf("Insert dish's name to be deleted [Lowercase letters]: ");
        scanf("%[^\n]", rmDish);
        getchar();
        len = strlen(rmDish);
        int  i;
        for(i=0; i<len; i++){
            if(rmDish[i]>='a'&&rmDish[i]<='z'){ 
                trueCounter++;
            }
            else if(rmDish[i]==' '){
                trueCounter++;
            }
            else{
                falseCounter++;
            }
        }
        if(falseCounter>0) cek= -1;
    }while(cek==-1);
    removeLL(rmDish);
    printf("Press enter to continue...");
    getchar();
}

void addCustomers(){
    char name[255];
    int len=0;
    int cek=1;
    do{
        int trueCounter=0, falseCounter=0;
        cls();
        cek=1;
        printf("Insert the customer's name [Without space]: ");
        scanf("%[^\n]", name);
        getchar();
        len=strlen(name);
        int i;
        for(i=0; i<len; i++){
            if(name[i]>='a'&&name[i]<='z'){
                trueCounter++;
            }
            else if(name[i]>='A'&&name[i]<='Z'){
                trueCounter++;
            }
            else{
                falseCounter++;
            }
        }
        if(falseCounter>0) cek=-1;
    }while(cek==-1);
    insertCust(name);
    puts("Customer has been added!");
    printf("Press enter to continue...");
    getchar();
}

void searchCostumer(){
    char srcName[255];
    int cek=1;
    int len=0;
    do{
        cls();
        int trueCounter=0, falseCounter=0;
        cek=1;
        printf("Insert the customer's name to be searched [Without space]: ");
        scanf("%s", srcName);
        getchar();
        len=strlen(srcName);
        int  i;
        for(i=0; i<len; i++){
            if(srcName[i]>='a'&&srcName[i]<='z'){
                trueCounter++;
            }
            else if(srcName[i]>='A'&&srcName[i]<='Z'){
                trueCounter++;
            }
            else{
                falseCounter++;
            }
        }
        if(falseCounter>0) cek=-1;
    }while(cek==-1);
    int srcResult = searchCust(srcName);
    if(srcResult==-1) printf("%s is not present.\n", srcName);
    else printf("%s's customer number is %d\n", srcName, srcResult+1); 
    printf("Press enter to continue...");
    getchar();
}

void viewWarteg(){
    system("cls");
    puts("Customer's List");
    int  i;
    for(i=0; i<999; i++){
        if(custCounter==0){
            puts("Warteg don't have any customer!");
            puts("Please add the customer first");
            break;
        }
        if(table[i]){
            printf("%d. %s\n", i+1, table[i]->name);
        }
    }
    printf("Press enter to continue...");
    getchar();
}

void order(){
    system("cls");
    char name[255], dish[255]="", temp[255];
    int manyOrder, qty;
    int cek=1;
    int srcCustResult = 0;
    if(!head) {
        puts("The customer can't order because the menu is empty!");
        puts("Please add the dish first");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    else if(custCounter==0){
        puts("Warteg don't have any customer!");
        puts("Please add the customer first");
        printf("Press enter to continue...");
        getchar();
        return;
    }
    do{
        int trueCounter=0, falseCounter=0, len=0;
        cek=1;
        printf("Insert the customer's name: ");
        scanf("%[^\n]", name);
        getchar();
        len=strlen(name);
        int i;
        for(i=0; i<len; i++){
            if(name[i]>='a'&&name[i]<='z'){
                trueCounter++;
            }
            else if(name[i]>='A'&&name[i]<='Z'){
                trueCounter++;
            }
            else{
                falseCounter++;
            }
        }
        if(falseCounter>0){
            printf("Name only consist of lower and uppercase letters");
            getchar();
            cek=-1;
        }
        srcCustResult = searchCust(name);
        if(srcCustResult==-1){
            printf("Customer is not present");
            getchar();
            cek=-1;
        }
    }while(cek==-1);

    do{
        printf("Insert the amount of dish: ");
        scanf("%d", &manyOrder);
        getchar();
        if(manyOrder>menuCounter){
            printf("Menu available only %d types\n", menuCounter);
            puts("Please input below or equal to the number of the menu");
            printf("Press enter to continue..."); getchar();
        }
    }while(manyOrder>menuCounter);
    int i;
    for(i=1; i<=manyOrder; i++){
        do{
            cek=1;
            printf("[%d] ", i);
            scanf("%[^x] x%d", &temp, &qty);
            getchar();
            int len = strlen(temp);
            if(temp[len-1]==' '){
                strncpy(dish, temp, len-1);
            }

            int srcResult = searchMenu(dish);
            if(srcResult==-2){
                puts("This menu is not exist!");
                cek=-1;
                printf("Press enter to continue...");
                getchar();
                puts("");
            }
            if(qty>foundQty){
                puts("Sorry, our quantity of this food is not enough for your order");
                printf("Press enter to continue...");
                getchar();
                puts("");
            }
        }while(cek==-1||qty>foundQty);
        insertOrder(srcCustResult, dish, foundPrc*qty, qty);
    }
    
    puts("Order success!");
    printf("Press enter to continue...");
    getchar();
}

void payment(){
    int idx;
    printf("Insert the customer's index: ");
    scanf("%d", &idx);
    getchar();
    if(!table[idx-1]){
        printf("The %d customer's index is not exist!\n");
        printf("Press enter to continue...");
        getchar();
    }
    else{
        long long int sumPrc=0;
        int i=1;
        printf("%s\n", table[idx-1]->name);
        struct Dish *curr = table[idx-1]->foodOrder;
        while(curr){
            printf("[%d] %s x%d\n", i, curr->dish, curr->quantity);
            sumPrc+=curr->prc;
            curr=curr->next;
            i++;
        }
        printf("Total: Rp%lld\n", sumPrc);
        printf("Press enter to continue...");
        getchar();
        removeOrder(idx-1);
        removeCust(idx-1);
    }
}

void Exit(){
    cls();
    logoExit();
    getchar();
    exit(0);
}

void mainMenu(){
    int inputMainMenu;
    while(1){
        cls();
        logoWarteg();
        puts("");
        puts("1. Add Dish");
        puts("2. Remove Dish");
        puts("3. View Menu");
        puts("4. Add Customer");
        puts("5. Search Customer");
        puts("6. View Warteg");
        puts("7. Order");
        puts("8. Payment");
        puts("9. Exit Warteg");
        do{
            printf(">> ");
            scanf("%d", &inputMainMenu);
            getchar();
        }while(inputMainMenu<1||inputMainMenu>9);
        switch(inputMainMenu){
            case 1:
                addDish();
                break;
            case 2:
                removeDish();
                break;
            case 3:
                viewMenu();
                break;
            case 4:
                addCustomers();
                break;
            case 5:
                searchCostumer();
                break;
            case 6:
                viewWarteg();
                break;
            case 7:
                order();
                break;
            case 8:
                payment();
                break;
            case 9:
                Exit();
                break;
        }
    };
}

int main(){
    readFromFileAndPushToList("menu.txt");
    mainMenu();
    return 0;
}