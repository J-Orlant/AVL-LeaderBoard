#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char job[6][50] = {
    "Novice", "Knight", "Sniper", "Wizard", "Druid", "Assassin"
};

struct Player
{
    char username[50];
    int job;
    int score;
    int match;
    int win;
    int lose;
    int draw;
    double winrate;

    // default properties linked list
    Player *next, *prev;
};


Player *createPlayer(char username[50], int job, int score, int match, int win, int lose, int draw) {
    Player *newNode = (Player*)malloc(sizeof(Player));

    strcpy(newNode->username, username);
    newNode->job = job;
    newNode->score = score;
    newNode->match = match;
    newNode->win = win;
    newNode->lose = lose;
    newNode->draw = draw;

    newNode->winrate = ((double)win / (double)match) * 100;

    newNode->next = newNode->prev = NULL;

    return newNode;
}

void pushHead(Player **head, Player **tail, Player *newnode) {
    if(!*head) {
        *head = *tail = newnode;
        return;
    }

    (*head)->prev = newnode;
    newnode->next = (*head);
    (*head) = newnode;

}

void pushTail(Player **head, Player **tail, Player *newnode) {
    if(!*head) {
        *head = *tail = newnode;
        return;
    }

    (*tail)->next = newnode;
    newnode->prev = (*tail);
    (*tail) = newnode;

}

void pushSort(Player **head, Player **tail, Player *newnode) {
    if(!*head) {
        *head = *tail = newnode;
        return;
    }

    if(newnode->winrate > (*head)->winrate) {
        // puts("masok");
        pushHead(head, tail, newnode);
        return;
    }

    if(newnode->winrate <= (*tail)->winrate) {
        pushTail(head, tail, newnode);
        return;
    }

    Player *temp = *head;
    while(newnode->winrate <= temp->next->winrate) temp = temp->next;

    newnode->next = temp->next;
    newnode->prev = temp;
    temp->next->prev = newnode;
    temp->next = newnode;
}

void popHead(Player **head, Player **tail) {
    if(!*head) return;

    if(*head == *tail) {
        free(*head);
        *head = *tail = NULL;
        return;
    }

    Player *temp = *head;
    *head = (*head)->next;
    free(temp);
}

void popAll(Player *head, Player *tail) {
    while (head)
    {
        popHead(&head, &tail);
    }    
}

void view(Player **HEAD) {
    // printf("%s", HEAD->username);
    if(!*HEAD) return;
    
    Player *CURR = *HEAD;
    while(CURR) {
        printf("%s - %.2lf\n", CURR->username, CURR->winrate);
        CURR = CURR->next;
    }
}