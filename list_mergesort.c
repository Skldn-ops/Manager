#include "headl.h"
#include <stdio.h>
#include <stdlib.h>

QueueNode* splitList(QueueNode* head) {
    QueueNode* slow = head;
    QueueNode* fast = head->next;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    QueueNode* middle = slow->next;
    slow->next = NULL;
    return middle;
}

QueueNode* mergeSortedLists(QueueNode* a, QueueNode* b) {
    if (a == NULL) return b;
    if (b == NULL) return a;
    
    QueueNode* result = NULL;
    
    if (a->deploy_tm <= b->deploy_tm) {
        result = a;
        result->next = mergeSortedLists(a->next, b);
    } else {
        result = b;
        result->next = mergeSortedLists(a, b->next);
    }
    
    return result;
}

void mergeSortList(QueueNode** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }
    
    QueueNode* middle = splitList(*head);
    
    mergeSortList(head);
    mergeSortList(&middle);
    
    *head = mergeSortedLists(*head, middle);
}

QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode) {
    if (head == NULL || newQueueNode->deploy_tm < head->deploy_tm) {
        newQueueNode->next = head;
        return newQueueNode;
    }
    
    QueueNode* current = head;
    
    while (current->next != NULL && current->next->deploy_tm < newQueueNode->deploy_tm) {
        current = current->next;
    }
    
    newQueueNode->next = current->next;
    current->next = newQueueNode;
    
    return head;
}