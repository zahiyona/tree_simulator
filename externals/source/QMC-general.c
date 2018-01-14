#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "QMC.h"


 void log_msg(char *msg)
 {
   time_t t1;	
   FILE *ferr;
   struct tm * local;	
   char day[6], mon[6], time_s[10], year[10], day_n[4];
   ferr = fopen ("MXCN.log","a");
   					      
   t1 = time(NULL);						
  						
   local = localtime(&t1);
   
  
   if (sscanf (asctime(local), "%3s %3s %[0-9] %[0-9:] %4s",
	      day, mon, day_n, time_s, year) != 5){
     printf("Invalid sscanf for time %s\n", asctime(local));
     exit(8);
   }

   fprintf(ferr,"\n\n------------------------------\n");
   fprintf(ferr,"%s %s %s. %s", mon, day_n, time_s, msg);		
   fclose(ferr);					      
 }


/*---------------------------------------------------------------------*/

int randInt(int max)
{
  int mid;
  mid =  rand() % max;
  //  printf ("mid %d \n",mid);
  return mid;
}



/*---------------------------------------------------------------------*/

int getStr(char *dest, int destLen, char * source, int cpyLen){

  if (destLen < cpyLen + 1){
    EXIT("Destination length %d is too short to copy %d bytes from source\n",
	destLen, cpyLen);
  }

  if (strlen(source) < cpyLen){
    EXIT("source string len %d, is shorter than cpyLen %d\n", strlen(source), cpyLen);
  }

  strncpy(dest, source, cpyLen);
  dest[cpyLen] = '\0';
  return 0;

}

