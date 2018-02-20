#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <ftw.h>

int maxSize;

void printFileInformations(char * filePath, struct stat * fileInfo){
  printf("Sciezka bezwzgledna pliku: %s \n",filePath);
  printf("Rozmiar pliku w bajtach: %ld \n",fileInfo->st_size);
  printf("Prawa dostepu: ");
  printf((S_ISDIR(fileInfo->st_mode)) ? "d":"-");
  printf((fileInfo->st_mode & S_IRUSR) ? "r":"-");
  printf((fileInfo->st_mode & S_IWUSR) ? "w":"-");
  printf((fileInfo->st_mode & S_IXUSR) ? "x":"-");
  printf((fileInfo->st_mode & S_IRUSR) ? "r":"-");
  printf((fileInfo->st_mode & S_IWUSR) ? "w":"-");
  printf((fileInfo->st_mode & S_IXUSR) ? "x":"-");
  printf((fileInfo->st_mode & S_IRUSR) ? "r":"-");
  printf((fileInfo->st_mode & S_IWUSR) ? "w":"-");
  printf((fileInfo->st_mode & S_IXUSR) ? "x":"-");
  printf("\n");
  printf("Ostatnia data modyfikacji: %s \n", asctime(gmtime(&(fileInfo->st_mtime))));
}

void findFiles(char * path, int sizeOfFile){
  DIR * folder;
  struct dirent * position;
  if(!(folder = opendir(path))) {
    perror("folderNotOpen");
    return;
  }
  char * filePath = (char*)calloc(strlen(path),sizeof(char));
  while(position=readdir(folder)){
    if(strcmp(position->d_name,".")!=0 && strcmp(position->d_name,"..")!=0){
      filePath = (char*)calloc(strlen(path)+strlen(position->d_name)+2,sizeof(char));
      strcpy(filePath,path);
      strcat(filePath,"/");
      strcat(filePath,position->d_name);
      struct stat * fileInfo = (struct stat*)malloc(sizeof(struct stat));
      stat(filePath, fileInfo);
      if(S_ISDIR(fileInfo->st_mode)) findFiles(filePath,sizeOfFile);
      else if(fileInfo->st_size < sizeOfFile) printFileInformations(filePath,fileInfo);
      free(fileInfo);
    }
  }
  free(filePath);
  closedir(folder);
}

static int printFileInformationsNFTW(const char * filePath, const struct stat * fileInfo, int typeflag, struct FTW * ftwbuf){
  if(fileInfo->st_size > maxSize) return 0;
  printf("Sciezka bezwzgledna pliku: %s \n",filePath);
  printf("Rozmiar pliku w bajtach: %ld \n",fileInfo->st_size);
  printf("Prawa dostepu: ");
  printf((S_ISDIR(fileInfo->st_mode)) ? "d":"-");
  printf((fileInfo->st_mode & S_IRUSR) ? "r":"-");
  printf((fileInfo->st_mode & S_IWUSR) ? "w":"-");
  printf((fileInfo->st_mode & S_IXUSR) ? "x":"-");
  printf((fileInfo->st_mode & S_IRGRP) ? "r":"-");
  printf((fileInfo->st_mode & S_IWGRP) ? "w":"-");
  printf((fileInfo->st_mode & S_IXGRP) ? "x":"-");
  printf((fileInfo->st_mode & S_IROTH) ? "r":"-");
  printf((fileInfo->st_mode & S_IWOTH) ? "w":"-");
  printf((fileInfo->st_mode & S_IXOTH) ? "x":"-");
  printf("\n");
  printf("Ostatnia data modyfikacji: %s \n", asctime(gmtime(&(fileInfo->st_mtime))));
  return 0;
}

void findFilesNWFD(char * path, int sizeOfFile){
  int flags = 0;
  flags = FTW_DEPTH;
  if(nftw(path,printFileInformationsNFTW,20,flags)==-1){
    perror("nftw");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv){
  char * path = (char *)malloc(256 * sizeof(char));
  path = argv[1];
  int sizeOfFile = atoi(argv[2]);

  if(argc!=3){
    printf("Zla liczba argumentow. Maja byc dwa.\n");
    exit(1);
  }
  if(sizeOfFile<=0){
    printf("Argument liczbowy (nr 2) musi byc wiekszy od 0.\n");
    exit(1);
  }

  printf("Normalnie.\n");
  findFiles(path,sizeOfFile);
  maxSize = sizeOfFile;
  printf("Za pomoca nwfd.\n");
  findFilesNWFD(path,sizeOfFile);

  return 0;
}
