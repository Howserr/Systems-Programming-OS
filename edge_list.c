#include <edge_list.h>
#include <_null.h>
#include <console.h>

void PrintListY(node *head)
{
    // Loop through the list until NULL, printing the yMin, yMax and x values of each edge
    ConsoleWriteString("List of Y: \n");
    node * current = head;    
    while (current != NULL)
    {
        ConsoleWriteString("yMin = ");
        ConsoleWriteInt(current->edge->yMin, 10);
        ConsoleWriteString("   yMax = ");
        ConsoleWriteInt(current->edge->yMax, 10);
        ConsoleWriteString("   x = ");
        ConsoleWriteInt(current->edge->x, 10);
        ConsoleWriteCharacter('\n');
        current = current->next;
    }
}

void PrintListX(node *head)
{
    // Loop through the list until NULL, printing the x values of each edge
    ConsoleWriteString("List of X: \n");
    node * current = head;    
    while (current != NULL)
    {
        ConsoleWriteString("x = ");
        ConsoleWriteInt(current->edge->x, 10);
        ConsoleWriteCharacter('\n');
        current = current->next;
    }
}

// Loop through the list and count the number of nodes before the end
int GetListSize(node *head)
{
    node *current = head;
    int size = 0;
    while(current != NULL)
    {
        size++;
        current = current->next;
    }
    return size;
}

// Get the node at index from the list
node * GetNodeFromList(node *head, int index)
{
    // List is empty so return NULL
    if (head == NULL)
    {
        return NULL;
    }

    node * current = head;

    for (int i = 0; i < index; i++) 
    {
        if (current->next == NULL)
        {
            // Reached end of the list before index so return NULL
            return NULL;
        }
        // Move to the next node
        current = current->next;
    }
    return current;
}

// Add a node to the end of the list
void AddToList(node **head, node *nodeToAdd)
{
    // List is empty so make node the head
    if (*head == NULL)
    {
        *head = nodeToAdd;
        return;
    }

    node *current = *head;
    
    //Find the end of the list
    while(current->next != NULL)
    {
        current = current->next;
    }

    // set the last node's next to the node we wish to add
    current->next = nodeToAdd;
}

bool RemoveFromList(node **head, edge *edgeToRemove)
{
    // TODO:
    // Check if it's the head, update head to head->next then return true;
    if ((*head)->edge == edgeToRemove)
    {
        node *next = (*head)->next;
        *head = next;
        return true;
    }

    node *current = *head;
    // While through list until current->next == null;
    while (current->next != NULL)
    {
        // if current->next->edge == nodeToRemove
        if (current->next->edge == edgeToRemove)
        {
            // current->next = current->next->next (use tmp)
            current->next = current->next->next;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

void SwapInEdgeList(node *head, int first, int second)
{
    // if head is null then list is empty so we can not swap anything
    if (head == NULL)
    {
        return;
    }

    // Get both nodes from the list
    node *firstNode = GetNodeFromList(head, first);
    node *secondNode = GetNodeFromList(head, second);

    // swap the edge pointers of each node
    edge *tmp = firstNode->edge;
    firstNode->edge = secondNode->edge;
    secondNode->edge = tmp;

    // currently just swap the edges as could not resolve page fault issue when trying update the head
}

// Uses bubble sort to sort the list based on the yMin value
void SortEdgeListByY(node *edgeListHead)
{
    int size = GetListSize(edgeListHead);
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            node *first = GetNodeFromList(edgeListHead, j);
            node *second = GetNodeFromList(edgeListHead, j+1);
            if (first->edge->yMin > second->edge->yMin)
            {
                SwapInEdgeList(edgeListHead, j, j+1);
            }
        }
    }
}

// Uses bubble sort to sort the list based on the x value
void SortEdgeListByX(node *edgeListHead)
{
    int size = GetListSize(edgeListHead);
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            node *first = GetNodeFromList(edgeListHead, j);
            node *second = GetNodeFromList(edgeListHead, j+1);
            if (first->edge->x > second->edge->x)
            {
                SwapInEdgeList(edgeListHead, j, j+1);
            }
        }
    }
}