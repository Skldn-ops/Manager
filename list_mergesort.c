#include "headl.h"
#include <stdio.h>
#include <stdlib.h>

// Функция для разделения списка на две половины
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

// Функция для слияния двух отсортированных списков
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

// Основная функция сортировки слиянием
void mergeSortList(QueueNode** head) {
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }
    
    // Разделяем список на две части
    QueueNode* middle = splitList(*head);
    
    // Рекурсивно сортируем каждую часть
    mergeSortList(head);
    mergeSortList(&middle);
    
    // Сливаем отсортированные части
    *head = mergeSortedLists(*head, middle);
}

// Вставка готового узла в отсортированный список
QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode) {
    // Если список пуст или узел должен быть вставлен в начало
    if (head == NULL || newQueueNode->deploy_tm < head->deploy_tm) {
        newQueueNode->next = head;
        return newQueueNode;  // newQueueNode становится новой головой
    }
    
    QueueNode* current = head;
    
    // Ищем позицию для вставки
    while (current->next != NULL && current->next->deploy_tm < newQueueNode->deploy_tm) {
        current = current->next;
    }
    
    // Вставляем новый узел после current
    newQueueNode->next = current->next;
    current->next = newQueueNode;
    
    return head;  // голова не изменилась
}