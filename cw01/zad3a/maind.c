#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include "addressBook.h"

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

void clockStarts(){
  st_time = times(&st_cpu);
}

void clockEnds(double * t,FILE * fp){
  static long clock = 0;
  clock = sysconf(_SC_CLK_TCK);
  en_time = times(&en_cpu);
  t[0] = (double)(en_time - st_time) / (double)clock;
  t[1] = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / (double)clock;
  t[2] = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / (double)clock;
  fprintf(fp,"REAL TIME: %4.2f, USER TIME: %4.2f, SYSTEM TIME: %4.2f \n", t[0], t[1], t[2]);
}

int main(){

  FILE * fp;
  if((fp = fopen("results.txt", "w")) == NULL){
    fprintf(fp,"Nie działa cos");
    exit(1);
  }

  void * handle = dlopen("libdynamic.so", RTLD_LAZY);

  List * (*createList)() = dlsym(handle,"createList");
  void (*deleteList)(List*) = dlsym(handle,"deleteList");
  ListNode * (*makeListNode)(char* ,char* ,char* ,char* ,char* ,char*) = dlsym (handle,"makeListNode");
  void  (*addListNodeToBeginning)(List* ,ListNode*) = dlsym(handle,"addListNodeToBeginning");
  void (*addListNodeToEnd)(List* ,ListNode*) = dlsym(handle,"addListNodeToEnd");
  void (*releaseListNode)(ListNode*) = dlsym(handle,"releaseListNode");
  void (*deleteListNode)(List* ,ListNode*) = dlsym(handle,"deleteListNode");
  ListNode * (*findListNode)(List* ,char* ,char*) = dlsym(handle,"findListNode");
  List * (*sortList)(List* ,int) = dlsym(handle,"sortList");
  void (*printList)(List* ,int) = dlsym(handle,"printList");

  Tree * (*createTree)() = dlsym(handle,"createTree");
  void (*deleteTree)(Tree*) = dlsym(handle,"deleteTree");
  void (*deleteAllNodes)(TreeNode *) = dlsym(handle,"deleteAllNodes");
  TreeNode * (*makeTreeNode)(char* ,char* ,char* ,char* ,char* ,char*) = dlsym(handle,"makeTreeNode");
  void (*addTreeNode)(Tree* ,TreeNode*) = dlsym(handle,"addTreeNode");
  void (*releaseTreeNode)(TreeNode*) = dlsym(handle,"releaseTreeNode");
  void (*deleteTreeNode)(Tree* ,TreeNode*) = dlsym(handle,"deleteTreeNode");
  TreeNode * (*minNode)(TreeNode *) = dlsym(handle,"minNode");
  TreeNode * (*findTreeNode)(Tree* ,char*) = dlsym(handle,"findTreeNode");
  Tree * (*sortTree)(Tree*) = dlsym(handle,"sortTree");
  void (*treeToList)(TreeNode * aNode,List * aList) = dlsym(handle,"treeToList");
  void (*printTree)(TreeNode*) = dlsym(handle,"printTree");

  srand(time(NULL));
  int r = 0; //liczba randomowa potrzebna do przypisywanaia danych do wezlow
  double * t1 = (double*)malloc(3*sizeof(double)); // potrzebna do czasu

  // tablice randomowych danych
  char * names[] = {"Karolina", "Aurelia","Magdalena","Klaudia","Agnieszka"};
  char * surnames[] = {"Kowalska", "Majewska","Chomik","Pies","Kot"};
  char * birthDates[] = {"23.12.12","12.09.03","13.06.09","04.07.94","16.01.00"};
  char * emails[] = {"kotek@gmail.com","piesek@gmail.com","malpka@gmail.com","tygrysek@gmail.com","slonik@gmail.com"};
  char * phoneNumbers[] = {"664938472","274624837","131415167","950284752","185726354"};
  char * addresses[] = {"Chmielna 2, 29-212 Krakow","Jaworowa 23/2, 28-100 Warszawa","Lancut","Kolobrzeg","Wysoka"};

  //------- tworzenie ksiazki adresowej za pomoca list -------

  fprintf(fp,"DLA LISTY !!!\n");

  // tworzenie ksaizki adresowej
  fprintf(fp,"Tworzenie ksiazki adresowej.\n");
  clockStarts();
  List * book = createList();
  char * features[6];
  for(int i=0; i<10000; i++){
    r=rand()%5;
    features[0] = names[r];
    r=rand()%5;
    features[1] = surnames[r];
    r=rand()%5;
    features[2] = birthDates[r];
    r=rand()%5;
    features[3] = emails[r];
    r=rand()%5;
    features[4] = phoneNumbers[r];
    r=rand()%5;
    features[5] = addresses[r];
    ListNode * contact = makeListNode(features[0],features[1],features[2],features[3],features[4],features[5]);
    addListNodeToEnd(book,contact);
  }
  clockEnds(t1,fp);

  // dodawanie pojedynczego elementu
  fprintf(fp,"Dodawanie pojedynczego elementu.\n");
  clockStarts();
  ListNode * contact = makeListNode("Kinga","Kaczmarczyk","23.10.1996","nieznajoma@gmail.com","111111111","Krakowska 30, 30-072 Krakow");
  addListNodeToEnd(book,contact);
  clockEnds(t1,fp);

  // usuwanie kontaktu z ksiazki
  fprintf(fp,"Usuwanie kontaktu z ksiazki - optymistyczny.\n");
  clockStarts();
  ListNode * foundNode = findListNode(book,"Karolina","Pies");
  deleteListNode(book,foundNode);
  clockEnds(t1,fp);

  fprintf(fp,"Usuwanie kontaktu z ksiazki - pesymistyczny.\n");
  clockStarts();
  foundNode = findListNode(book,"Kinga","Kaczmarczyk");
  deleteListNode(book,foundNode);
  clockEnds(t1,fp);

  // wyszukiwanie kontaktu w ksiazce
  fprintf(fp,"Wyszukiwanie kontaktu w ksiazce - optymistyczny.\n");
  clockStarts();
  foundNode = findListNode(book,"Aurelia","Majewska");
  clockEnds(t1,fp);

  fprintf(fp,"Wyszukiwanie kontaktu w ksiazce - pesymistyczny.\n");
  clockStarts();
  foundNode = findListNode(book,"Dominika","Nic");
  clockEnds(t1,fp);

  // sortowanie ksiazki
  fprintf(fp,"Sortowanie ksiazki.\n");
  clockStarts();
  List * newbook = createList();
  newbook = sortList(book,1);
  clockEnds(t1,fp);

  // usuwanie ksiazki
  fprintf(fp,"Usuwanie ksiazki.\n");
  fprintf(fp,"- pustej\n");
  clockStarts();
  deleteList(book);
  clockEnds(t1,fp);
  fprintf(fp,"- niepustej\n");
  clockStarts();
  deleteList(newbook);
  clockEnds(t1,fp);

  //------- tworzenie ksiazki adresowej za pomoca drzewa -------

  fprintf(fp,"DLA DRZEWA !!!\n");

  // tworzenie ksaizki adresowej
  fprintf(fp,"Tworzenie ksiazki adresowej.\n");
  clockStarts();
  Tree * tbook = createTree();
  char * tfeatures[6];
  for(int j=0; j<10000; j++){
    r=rand()%5;
    tfeatures[0] = names[r];
    r=rand()%5;
    tfeatures[1] = surnames[r];
    r=rand()%5;
    tfeatures[2] = birthDates[r];
    r=rand()%5;
    tfeatures[3] = emails[r];
    r=rand()%5;
    tfeatures[4] = phoneNumbers[r];
    r=rand()%5;
    tfeatures[5] = addresses[r];
    TreeNode * tcontact = makeTreeNode(tfeatures[0],tfeatures[1],tfeatures[2],tfeatures[3],tfeatures[4],tfeatures[5]);
    addTreeNode(tbook,tcontact);
  }
  clockEnds(t1,fp);

  // dodawanie pojedynczego elementu
  fprintf(fp,"Dodawanie pojedynczego elementu.\n");
  clockStarts();
  TreeNode * tcontact = makeTreeNode("Kinga","Kaczmarczyk","23.10.1996","nieznajoma@gmail.com","111111111","Krakowska 30, 30-072 Krakow");
  addTreeNode(tbook,tcontact);
  clockEnds(t1,fp);

  // usuwanie kontaktu z ksiazki
  fprintf(fp,"Usuwanie kontaktu z ksiazki - optymistyczny.\n");
  clockStarts();
  TreeNode * tfoundNode = findTreeNode(tbook,"Kowalska");
  deleteTreeNode(tbook,tfoundNode);
  clockEnds(t1,fp);

  fprintf(fp,"Usuwanie kontaktu z ksiazki - pesymistyczny.\n");
  clockStarts();
  tfoundNode = findTreeNode(tbook,"Kaczmarczyk");
  deleteTreeNode(tbook,tfoundNode);
  clockEnds(t1,fp);

  // wyszukiwanie kontaktu w ksiazce
  fprintf(fp,"Wyszukiwanie kontaktu w ksiazce - optymistyczny.\n");
  clockStarts();
  tfoundNode = findTreeNode(tbook,"Kowalska");
  clockEnds(t1,fp);

  fprintf(fp,"Wyszukiwanie kontaktu w ksiazce - pesymistyczny.\n");
  clockStarts();
  tfoundNode = findTreeNode(tbook,"Kaczmarczyk");
  clockEnds(t1,fp);

  // sortowanie ksiazki
  fprintf(fp,"Sortowanie ksiazki.\n");
  clockStarts();
  Tree * tnewbook = createTree();
  tnewbook = sortTree(tbook);
  clockEnds(t1,fp);

  // usuwanie ksiazki
  fprintf(fp,"Usuwanie ksiazki.\n");
  fprintf(fp,"- pustej\n");
  clockStarts();
  deleteTree(tbook);
  clockEnds(t1,fp);
  fprintf(fp,"- niepustej\n");
  clockStarts();
  deleteTree(tnewbook);
  clockEnds(t1,fp);

  fclose(fp);
  dlclose(handle);

  return(0);
}
