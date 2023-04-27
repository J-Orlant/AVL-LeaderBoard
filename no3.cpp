#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "playerLinkedList.cpp"

char actionList[7][50] {
    "INSERT", "FIND", "DELETE", "WORST", "BEST", "CLOSTEST", "SHOWALL"
};

int findAction(char *action) {
    for (int i = 0; i < 7; i++)
    {
        if(strcmp(action, actionList[i]) == 0) return i;
    }
    
    return -1;
}

struct AVL
{
    int score;
    Player *Head;
    Player *Tail;

    // default properties avl
    int height;
    AVL *left, *right;
};

AVL *ROOT = NULL;

int getHeight(AVL* node) {
    if(!node) return 0;

    return node->height;
}

int getMax(int left, int right) {
    return (left > right) ? left : right;
}

int calculateHeight(AVL* node) {
    return getMax(getHeight(node->left), getHeight(node->right)) + 1;
}

int calculateBalanceFactor(AVL *node) {
    if(!node) return 0;

    return getHeight(node->left) - getHeight(node->right);
}

AVL *createNodeAVL(Player* player) {
    AVL *newnode = (AVL*)malloc(sizeof(AVL));

    newnode->score = player->score;
    newnode->Head = newnode->Tail = player;

    newnode->height = 0;
    newnode->left = newnode->right = NULL;

    return newnode;
}

AVL *leftRotate(AVL *node) {
    AVL *pivot = node->right;
    AVL *cut = pivot->left;

    pivot->left = node;
    node->right = cut;

    node->height = calculateHeight(node);
    pivot->height = calculateHeight(pivot);

    return pivot;
}

AVL *rightRotate(AVL *node) {
    AVL *pivot = node->left;
    AVL *cut = pivot->right;

    pivot->right = node;
    node->left = cut;

    node->height = calculateHeight(node);
    pivot->height = calculateHeight(pivot);

    return pivot;
}

AVL *insertNode(AVL *curr, Player *newPlayer) {
    if(!curr) {
        AVL *newnode = createNodeAVL(newPlayer);
        newnode->height = calculateHeight(newnode);
        return newnode;
    }

    if(curr->score > newPlayer->score) curr->left = insertNode(curr->left, newPlayer);
    if(curr->score < newPlayer->score) curr->right = insertNode(curr->right, newPlayer);

    if(curr->score == newPlayer->score) {
        pushSort(&curr->Head, &curr->Tail, newPlayer);
    }

    curr->height = calculateHeight(curr);
    int balance = calculateBalanceFactor(curr);

    if(balance > 1 && newPlayer->score < curr->left->score) {
        curr = rightRotate(curr);
    }
    
    else if(balance > 1 && newPlayer->score > curr->left->score) {
        curr->left = leftRotate(curr->left);
        curr = rightRotate(curr);
    }

    else if(balance < -1 && newPlayer->score > curr->right->score) {
        curr = leftRotate(curr);
    }

    else if(balance < -1 && newPlayer->score < curr->right->score) {
        curr->right = rightRotate(curr->right);
        curr = leftRotate(curr);
    }

    return curr;
}

AVL* getPredes(AVL* Curr) {
    if(!Curr) return NULL;

    AVL* replace = Curr->left;
    while(replace->right) replace = replace->right;

    return replace;
}

AVL *deleteNode(AVL *curr, int score) {
    if(!curr) {
        printf(" DELETE: %d not found.\n", score);
        return NULL;
    }

    if(curr->score > score) curr->left = deleteNode(curr->left, score);
    if(curr->score < score) curr->right = deleteNode(curr->right, score);

    if(curr->score == score) {
        if(!curr->left && !curr->right) {
            free(curr);
            return NULL;
        }

        if(!curr->left) {
            AVL *replace = curr->right;
            free(curr);

            return replace;
        }

        if(!curr->right) {
            AVL *replace = curr->left;
            free(curr);

            return replace;
        }

        AVL *replace = getPredes(curr);
        popAll(curr->Head, curr->Tail);
        curr->score = replace->score;
        curr->Head = replace->Head;
        curr->Tail = replace->Tail;

        curr->left = deleteNode(curr->left, replace->score);
    }

    curr->height = calculateHeight(curr);
    int balance = calculateBalanceFactor(curr);

    if(balance > 1 && score < curr->left->score) {
        curr = rightRotate(curr);
    }
    
    else if(balance > 1 && score > curr->left->score) {
        curr->left = leftRotate(curr->left);
        curr = rightRotate(curr);
    }

    else if(balance < -1 && score > curr->right->score) {
        curr = leftRotate(curr);
    }

    else if(balance < -1 && score < curr->right->score) {
        curr->right = rightRotate(curr->right);
        curr = leftRotate(curr);
    }

    return curr;
}

void insert() {
    char username[50];
    int job;
    int score;
    int match;
    int win;
    int lose;
    int draw;

    scanf("%[^#]#%d#%d#%d#%d#%d#%d",
        username, &job, &score, &match,
        &win, &lose, &draw
    );

    ROOT = insertNode(ROOT, createPlayer(username, job, score, match, win, lose, draw));
}

void showAll(AVL *curr) {
    if(!curr) return;

    showAll(curr->left);
    printf("Score %d (%d)\n", curr->score, curr->height);
    showPlayer(curr->Head);
    puts("");
    showAll(curr->right);
}

AVL* find(AVL *curr, int value) {
    if(!curr) {
        printf(" FIND: %d NOT FOUND\n", value);
        return NULL;
    }

    if(curr->score > value) curr->left = find(curr->left, value);
    if(curr->score < value) curr->right = find(curr->right, value);

    if(curr->score == value) {
        Player* temp = curr->Head;
        printf(" Score %d\n", curr->score);
        showPlayer(curr->Head);
    }
}

AVL* worst(AVL *curr) {
    if(!curr->left) {
        printf("Score %d\n", curr->score);
        printf("WORST: %s\t [%10s] ( %.2lf%%)\n",
            curr->Tail->username, job[curr->Tail->job], curr->Tail->winrate
        );

        return NULL;
    }

    worst(curr->left);
}

AVL* best(AVL *curr) {
    if(!curr->right) {
        printf("Score %d\n", curr->score);
        printf("BEST: %s\t\t [%10s] ( %.2lf%%)\n",
            curr->Head->username, job[curr->Head->job], curr->Head->winrate
        );

        return NULL;
    }

    best(curr->right);
}

AVL* closest(AVL *curr, int value, int oldDiff, AVL *keep) {
    if(!curr) {
        Player* temp = keep->Head;
        printf(" Score %d\n", keep->score);
        showPlayer(keep->Head);

        return NULL;
    }

    if(curr->score == value) {
        Player* temp = curr->Head;
        printf(" Score %d\n", curr->score);
        showPlayer(curr->Head);

        return NULL;
    }
    
    // diff = selisih

    int newDiff = curr->score - value;
    if(newDiff < 0) newDiff *= -1;

    if(oldDiff != -1) {
        if(newDiff < oldDiff) {
            keep = curr;
        } else {
            newDiff = oldDiff;
        }
    } else  {
        oldDiff = newDiff;
    } 

    if(curr->score > value) curr->left = closest(curr->left, value, newDiff, keep);
    if(curr->score < value) curr->right = closest(curr->right, value, newDiff, keep);
}

void menu(int index) {
    int value;
    switch (index)
    {
    case 0:
        insert();
        break;
    case 1:
        scanf("%d", &value);
        find(ROOT, value);
        break;
    case 2:
        scanf("%d", &value);
        ROOT = deleteNode(ROOT, value);
        break;
    case 3:
        worst(ROOT);
        break;
    case 4:
        best(ROOT);
        break;
    case 5:
        scanf("%d", &value);
        closest(ROOT, value, -1, ROOT);
        break;
    case 6:
        showAll(ROOT);
        break;
    default:
        break;
    }
}

int main() {
    int n;
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        char command[50];
        
        command:
            scanf("%s", command); getchar();
            if(findAction(command) == -1) goto command;
        
        menu(findAction(command));
    }
}