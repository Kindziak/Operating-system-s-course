#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

// zdefiniowanie struktur
typedef struct contact contact;
typedef struct ListNode ListNode;
typedef struct List List;
typedef struct TreeNode TreeNode;
typedef struct Tree Tree;

// informacje o kontakcie
struct contact {
  char * name;
  char * surname;
  char * birthDate;
  char * email;
  char * phoneNumber;
  char * address;
};

// wezel listy
struct ListNode {
  ListNode * prev;
  ListNode * next;
  contact data;
};

// lista dwukierunkowa
struct List {
  ListNode * head;
  ListNode * tail;
};

// wezel drzewa binarnego
struct TreeNode {
  TreeNode * right;
  TreeNode * left;
  TreeNode * parent;
  contact data;
};

// drzewo binarne
struct Tree {
  TreeNode * root;
};

// ------- funkcje do listy dwukierunkowej -------

struct List * createList(); // tworzy liste (ksiazke adresowa)
void deleteList(List*); // usuwa liste (ksiazke adresowa)
ListNode * makeListNode(char* ,char* ,char* ,char* ,char* ,char*); //tworzy nowy wezel (nowy kontakt)
void addListNodeToBeginning(List* ,ListNode*); // dodaje wezel do listy na poczatek
void addListNodeToEnd(List* ,ListNode*); // dodaje wezel do listy na koniec
void releaseListNode(ListNode*); // uwalnia pamiec wezla
void deleteListNode(List* ,ListNode*); // usuwa wezel z listy (usuwa kontakt)
ListNode * findListNode(List* ,char* ,char*); // szuka wezla w liscie (szuka kontaktu)
List * sortList(List* ,int); // sortuje liste wedlug wybranego pola w contact
void printList(List* ,int); // wypisuje liste (ksiazke adresowa)

// ------- funkcje do drzewa binarnego -------
Tree * createTree(); // tworzy drzewo (ksiazke adresowa)
void deleteTree(Tree*); // usuwa drzewo (ksiazke adresowa)
void deleteAllNodes(TreeNode *); // usuwa poddrzewa
TreeNode * makeTreeNode(char* ,char* ,char* ,char* ,char* ,char*); // tworzy nowy wezel (nowy kontakt)
void addTreeNode(Tree* ,TreeNode*); // dodaje wezel do drzewa (nowy kontakt)
void releaseTreeNode(TreeNode*); // uwalnia pamiec wezla
void deleteTreeNode(Tree* ,TreeNode*); // usuwa wezel z drzewa (usuwa kontakt)
TreeNode * minNode(TreeNode *); // minimalny wezwl w drzewie/poddrzewie
TreeNode * findTreeNode(Tree* ,char*); // szuka wezla w drzewie (szuka kontaktu)
Tree * sortTree(Tree*); // sortuje drzewo wedlug wybranego pola w contact
void treeToList(TreeNode * aNode,List * aList); // robi z drzewa liste
void printTree(TreeNode*); // wypisuje liste (ksiazke adresowa)

#endif
