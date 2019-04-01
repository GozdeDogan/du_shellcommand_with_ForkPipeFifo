/******************************************************************************/
/* main.h                                                                     */  
/* Gozde DOGAN - 131044019                                                    */
/* CSE344 System Programming - Homework 4                                     */
/*                                                                            */ 
/* Her directory icin fork yapildi. (input olarak girilen directory dahil)    */
/* Bu nedenle child process sayisi hesabinda input olarak girilen directory   */
/* icin yapilan fork islemi sonucu olusan child process de dahildir.          */
/*                                                                            */
/* main'de olusturulan fifo ana fifo olarak kullanildi. Bu fifo ya bulunan    */
/* degerler yazildi. Butun islemlerin sonunda fifo'daki veriler okunarak      */
/* ekrana yazilmaktadir.                                                      */
/* bu fifo program icinde kullanilan tek fifodur.                             */
/*                                                                            */
/* Fork yapilmadan once yapilan pipe ile child process, program               */
/* calistirilirken "-z" komutu ile run edilirse buldugu degeri bu pipe'a      */
/* yazarak parant process'in okumasini beklemektedir.                         */
/* Parent process de bu pipe'dan "-z" komutu girilirse degeri okumaktadir.    */
/* Boylece size degerleri hesaplanirken de kolaylik saglnamis olur.           */       
/*                                                                            */   
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>  //wait fonksiyonlari icin
#include <dirent.h> 
#include <errno.h>    
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
//#include "restart.h"

#define MAX_PATH_SIZE 1024
#define MAXSIZE 100000
#define DEBUG
#define SIZEBUFFER 80

/****************************** Global Veriables ******************************/
int iIs_z = 0;
int iFirstSize = 0;
pid_t firstPID;
int iIndex = 0;
char sOldFname[MAX_PATH_SIZE] = "";
int iOldSizeOfDir = 0;
static int iNumOfChildProcess = 0; 
char sTempFile[MAX_PATH_SIZE] = "tempChildProcess.txt";
FILE *fPtrTemp;
char sSizeFiles[MAX_PATH_SIZE] = "131044019sizes.txt";
FILE *fPtrSizeFile;

char sFifoName[MAX_PATH_SIZE] = "/tmp/131044019sizes";
int iFifo = 0;

volatile sig_atomic_t is_signal = 0;
struct sigaction handler;
/******************************************************************************/


/**************************** Function Definitions ****************************/
void doOperation(int argc, char *argv[]);
int postOrderApply(char *path, int pathfun (char *path1));
int sizepathfun (char *path);
void usage();
int isInSameDirectory(char *sString1, char *sString2);
void readFile(char filename[MAX_PATH_SIZE]);
void readFifo();
void signalHandler(int const sig);
void checkSignal();
/******************************************************************************/


