/******************************************************************************/
/* main.c                                                                     */  
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

#include "main.h"


/******************************** START OF MAIN *******************************/
int main(int argc, char *argv[])
{
	if((argc != 2 && argc != 3) || (argc == 3 && strcmp(argv[1], "-z") != 0))
	{
		usage();
		return 0;
	}
	
    doOperation(argc, argv);
    		
	return 0;
}
/********************************** END OF MAIN *******************************/

/************************** Function Declaration ******************************/

/*
 * Kullanici tarafindan girilen compiler kodu kontrol edilerek 
 * islemler gerceklestiriliyor.
 * Kullanicinin calistiriken "-z" komutunu girip girmedigi kontrol eidliyor.
 * Bunun sonucuna gore ekrana yazdirilacak seyler ve klasor boyutlarinin 
 * toplanma sekli degismektedir.
 *
 * input: 
 *  argc: a number of string in the compiler code
 *  argv: strings in the compiler code
 */
void doOperation(int argc, char *argv[])
{
    int totalSize = 0;
    int check = 0;
    char s[MAXSIZE] = ""; 
    pid_t mainProcessPid = getpid();
    sprintf(sOldFname, "%s", argv[argc-1]);

////////////////////// SIGNAL HANDLER ////////////////////////////////    
    handler.sa_handler = signalHandler;
    handler.sa_flags = SA_RESTART;
    
    check = sigemptyset(&handler.sa_mask);   
    if (check == -1) 
    {
        perror("sigemptyset: ");
        exit(EXIT_FAILURE);
    }

    check = sigaction(SIGINT, &handler, NULL);
    if (check == -1) 
    {
        perror("sigaction: ");
        exit(EXIT_FAILURE);
    }
    
    check = sigaction(SIGTSTP, &handler, NULL);
    if (check == -1) 
    {
        perror("sigaction: ");
        exit(EXIT_FAILURE);
    }
////////////////////////////////////////////////////////////////////////

	unlink(sFifoName);
	
    check = mkfifo(sFifoName, (S_IRUSR | S_IWUSR));
    if(check == -1)
    {
        perror("mkfifo: ");
        exit(EXIT_FAILURE);
    }
    
	iFifo = open(sFifoName, O_RDWR | O_TRUNC);
    if(iFifo == -1)
    {
        perror("open fifo: ");
        exit(EXIT_FAILURE);
    }

    if(argc == 3 && strcmp(argv[1], "-z") == 0)
        iIs_z = 1;
     
    if(argc == 2)
    {
        #ifndef DEBUG
            fprintf(stderr, "Output of \"buNeDuFork %s\" don't add subdirectory sizes:\n", argv[1]);
            fprintf(stderr, "PID\t\tSIZE\t\tPATH \n");
            
            fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
            fprintf(fPtrSizeFile, "Output of \"buNeDu %s\" don't add subdirectory sizes:\n", argv[1]);
            fprintf(fPtrSizeFile, "PID\t\tSIZE\t\tPATH \n");
            fclose(fPtrSizeFile);
        #endif        
        
        //////////////////////////////// PRINT FIFO ////////////////////////////
        sprintf(s, "Output of \"buNeDuFork %s\" don't add subdirectory sizes:\n", argv[1]);
        write(iFifo, &s, strlen(s));
        sprintf(s, "PID\t SIZE\t\tPATH \n");
        write(iFifo, &s, strlen(s));
        ////////////////////////////////////////////////////////////////////////
        
        totalSize = postOrderApply(argv[argc-1], sizepathfun);
        
        #ifndef DEBUG
            fprintf(stderr, "%d\t  %d\t\t%s\n", mainProcessPid, iFirstSize, argv[argc-1]); 
            
            fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
            fprintf(fPtrSizeFile, "%d\t  %d\t\t%s\n", firstPID, iFirstSize, argv[argc-1]); 
            fclose(fPtrSizeFile); 
        #endif          
        
        //////////////////////////////// PRINT FIFO ////////////////////////////       
        sprintf(s, "%d\t  %d\t\t%s\n", firstPID, iFirstSize, argv[argc-1]);
        write(iFifo, &s, strlen(s));         
        //////////////////////////////// PRINT FIFO ////////////////////////////
    }
    
    else if(argc == 3)
    {
        #ifndef DEBUG
            fprintf(stderr, "Output of \"buNeDuFork %s %s\" gives total sizes:\n", argv[1], argv[2]);
            fprintf(stderr, "PID\t\tSIZE\t\tPATH \n");
            
            fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
            fprintf(fPtrSizeFile, "Output of \"buNeDuFork %s %s\" gives total sizes:\n", argv[1], argv[2]);
            fprintf(fPtrSizeFile, "PID\t\tSIZE\t\tPATH \n");
            fclose(fPtrSizeFile);
        #endif
               
        //////////////////////////////// PRINT FIFO ////////////////////////////
        sprintf(s, "Output of \"buNeDuFork %s %s\" gives total sizes:\n", argv[1], argv[2]);
        write(iFifo, &s, strlen(s));
        sprintf(s, "PID\t SIZE\t\tPATH \n");
        write(iFifo, &s, strlen(s));
        ////////////////////////////////////////////////////////////////////////
        
        totalSize = postOrderApply(argv[argc-1], sizepathfun);
        
        #ifndef DEBUG
            fprintf(stderr, "%d\t  %d\t\t%s\n", firstPID, totalSize+iFirstSize, argv[argc-1]);
            
            fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
            fprintf(fPtrSizeFile, "%d\t  %d\t\t%s\n", firstPID, totalSize+iFirstSize, argv[argc-1]); 
            fclose(fPtrSizeFile);
        #endif         
        
        //////////////////////////////// PRINT FIFO ////////////////////////////
        sprintf(s, "%d\t  %d\t\t%s\n", mainProcessPid, totalSize+iFirstSize, argv[argc-1]);
        write(iFifo, &s, strlen(s));
        //////////////////////////////// PRINT FIFO ////////////////////////////
    }
    else
    {
        usage();
        exit(EXIT_FAILURE);
    }
    
    fPtrTemp = fopen(sTempFile, "r");
    fscanf(fPtrTemp, "%d", &iNumOfChildProcess);
    fclose(fPtrTemp);
    remove(sTempFile);
    
    #ifndef DEBUG
        fprintf(stderr, "%d child processes created. Main process is %d.\n", iNumOfChildProcess, mainProcessPid);
        
        fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
        fprintf(fPtrSizeFile, "%d child processes created. Main process is %d.\n\n\n", iNumOfChildProcess, mainProcessPid);    
        fclose(fPtrSizeFile);
    #endif    
    
    //////////////////////////////// PRINT FIFO ////////////////////////////
    sprintf(s, "%d child processes created. Main process is %d.\n\n\n", iNumOfChildProcess, mainProcessPid);
    write(iFifo, &s, strlen(s));
    //////////////////////////////// PRINT FIFO ////////////////////////////
    
    /////////////////////////////////////////////////////////////////////////////
   // readFile(sSizeFiles); // FIFO dan okumaya cevirmelisin
    readFifo(iFifo);
    
    close(iFifo);
    unlink(sFifoName);
}


/*
 * Directory icindeki directory'lerin boyutlarini buluyor.
 * Kullanici tarafindan girilen calistirma kodundaki "-z" degerinin 
 * varligina gore hesaplama yapilmakta.
 *
 * input: 
 *  path: directory name
 *  int pathfun(char *path1): function name
 * output:
 *  int: total size of directories in input directory
 */
int postOrderApply(char *path, int pathfun (char *path1))
{
    DIR *dir;
    struct dirent *dosya;																	
	struct stat status, status_1;  //Degisen fname ile birlikte status durumunu alir ve S_ISDIR fonksiyonunda kullanir

	//Variables
	int iSizeFname = 0;
	char fname[MAX_PATH_SIZE], path_1[MAX_PATH_SIZE]; //dosyanin adini tutar
	static int iSizeOfTotalDir = 0;
	int iSizeOfDir = 0;
	int iPrintSize = 0;
	pid_t childPid;
	int status1;
	static int iTotalSize = 0;
	int err = -1000;
	
	////////////////////////////// CREAT PIPE /////////////////////////////////
	int fd_pipe[2];
	
	if (pipe(fd_pipe)==-1) 
    { 
        perror("pipe: "); 
        return 1; 
    } 
	////////////////////////////// CREAT PIPE /////////////////////////////////
	
    if (lstat(path, &status_1) < 0)
    {
        perror ("lstat");
        exit(EXIT_FAILURE); 
    }

    if (!S_ISDIR(status_1.st_mode))
    {
        fprintf(stderr, "%s not a directory!!\n", path);
        exit(EXIT_FAILURE); 
    }
	// input olarak girilen klasorun icindeki dosyalarin boyutlari bulundu.
	if(iIndex == 0)
	{
	    iFirstSize = pathfun(path);
	    firstPID = getpid();
	    iOldSizeOfDir = iFirstSize;
	    iIndex++;
	}
    
    //Directory acilabiliyor mu kontrolu yaptim
    if ((dir = opendir(path)) == NULL) {
	    //perror("opendir");
	    fprintf(stderr, "%d\t%s\n", pathfun(path), path);
	    exit(EXIT_FAILURE); 
    }	

    while ((dosya = readdir(dir)) != NULL) 
    {	
        //Dosya adi "." veya ".." olmadiginda islem yapilacak.
	    if ((strcmp(dosya->d_name, ".") != 0) && (strcmp(dosya->d_name, "..") != 0 )) 
	    {        		
	        sprintf(fname, "%s/%s", path, dosya->d_name); //dosya ismini fname'e attim
	        //fprintf(stderr, "%s\n", fname);

	        iSizeFname=strlen(fname);
	        if( fname[iSizeFname-1] != '~'  )
	        {   
	            #ifndef DEBUG
		            puts(fname);
	            #endif
		        if (lstat(fname, &status) == -1) 
		        {                            
			        //perror("lstat");                                  
			        break;
		        }
		        if (S_ISDIR(status.st_mode))   //File OR Directory kontrolu
		        {		                       // Directory ise islem yapilmaktadir.	        
                   childPid = fork();
		                                
                    if(childPid == -1)
                    {
                        perror("fork: ");
                        exit(EXIT_FAILURE);
                    }

                    int iNumOfChildProcessOld = 0;

                    fPtrTemp = fopen(sTempFile, "r");
                    if(fPtrTemp != NULL)
                    {
                        fscanf(fPtrTemp, "%d", &iNumOfChildProcessOld);
                        fclose(fPtrTemp);
                        
                        //fprintf(stderr, "--iNumOfChildProcessOld: %d\n", iNumOfChildProcessOld);
                        
                        if(iNumOfChildProcessOld > iNumOfChildProcess+1)
                            iNumOfChildProcess = iNumOfChildProcessOld + iNumOfChildProcess;
                    }                 

                    iNumOfChildProcess++; // child process sayisi arttirildi		                

                    if(childPid == 0)
                    {                                            
                        //////////////////// CONTROL SIGNAL ////////////////////
                        int check = sigaction(SIGINT, &handler, NULL);
                        if(check == -1){
	                        perror("\tCan't SIGINT");
	                        //exit(EXIT_FAILURE);
                        }

                        check = sigaction(SIGTSTP, &handler, NULL);
                        if(check == -1){
	                        perror("\tCan't SIGTSTP");
	                        //exit(EXIT_FAILURE);
                        }
                        
                        if(is_signal > 0)
                        {
                            closedir(dir);
                            _exit(EXIT_SUCCESS);
                        }
                        
                        ////////////////////////////////////////////////////////

                        fPtrTemp = fopen(sTempFile, "w");
                        fprintf(fPtrTemp, "%d", iNumOfChildProcess);
                        fclose(fPtrTemp);
                                            
                        //fprintf(stderr, " CHILD - iNumOfChildProcess: %d\n", iNumOfChildProcess);
                    
	                    if(iIs_z != 0) // "-z" komutunun girildiginde add subdirectory
	                    {
	                        //////////////// Close pipe for reading ///////////
	                        close(fd_pipe[0]); 
        
	                        iSizeOfDir = 0;
                        	iSizeOfDir = pathfun(fname);
                        	
                        	int flag = 0;
                        	
                        	char sString[MAX_PATH_SIZE];
		                    sprintf(sString, "%s", fname);
	                        
		                    if(isInSameDirectory(sOldFname, sString) == 1) // ayni klasor icindeki klasorlerin boyutlari toplanmamali
		                    {
		                        iPrintSize -= iOldSizeOfDir;
		                        flag = 1;
		                    }
		                    if(iSizeOfTotalDir + iPrintSize > 0) // en icteki klasore kadar ilk deger 0 olmaliydi
	                            iPrintSize -= iOldSizeOfDir;
	                        
	                        if(flag != 1)
	                            iSizeOfTotalDir += iSizeOfDir + postOrderApply(fname, pathfun);
                            else
	                            iSizeOfTotalDir = iSizeOfDir - iOldSizeOfDir + postOrderApply(fname, pathfun);
	                       
	                        #ifndef DEBUG
		                        fprintf(stderr, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
		                        
		                        fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
		                        fprintf(fPtrSizeFile, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
		                        fclose(fPtrSizeFile);
		                    #endif
		                                           
	                        //////////////////////////////// PRINT FIFO ////////////////////////////
	                        char s[MAXSIZE] = "";
                            sprintf(s, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
                            write(iFifo, &s, strlen(s));
                            //////////////////////////////// PRINT FIFO ////////////////////////////
	                        
	                        ///////////////////// Write pipe //////////////////
                            write(fd_pipe[1], &iSizeOfDir, sizeof(iSizeOfDir)); 
                            close(fd_pipe[1]);	                        
	                        ///////////////////// Write pipe ////////////////// 
	                        
	                        iOldSizeOfDir = iSizeOfDir;
		                        
		                    sprintf(sOldFname, "%s", fname);  
	                    }
	                    else // "-z girilmediginde don't add subdirectory"
	                    {	
	                    
	                        iSizeOfTotalDir = 0;
	                        iSizeOfTotalDir = pathfun(fname);

	                        postOrderApply(fname, pathfun);
	                        
	                        #ifndef DEBUG
		                        fprintf(stderr, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
	                        	
	                        	fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
	                        	fprintf(fPtrSizeFile, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
                                fclose(fPtrSizeFile);
		                    #endif
	                    	                            
                            //////////////////////////////// PRINT FIFO ////////////////////////////
	                        char s[MAXSIZE] = "";
                            sprintf(s, "%d\t  %d\t\t%s\n", getpid(), iSizeOfTotalDir, fname);
                            write(iFifo, &s, strlen(s));
                            //////////////////////////////// PRINT FIFO ////////////////////////////
	                    }
	                    
	                    _exit(iSizeOfTotalDir);	
                    }
                    else
                    {
                        ///////////////////// CONTROL SIGNAL //////////////////
                        int check = sigaction(SIGINT, &handler, NULL);
                        if(check == -1){
                   			perror("\tCan't SIGINT");
                   			//exit(EXIT_FAILURE);
                   		}
                   		
                   		check = sigaction(SIGTSTP, &handler, NULL);
                        if(check == -1){
                   			perror("\tCan't SIGTSTP");
                   			//exit(EXIT_FAILURE);
                   		}
                   		
                   		if(is_signal > 0)
                        {
                            wait(NULL);
                            closedir(dir);	
		                    _exit(EXIT_SUCCESS);
                        }
                   		
                        ///////////////////////////////////////////////////////
                    
                        fPtrTemp = fopen(sTempFile, "r");
                        if(fPtrTemp != NULL)
                        {
                            fscanf(fPtrTemp, "%d", &iNumOfChildProcess);
                            fclose(fPtrTemp);
                        
                            //fprintf(stderr, " PARENT - iNumOfChildProcess: %d\n", iNumOfChildProcess); 
                        }
                        
                        if(iIs_z != 0){
                            //////////////// Close pipe for writing ///////////
	                        close(fd_pipe[1]); 
	                        
	                        ///////////////////// Read pipe //////////////////
                            read(fd_pipe[0], &iSizeOfTotalDir, sizeof(iSizeOfTotalDir)); 
                            close(fd_pipe[0]);	                        
	                        ///////////////////// Read pipe ////////////////// 
	                        
                        }
                        
                        wait(&status1);	
		                iSizeOfTotalDir += WEXITSTATUS(status1);
                    }	
		        }																						
            }   				    
	    }
    }  
    while ((closedir(dir) == -1) && (errno == EINTR)) ;	   
    //fprintf(stderr, "SON - iSizeOfTotalDir: %d\n", iSizeOfTotalDir);
	return iSizeOfTotalDir;
}


/*
 * Directory icindeki dosyalarin boyutlarinin toplamini buluyor.
 * link ya da fifo gibi ozel bir file'a denk gelindiginde
 * dosyanın ozel oldugu yazilarak boyutu 0 kabul ediliyor.
 *
 * input: 
 *  path: directory name
 * output:
 *  int: total size of files in directory
 */
int sizepathfun (char *path)
{
    DIR *dir;
    struct dirent *dit;
    struct stat st;

    int iSizeFilePath = 0;
    int iSizeOfFile = 0;

    char filePath[MAX_PATH_SIZE];

    if ((dir = opendir(path)) == NULL) 
    {
		//perror("opendir");
		return 0;
	}
    
    while ((dit = readdir(dir)) != NULL)
    {
        if ((strcmp(dit->d_name, ".") != 0) && (strcmp(dit->d_name, "..") != 0 )) 
		{        
			sprintf(filePath, "%s/%s", path, dit->d_name);
			//lstat kullanildi, cunku ozel dosyalar boyle gorulebilmektedir.
            if (lstat(filePath, &st) == -1) 
			{                            
				//perror("stat");                                  
				break;
			}

			iSizeFilePath = strlen(filePath);
			if(filePath[iSizeFilePath-1] != '~')
			{ 
                if(!S_ISDIR(st.st_mode))
                {
			        if(S_ISLNK(st.st_mode) || S_ISFIFO(st.st_mode) || S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode))
			        {
			            #ifndef DEBUG
			                fprintf(stderr, "%d\t  -1\t\t%s\n", getpid(), dit->d_name);
			                
			                fPtrSizeFile = fopen(sSizeFiles, "a+"); // FIFO YAP
			                fprintf(fPtrSizeFile, "%d\t  -1\t\t%s\n", getpid(), dit->d_name);
			                fclose(fPtrSizeFile);
			            #endif			            
			            
			            //////////////////////////////// PRINT FIFO ////////////////////////////
                        char s[MAXSIZE] = "";
                        sprintf(s, "%d\t  -1\t\t%s\n", getpid(), dit->d_name);
                        write(iFifo, &s, strlen(s));
                        //////////////////////////////// PRINT FIFO ////////////////////////////
		            }
		            else
		            {
                        #ifndef DEBUG
                            fprintf(stderr, "\tFile: %s --- Size: %d\n", dit->d_name, (int)st.st_size);
                        #endif
                        iSizeOfFile += st.st_size;
                    }
                }
            }
        }
    }
    
    #ifndef DEBUG
        fprintf(stderr, "\n\tDir: %s --- Size: %d\n\n", path, iSizeOfFile);
    #endif
    
    closedir(dir);
    return iSizeOfFile;
}


/*
 * Iki path'in ayni directory icinde olup olamdigini buluyor.
 * Gelen iki path icindeki "/" karakteri sayisi bulundu.
 * Gelen iki path icinde de ayni degerde "/" karakteri varsa 
 * ayni directory icinde yer aldiklari dogrulanmis olur.
 *
 * input: 
 *  sString1: old directory name
 *  sString2: new directory name
 * output:
 *  int: returns 1 if they are in the same directory
 *       returns 0 if they are not in the same directory
 */
int isInSameDirectory(char *sString1, char *sString2)
{
    #ifndef DEBUG
        fprintf(stderr, "\n\nsString1: %s ---- sString2: %s\n", sString1, sString2);
    #endif
    
    const char s[2] = "/";
    int iCount1 = 0, iCount2 = 0;
    char *sToken1, *sToken2;
    
    sToken1 = strtok(sString1, s);
    while(sToken1 != NULL) 
    {
        iCount1++;
        sToken1 = strtok(NULL, s);
    }
    
    sToken2 = strtok(sString2, s);
    while(sToken2 != NULL) 
    {
        iCount2++;
        sToken2 = strtok(NULL, s);
    }
    
    #ifndef DEBUG
        fprintf(stderr, "iCount1: %d ---- iCount2:%d\n\n", iCount1, iCount2);
    #endif
    
    if(iCount1 == iCount2)
        return 1;
    return 0;    
}

#ifndef DEBUG
    void readFile(char filename[MAX_PATH_SIZE])
    {
        char sTempStr[MAX_PATH_SIZE] = "";
        FILE *fPtrSizeFile = fopen(sSizeFiles, "a+");

        if (fPtrSizeFile == NULL)
        {
            perror("fopen: ");
            exit(EXIT_FAILURE);
        }

        while((fgets(sTempStr, 80, fPtrSizeFile)) != NULL)
          fprintf(stderr, "%s", sTempStr);
     
        fclose(fPtrSizeFile);
    }
#endif

void readFifo(int fd)
{
    char s[100] = "";
    int reads = 0;
    
    while(1){			
		reads = read(fd, s, 100);		
		if(reads > 0){
			s[reads] = '\0';
			
			fprintf(stderr, "%s", s);
					
			if(reads < 100)
				break;
		}else{ 		    
			exit(1);
		}			
	}
    
}

/*
 * Yanlis derleme sonrasi kullaniciya dogrusu gosterilmektedir
 */
void usage()
{
    fprintf(stderr, "Usage:\n");
	fprintf(stderr, "-----------------------------------\n");
	fprintf(stderr, "\t./buNeDuFork \"filename\" \n \t\tOR \n\t./buNeDu -z \"filename\"\n");
	fprintf(stderr, "-----------------------------------\n");
}


void signalHandler(int const sig) 
{
    switch (sig) {
        case SIGINT:
            fprintf(stderr, "CTRL-C signal caught!\n");
            is_signal++;
            break;
        case SIGTSTP:
            fprintf(stderr, "CTRL-Z signal caught!\n");
            is_signal++;
            break;
    }
    _exit(EXIT_FAILURE);
}

void checkSignal(int pid)
{
    if(is_signal > 0)
    {
        //close files
        //exit
        // others
    }
}

/******************************************************************************/
