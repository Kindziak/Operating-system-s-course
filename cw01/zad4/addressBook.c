#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "addressBook.h"

// -----------------------------------------------
// ------- funkcje do listy dwukierunkowej -------

// tworzy liste (ksiazke adresowa)
List * createList(){
  List * aList = (List *) malloc(sizeof(List));
  aList -> head = NULL;
  aList -> tail = NULL;
  return aList;
}

// usuwa liste (ksiazke adresowa)
void deleteList(List* aList){
  if (aList == NULL){
    free(aList);
  }
  ListNode * iter = aList -> head;
  while (iter != NULL){
    ListNode * tmp = iter -> next;
    deleteListNode(aList,iter);
    iter = tmp;
  }
  free(aList);
}

// tworzy nowy wezel *nowy kontakt)
ListNode * makeListNode(char* newName ,char* newSurname ,char* newBirthDate ,char* newEmail ,char* newPhoneNumber ,char* newAddress){
  ListNode * aNode = (ListNode*)malloc(sizeof(ListNode));
  aNode -> prev = NULL;
  aNode -> next = NULL;
  aNode -> data.name = newName;
  aNode -> data.surname = newSurname;
  aNode -> data.birthDate = newBirthDate;
  aNode -> data.email = newEmail;
  aNode -> data.phoneNumber = newPhoneNumber;
  aNode -> data.address = newAddress;
  return aNode;
}

// dodaje wezel do listy na poczatek (nowy kontakt)
void addListNodeToBeggining(List* aList ,ListNode* aNode){
  if(aList -> head != NULL){
    aList -> head -> prev = aNode;
    aNode -> next = aList -> head;
    aList -> head = aNode;
  }
  else{
    aList -> head = aNode;
    aList -> tail = aNode;
  }
}

// dodaje wezel do listy na koniec (nowy kontakt)
void addListNodeToEnd(List* aList ,ListNode* aNode){
  if(aList -> head != NULL){
    aList -> tail -> next = aNode;
    aNode -> prev = aList -> tail;
    aList -> tail = aNode;
  }
  else{
    aList -> head = aNode;
    aList -> tail = aNode;
  }
}

// uwalnia pamiec wezla
void releaseListNode(ListNode * aNode){
  /*if (aNode -> data.name) free(aNode.data.name);
  if (aNode -> data.surname) free(aNode -> data.surname);
  if (aNode -> data.birthDate) free(aNode -> data.birthDate);
  if (aNode -> data.email) free(aNode -> data.email);
  if (aNode -> data.phoneNumber) free(aNode -> data.phoneNumber);
  if (aNode -> data.address) free(aNode -> data.address);
  */free(aNode);
}

// usuwa wezel z listy (usuwa kontakt)
void deleteListNode(List* aList ,ListNode* aNode){
  if (aList -> head == NULL || aList -> tail == NULL){
    printf("Ksiazka adresowa jest pusta.");
  }
  if (aNode == NULL){
    printf("Nie ma takiego kontaktu.");
  }
  if (aNode == aList -> tail && aNode == aList -> head){
    aList -> head = NULL;
    aList -> tail = NULL;
    releaseListNode(aNode);
  } else if (aNode == aList -> head){
    aList -> head = aList -> head -> next;
    aList -> head -> prev = NULL;
    releaseListNode(aNode);
  } else if (aNode == aList -> tail){
    aList -> tail = aList -> tail -> prev;
    aList -> tail -> next = NULL;
    releaseListNode(aNode);
  } else {
    ListNode * aPrev = aNode -> prev;
    ListNode * aNext = aNode -> next;
    aPrev -> next = aNext;
    aNext -> prev = aPrev;
    releaseListNode(aNode);
  }
}

// szuka wezla w liscie (szuka kontaktu)
ListNode * findListNode(List* aList ,char* aName, char* aSurname){
    ListNode * iter = aList -> head;
    while (iter != NULL && strcmp(aName,iter -> data.name) != 0 && strcmp(aSurname,iter -> data.surname) != 0){
      iter = iter -> next;
    }
    return iter;
}

// sortuje liste wedlug wybranego pola w contact
List * sortList(List* aList ,int aType){
  if (aList == NULL || (aList -> head == NULL && aList -> tail == NULL)){
    printf("Ksiazka adresowa jest pusta.");
    return NULL;
  }
  if (aList -> head -> next == NULL && aList -> tail -> prev == NULL){
    return aList;
  }

  List * newList = createList();
  ListNode * iter;
  ListNode * chosenNode;
  while (aList -> head != NULL){
    iter = aList -> head;
    chosenNode = aList -> head;
      switch (aType){
        case 1:
          while(iter != NULL){
            if(strcmp(iter -> data.surname, chosenNode -> data.surname) < 0){
              chosenNode = iter;
            }
            iter = iter -> next;
          }
          break;
        case 2:
          while(iter != NULL){
            if(strcmp(iter -> data.birthDate, chosenNode -> data.birthDate) < 0){
              chosenNode = iter;
            }
            iter = iter -> next;
          }
          break;
        case 3:
          while(iter != NULL){
            if(strcmp(iter -> data.email, chosenNode -> data.email) < 0){
              chosenNode = iter;
            }
            iter = iter -> next;
          }
          break;
        case 4:
          while(iter != NULL){
            if(strcmp(iter -> data.phoneNumber, chosenNode -> data.phoneNumber) < 0){
              chosenNode = iter;
            }
            iter = iter -> next;
          }
          break;
        default:
          printf("Nie mozna posortowac wzgledem tego.");
          break;
      }
    ListNode * aNode = makeListNode(chosenNode -> data.name, chosenNode -> data.surname, chosenNode -> data.birthDate, chosenNode -> data.email, chosenNode -> data.phoneNumber, chosenNode -> data.address);
    addListNodeToEnd(newList, aNode);
    deleteListNode(aList,chosenNode);
  }
  //deleteList(aList); //!!!!!!!!! Segmantation Fault
  return newList;
}

// wypisuje liste (ksiazke adresowa)
void printList(List * aList, int beginningORend){
  if(aList -> head == NULL || aList -> tail == NULL){
    printf("Ksiazka adresowa jest pusta.");
  }
  else{
    ListNode * iter;
    switch(beginningORend){
      case 1:
        iter = aList -> head;
        while (iter != NULL){
          printf("%s %s ->", iter -> data.name, iter -> data.surname);
          iter = iter -> next;
        }
        break;
      case 2:
        iter = aList -> tail;
        while (iter != NULL){
          printf("%s %s ->", iter -> data.name, iter -> data.surname);
          iter = iter -> prev;
        }
        break;
      default:
        printf("Nie ma takiej opcji wyswietlenia.");
        break;
    }
  }
}

// -------------------------------------------
// ------- funkcje do drzewa binarnego -------

// tworzy drzewo (ksiazke adresowa)
Tree * createTree(){
  Tree * aTree = (Tree*)malloc(sizeof(Tree));
  aTree -> root = NULL;
  return aTree;
}

// usuwa drzewo (ksiazke adresowa)
void deleteTree(Tree* aTree){
  if(aTree -> root != NULL) deleteAllNodes(aTree -> root);
  aTree -> root = NULL;
  free(aTree);
}

//usuwa poddrzewa
void deleteAllNodes(TreeNode * aNode){
  if(aNode != NULL){
      deleteAllNodes(aNode -> right);
      deleteAllNodes(aNode -> left);
      releaseTreeNode(aNode);
      aNode = NULL;
  }
}

// tworzy nowy wezel (nowy kontakt)
TreeNode * makeTreeNode(char* newName ,char* newSurname ,char* newBirthDate ,char* newEmail ,char* newPhoneNumber ,char* newAddress){
  TreeNode * aNode = (TreeNode*)malloc(sizeof(TreeNode));
  aNode -> right = NULL;
  aNode -> left = NULL;
  aNode -> parent = NULL;
  aNode -> data.name = newName;
  aNode -> data.surname = newSurname;
  aNode -> data.birthDate = newBirthDate;
  aNode -> data.email = newEmail;
  aNode -> data.phoneNumber = newPhoneNumber;
  aNode -> data.address = newAddress;
  return aNode;
}

// dodaje wezel do drzewa (nowy kontakt)
void addTreeNode(Tree* aTree ,TreeNode* aNode){
  TreeNode * iter = aTree -> root;
  if(iter == NULL){
    aTree -> root = (TreeNode *)malloc(sizeof(TreeNode));
    aTree -> root = aNode;
  }
  else{
    TreeNode * aParent = NULL;
    while(iter != NULL){
      aParent = iter;
      if(strcmp(aNode -> data.surname,iter -> data.surname) > 0) iter = iter -> right;
      else iter = iter -> left;
    }
    aNode -> parent = aParent;
    if(strcmp(aNode -> data.surname,aParent -> data.surname) > 0) aParent -> right = aNode;
    else aParent -> left = aNode;
  }
}

// dodaje wezel do drzewa (nowy kontakt)
void addTreeNodeByName(Tree* aTree ,TreeNode* aNode){
  TreeNode * iter = aTree -> root;
  if(iter == NULL){
    aTree -> root = (TreeNode *)malloc(sizeof(TreeNode));
    aTree -> root = aNode;
  }
  else{
    TreeNode * aParent = NULL;
    while(iter != NULL){
      aParent = iter;
      if(strcmp(aNode -> data.name,iter -> data.name) > 0) iter = iter -> right;
      else iter = iter -> left;
    }
    aNode -> parent = aParent;
    if(strcmp(aNode -> data.name,aParent -> data.name) > 0) aParent -> right = aNode;
    else aParent -> left = aNode;
  }
}

//uwalnia pamiec wezla
void releaseTreeNode(TreeNode * aNode){
  /*if (aNode -> data.name) free(aNode -> data.name);
  if (aNode -> data.surname) free(aNode -> data.surname);
  if (aNode -> data.birthDate) free(aNode -> data.birthDate);
  if (aNode -> data.email) free(aNode -> data.email);
  if (aNode -> data.phoneNumber) free(aNode -> data.phoneNumber);
  if (aNode -> data.address) free(aNode -> data.address);
  */free(aNode);
}

// usuwa wezel z drzewa (usuwa kontakt)
void deleteTreeNode(Tree* aTree ,TreeNode* aNode){
  if (aTree -> root == NULL) printf("Ksiazka adresowa jest pusta.");
  else if (aNode == NULL) printf("Nie ma wezla.");
  else if (aNode -> right == NULL && aNode -> left == NULL){
    if (aNode -> parent == NULL) aTree -> root = NULL;
    else if (aNode -> parent -> left == aNode) aNode -> parent -> left = NULL;
    else aNode -> parent -> right = NULL;
    releaseTreeNode(aNode);
  }
  else if (aNode -> right == NULL || aNode -> left == NULL){
    if (aNode -> left == NULL){
      if (aNode -> parent == NULL) aTree -> root = aNode -> right;
      else if (aNode -> parent -> left == aNode) aNode -> parent -> left = aNode -> right;
      else aNode -> parent -> right = aNode -> right;
    }
    else{
      if (aNode -> parent == NULL) aTree -> root = aNode -> left;
      else if (aNode -> parent -> left == aNode) aNode -> parent -> left = aNode -> left;
      else aNode -> parent -> right = aNode -> left;
    }
    releaseTreeNode(aNode);
  }
  else{
    TreeNode * tmp = minNode(aNode -> right);
    aNode -> data = tmp -> data;
    deleteTreeNode(aTree,tmp);
  }
}

// minimalny wezel w drzewie/poddrzewie
TreeNode * minNode(TreeNode * aNode){
  while(aNode -> left != NULL) aNode = aNode -> left;
  return aNode;
}

// szuka wezla w drzewie (szuka kontaktu)
TreeNode * findTreeNode(Tree* aTree ,char* aSurname){
  TreeNode * iter = aTree -> root;
  while (iter != NULL){
    if(strcmp(aSurname,iter -> data.surname) == 0) return iter;
    else{
      if(strcmp(aSurname,iter -> data.surname) < 0) iter = iter -> left;
      else iter = iter -> right;
    }
  }
  return iter;

  /*
  if (aRoot -> right == NULL || aRoot -> left == NULL) return NULL;
  if (strcmp(aSurname,aRoot -> data.surname) == 0) return aRoot;
  else if (strcmp(aSurname,aRoot -> data.surname) < 0) findTreeNode(aRoot -> left, aSurname);
  else if (strcmp(aSurname,aRoot -> data.surname) > 0) findTreeNode(aRoot -> right, aSurname);
  return NULL;
  */

}

// sortuje drzewo wedlug wybranego pola w contact - poprawic
Tree * sortTree(Tree* aTree){
  if(aTree == NULL || aTree -> root == NULL){
    printf("Ksiazka adresowa jest pusta.");
    return NULL;
  }
  if(aTree -> root -> left == NULL && aTree -> root -> right == NULL){
    return aTree;
  }
  Tree * newTree = createTree();
  ListNode * chosenNode = NULL;
  List * aList = createList();
  treeToList(aTree -> root,aList);
  while(aList -> head != NULL){
    chosenNode = aList -> head;
    TreeNode * aNode = makeTreeNode(chosenNode -> data.name, chosenNode -> data.surname, chosenNode -> data.birthDate, chosenNode -> data.email, chosenNode -> data.phoneNumber, chosenNode -> data.address);
    addTreeNodeByName(newTree, aNode);
    deleteListNode(aList,chosenNode);
  }
  deleteList(aList);
  return newTree;
}

// robi z drzewa listę
void treeToList(TreeNode * aNode,List * aList){
  if(aNode != NULL){
    treeToList(aNode -> left,aList);
    ListNode * newListNode = makeListNode(aNode -> data.name, aNode -> data.surname, aNode -> data.birthDate, aNode -> data.email, aNode -> data.phoneNumber, aNode -> data.address);
    addListNodeToEnd(aList,newListNode);
    treeToList(aNode -> right,aList);
  }
}

// wypisuje drzewo (ksiazke adresowa)
void printTree(TreeNode * aNode){
  if(aNode != NULL){
    printTree(aNode -> left);
    printf("%s %s -> ",aNode -> data.name, aNode -> data.surname);
    printTree(aNode -> right);
  }
}
