//Author: Deni Rakovic
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// SYMBOLIC CONSTANTS
#define  MAX_LENGTH 20  // of SKU name
#define  LINEAR      1
#define  FORM        0
#define  LIST        2 //Format for list items function
#define  MAX_ITEM_NO 21
#define  MAX_QTY     999
#define  SKU_MAX     999
#define  SKU_MIN     100
//#define  MAX_ITEM_NO 500
#define  DATAFILE    "inventory.dat"

// global - struct Item
struct Item
{
    double price;
    int    sku;
    int    isTaxed;
    int    quantity;
    int    minQuantity;
    char   name[MAX_LENGTH + 1]; // 1 for delimiter
};

// Prototypes
void   printWelcome(void);
void   printGoodbye(void);
void   printTitle(void);
void   printFooter(double gTotal);
void   clearKeyboard(void);
void   pause(void);
int    getInt(void);
double getDouble(void);
int    getIntLimited(int lowerLimit, int upperLimit);
double getDoubleLimited(double lowerLimit, double upperLimit);
bool   getYesorNo(void);
int    getMenuChoice(void);
void   runInventorySystem(void);
double totalAfterTax(struct Item Item);
bool isQuantityLow(struct Item *Item);
struct Item enterItem(int sku);
void displayItem(struct Item *I, int format);
void listItems(struct Item inventory[], int noOfItems);
bool locateItem(struct Item item[], int noOfRecs, int sku, int *index);
void updateItem(struct Item* itemptr);
void search(const struct Item item[], int NoOfRecs);
int loadItems(struct Item* item[], char fileName[], int* NoOfRecsPtr);

// global - TAX rate
const double TAX = 0.13;

int main(void)
{
    printWelcome();
    runInventorySystem();
    printGoodbye();

}

int loadItems(struct Item* item[], char fileName[], int* NoOfRecsPtr)
{
    //Open file
    FILE *fp;
    fp = fopen(fileName, "r");
    //Error handling
    if (fp == NULL)
    {
        printf("Cannot open <%s>\n", fileName);
        return 0;
    }
    //vars for reading item data
    int sku, qty, minQty, isTaxed, lines = 0;
    double price;
    char itemName[MAX_LENGTH + 1];

    while (fscanf(fp, "%d,%[^,],%lf,%d,%d,%d", &sku, itemName, &price, &isTaxed, &qty, &minQty) != EOF)
    {
        int x = 0;
        //Count lines and set struct members
        lines++;
        item[x]->sku = sku;
        strcpy(itemName, item[x]->name);
        item[x]->price = price;
        item[x]->isTaxed = isTaxed;
        item[x]->quantity = qty;
        item[x]->minQuantity = minQty;
        x++;
    }
    *NoOfRecsPtr = lines;

    return 1;
}

bool locateItem(struct Item item[], int noOfRecs, int sku, int *index)
{
    int counter;
    bool located = false;
    for (counter = 0; counter < noOfRecs; counter++)
    {

        if (item[counter].sku == sku)
        {
            *index = counter;
            located = true;
        }

    }

    return located;
}

void listItems(struct Item inventory[], int noOfItems)
{
    int counter = 0, row = 0;
    double totalPrice = 0;
    printTitle();
    for (counter = 0; counter < noOfItems; counter++)
    {
        row++; //Increment row counter by 1
        totalPrice += totalAfterTax(inventory[counter]);
        printf("%-4d", row); //Print row number;
        displayItem(&inventory[counter], LIST);//Print item details
    }

    printFooter(totalPrice);
    return;
}

struct Item enterItem(int sku)
{
    struct Item newItem;
    //Set & print new item's sku
    newItem.sku = sku;
    printf("        SKU: %d\n", newItem.sku);
    //Get name
    printf("       Name: ");
    scanf("%20[^\n]", newItem.name);
    clearKeyboard();
    //Get Price
    printf("      Price: ");
    newItem.price = getDoubleLimited(0, 100000000);
    //Get Qty
    printf("   Quantity: ");
    newItem.quantity = getIntLimited(0, 100000000);
    //Get MinQty
    printf("Minimum Qty: ");
    newItem.minQuantity = getIntLimited(1, 100000000);
    //Is taxed
    printf("   Is Taxed: ");
    bool isTaxed = getYesorNo();
    newItem.isTaxed = isTaxed;

    return newItem;
}

bool isQuantityLow(struct Item *Item)
{
    bool isLow = Item->quantity < Item->minQuantity;
    return isLow;
}

double totalAfterTax(struct Item Item)
{
    double total;
    //Is the item taxed?
    //Yes
    if (Item.isTaxed == 1)
    {
        total = ((Item.price * Item.quantity) * TAX) + Item.price * Item.quantity;
    }
        //No
    else if (Item.isTaxed == 0)
    {
        total = Item.price * Item.quantity;
    }

    return total;
}

void displayItem(struct Item *I, int format)
{
    bool isTax = I->isTaxed == 1, isLow = isQuantityLow(I);
    double total = totalAfterTax(*I);
    char taxString[4] = "No";
    //Set taxString if tax is applicable
    if (isTax == true)
        strcpy("Yes", taxString);

    //Linear format
    if (format == FORM)
    {
        printf("        SKU: %d\n", I->sku);
        printf("       Name: %s\n", I->name);
        printf("      Price: %.2lf\n", I->price);
        printf("   Quantity: %d\n", I->quantity);
        printf("Minimum Qty: %d\n", I->minQuantity);
        printf("   Is Taxed: %s\n", taxString);
        if (isLow == true)
            printf("WARNING: Quantity low, please order ASAP!!!\n");
    }

    //Form format
    if (format == LINEAR)
    {
        //If quantity is low print asterisk
        if (isLow == true)
        {
            printf("|%d|%-20s|    %.2lf|  %s|   %d |   %d |       %.2lf|***\n", I->sku, I->name, I->price, taxString, I->quantity, I->minQuantity, total);
        }
        else if (isLow == false)
        {
            printf("|%d|%-20s|    %.2lf|  %s|  %d |   %d |      %.2lf|\n", I->sku, I->name, I->price, taxString, I->quantity, I->minQuantity, total);

        }

    }
    //Format for listing items
    if (format == LIST)
    {
        //If quantity is low print asterisk
        if (isLow == true)
        {
            printf("|%-3d|%-20s|%8.2lf|  %s|%4d |%4d |%12.2lf|***\n", I->sku, I->name, I->price, taxString, I->quantity, I->minQuantity, total);

        }
        else if (isLow == false)
        {
            printf("|%-3d|%-20s|%8.2lf|   %s|%4d |%4d |%12.2lf|\n", I->sku, I->name, I->price, taxString, I->quantity, I->minQuantity, total);
        }

    }
}
void printGoodbye(void)
{
    printf("Thank you for using Deni's Grocery Inventory System\n");
}
void runInventorySystem(void)
{
    struct Item inventory[21];
    int itemsLoaded = 0;
    int menuChoice;
    bool yesNo;
    printf("Loading Inventory Data...\n");
    loadItems(inventory, DATAFILE, itemsLoaded);
    pause();
    do
    {
        //Print menu and get users choice
        menuChoice = getMenuChoice();
        switch (menuChoice)
        {
            case 0:
                printf("Exit the program? (Y)es/(N)o: ");
                yesNo = getYesorNo();
                if (yesNo == true)
                    break;
                if (yesNo == false)
                    menuChoice = 2;
                break;

            case 1:
                //listItems();
                pause();
                break;

            case 2:
                //search();
                pause();
                break;

            case 3:
                printf("NOT IMPLEMENTED\n");
                pause();
                break;

            case 4:
                printf("NOT IMPLEMENTED\n");
                pause();
                break;

            case 5:
                printf("Please enter the SKU: ");
                //getintlimited
                //locate item, .....?
                pause();
                break;

            case 6:
                printf("NOT IMPLEMENTED\n");
                pause();
                break;

            case 7:
                printf("NOT IMPLEMENTED\n");
                pause();
                break;
        }

    } while (menuChoice > 0 && menuChoice <= 7);

}

bool getYesorNo(void)
{
    char CH;
    do
    {
        scanf("%c", &CH);
        clearKeyboard();
        switch (CH)
        {

            case 'y':
                return true;
                break;

            case 'Y':
                return true;
                break;

            case 'n':
                return false;
                break;

            case 'N':
                return false;
                break;

            default:
                printf("Only (Y)es or (N)o are acceptable: ");
                scanf("%c", &CH);
                break;

        }
    } while (CH != 'y' && CH != 'Y' && CH != 'n' && CH != 'N');

    return false;


}

int getMenuChoice(void)
{
    int x;
    printf("1- List all items\n");
    printf("2- Search by SKU\n");
    printf("3- Checkout an item\n");
    printf("4- Stock an item\n");
    printf("5- Add new item or update item\n");
    printf("6- Delete item\n");
    printf("7- Search by name\n");
    printf("0- Exit program\n");
    printf("> ");

    x = getIntLimited(0, 7);
    return x;
}

int getIntLimited(int lowerLimit, int upperLimit)
{
    int x;
    do
    {
        x = getInt();
        if (x == 0)
        {
            return 0;
        }
        if (x < lowerLimit || x > upperLimit)
        {
            printf("Invalid value, %d <= value <= %d: ", lowerLimit, upperLimit);
        }
    } while (x < lowerLimit || x > upperLimit);
    return x;
}

void printWelcome(void)
{
    printf("---=== Deni's Grocery Inventory System ===---\n\n");
}


void printTitle(void)
{
    printf("Row |SKU| Name               | Price  |Taxed| Qty | Min |   Total    |Atn\n");
    printf("----+---+--------------------+--------+-----+-----+-----+------------|---\n");

}

void printFooter(double gTotal)
{
    printf("--------------------------------------------------------+----------------\n");
    if (gTotal > 0)
    {
        printf("                                           Grand Total: |     %.2lf\n", gTotal);
    }
}

void clearKeyboard(void)
{
    char ch;
    //Gets single char until newline
    while ((ch = getchar()) != '\n');
}

int getInt(void)
{
    int x;
    char y = 'w';

    while (y != '\n')
    {
        scanf("%d%c", &x, &y);
        if (y != '\n')
        {
            clearKeyboard();
            printf("Invalid integer, please try again: ");
        }
    }

    return x;
}

void pause(void)
{
    printf("Press <ENTER> to continue...");
    clearKeyboard();
}

double getDouble(void)
{
    double x;
    char y = 'w';

    while (y != '\n')
    {
        scanf("%lf%c", &x, &y);
        if (y != '\n')
        {
            clearKeyboard();
            printf("Invalid number, please try again: ");
        }
    }

    return x;
}

double getDoubleLimited(double lowerLimit, double upperLimit)
{
    double x;
    do
    {
        x = getDouble();
        if (x < lowerLimit || x > upperLimit)
        {
            printf("Invalid value, %lf <= value <= %lf: ", lowerLimit, upperLimit);
        }
    } while (x < lowerLimit || x > upperLimit);
    return x;

}

void search(const struct Item item[], int NoOfRecs)
{
    int sku, index;
    // Get sku from user
    printf("Please enter the SKU: ");
    sku = getIntLimited(SKU_MIN, SKU_MAX);
    //Call locateItem
    bool isitemFound = locateItem(item, NoOfRecs, sku, &index);
    //Check return
    if (isitemFound == true)
    {
        displayItem(&item[index], FORM);
    }
    else if (isitemFound == false)
    {
        printf("Item is not found!\n");
    }
    else
    {
        printf("Error searching for item\n");
    }
}

void updateItem(struct Item* itemptr)
{
    struct Item item;
    printf("Enter new data:\n");
    //Prompt user for new data
    item = enterItem(itemptr->sku);
    //Confirm if user wants to overwrite data
    printf("Overwrite old data? (Y)es or (N)o: ");
    bool yesOrNo = getYesorNo();
    if (yesOrNo == true)
    {
        *itemptr = item;
    }
    else if (yesOrNo == false)
    {
        printf("--== Aborted ==--\n");
    }
    else
    {
        printf("Error updating item \n");
    }
}
