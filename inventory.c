#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Product {
    int id;
    char name[50];
    int quantity;
    float price;
    struct Product *left, *right;
};

struct Transaction {
    int productId;
    char type[20];
    int quantityChanged;
    struct Transaction *next;
};

struct Product *root = NULL;
struct Transaction *historyHead = NULL;

struct Product* createProduct(int id, char name[], int quantity, float price) {
    struct Product* newNode = (struct Product*)malloc(sizeof(struct Product));
    newNode->id = id;
    strcpy(newNode->name, name);
    newNode->quantity = quantity;
    newNode->price = price;
    newNode->left = newNode->right = NULL;
    return newNode;
}

struct Product* insertProduct(struct Product* root, int id, char name[], int quantity, float price) {
    if (root == NULL)
        return createProduct(id, name, quantity, price);

    if (id < root->id)
        root->left = insertProduct(root->left, id, name, quantity, price);
    else if (id > root->id)
        root->right = insertProduct(root->right, id, name, quantity, price);
    else
        printf("Product with ID %d already exists!\n", id);

    return root;
}

struct Product* searchProduct(struct Product* root, int id) {
    if (root == NULL || root->id == id)
        return root;
    if (id < root->id)
        return searchProduct(root->left, id);
    else
        return searchProduct(root->right, id);
}

void recordTransaction(int productId, char type[], int qty) {
    struct Transaction* newTrans = (struct Transaction*)malloc(sizeof(struct Transaction));
    newTrans->productId = productId;
    strcpy(newTrans->type, type);
    newTrans->quantityChanged = qty;
    newTrans->next = NULL;

    if (historyHead == NULL) {
        historyHead = newTrans;
    } else {
        struct Transaction* temp = historyHead;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newTrans;
    }
}

void updateStock(int id, int change, char type[]) {
    struct Product* p = searchProduct(root, id);

    if (p == NULL) {
        printf("Product not found!\n");
        return;
    }

    if (strcmp(type, "Sale") == 0) {
        int soldQty = -change;
        if (p->quantity < soldQty) {
            printf("Not enough stock! Available: %d\n", p->quantity);
            return;
        }
    }

    p->quantity += change;
    recordTransaction(id, type, change);
    printf("Stock updated successfully! Current stock: %d\n", p->quantity);
}

void displayProducts(struct Product* root) {
    if (root == NULL) return;
    displayProducts(root->left);
    printf("ID: %d | Name: %s | Quantity: %d | Price: %.2f\n",
           root->id, root->name, root->quantity, root->price);
    displayProducts(root->right);
}

void showHistory() {
    if (historyHead == NULL) {
        printf("No transactions yet!\n");
        return;
    }

    struct Transaction* temp = historyHead;
    printf("\n--- Transaction History ---\n");

    while (temp != NULL) {
        printf("Product ID: %d | %s | Quantity: %d\n",
               temp->productId, temp->type, temp->quantityChanged);
        temp = temp->next;
    }
}

struct Product* findMin(struct Product* node) {
    while (node->left != NULL)
        node = node->left;
    return node;
}

struct Product* deleteProduct(struct Product* root, int id) {
    if (root == NULL)
        return NULL;

    if (id < root->id)
        root->left = deleteProduct(root->left, id);
    else if (id > root->id)
        root->right = deleteProduct(root->right, id);
    else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        else if (root->left == NULL) {
            struct Product* temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL) {
            struct Product* temp = root->left;
            free(root);
            return temp;
        }

        struct Product* temp = findMin(root->right);
        root->id = temp->id;
        strcpy(root->name, temp->name);
        root->quantity = temp->quantity;
        root->price = temp->price;
        root->right = deleteProduct(root->right, temp->id);
    }
    return root;
}

int main() {
    int choice, id, qty;
    char name[50];
    float price;

    while (1) {
        printf("\n===== Inventory Management System =====\n");
        printf("1. Add New Product\n");
        printf("2. Search Product\n");
        printf("3. Purchase Stock\n");
        printf("4. Sell Product\n");
        printf("5. Display All Products\n");
        printf("6. View Transaction History\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Product ID: ");
                scanf("%d", &id);
                printf("Enter Product Name: ");
                scanf(" %[^\n]", name);
                printf("Enter Quantity: ");
                scanf("%d", &qty);
                printf("Enter Price: ");
                scanf("%f", &price);
                root = insertProduct(root, id, name, qty, price);
                break;

            case 2: {
                printf("Enter Product ID to search: ");
                scanf("%d", &id);
                struct Product* found = searchProduct(root, id);
                if (found)
                    printf("ID: %d | Name: %s | Quantity: %d | Price: %.2f\n",
                           found->id, found->name, found->quantity, found->price);
                else
                    printf("Product not found!\n");
                break;
            }

            case 3:
                printf("Enter Product ID: ");
                scanf("%d", &id);
                printf("Enter Quantity Purchased: ");
                scanf("%d", &qty);
                updateStock(id, qty, "Purchase");
                break;

            case 4:
                printf("Enter Product ID: ");
                scanf("%d", &id);
                printf("Enter Quantity Sold: ");
                scanf("%d", &qty);
                updateStock(id, -qty, "Sale");
                break;

            case 5:
                displayProducts(root);
                break;

            case 6:
                showHistory();
                break;

            case 7:
                printf("Enter Product ID to delete: ");
                scanf("%d", &id);
                root = deleteProduct(root, id);
                printf("Product deleted (if it existed).\n");
                break;

            case 8:
                printf("Exiting...\n");
                exit(0);

            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}
