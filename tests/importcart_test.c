/* import_test.c
 *
 * Test the Impourt Cart/Cut API Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>             
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rivendell/rd_import.h>

int main(int argc,char *argv[])
{

  char buf[BUFSIZ];
  char *p;
  long int cartnum=0;
  long int cutnum=0;
  int create_flag=0;
  char filename[BUFSIZ];
  char group_name[BUFSIZ]="";
  char *host;
  char *user;
  char *passwd;

  /*      Get the Rivendell Host, User and Password if set in env */
  if (getenv("RIVHOST")!=NULL) {
    host = getenv("RIVHOST");
  }
  else {
    host="localhost";
  }

  if (getenv("RIVUSER")!=NULL) {
    user = getenv("RIVUSER");
  }
  else {
    user="USER";
  }

  if (getenv("RIVPASS")!=NULL) {
    passwd = getenv("RIVPASS");
  }
  else {
    passwd = "";
  } 

  printf("Please enter the Cart Number that you want to Import To  ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cartnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the Cut Number that you want to import To ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cutnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the File Name that you want to Import ==> ");
  if (fgets(filename,sizeof(filename),stdin) != NULL)
  {
  } 
  
  printf("Do you want to Create Cart If Doesn't Exist - (Yes or No) ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    if (( buf[0] != '\n') && (buf[0] =='Y'))
    {
      create_flag = 1;
      printf("Please enter the Group Name ==> ");
      if (fgets(group_name,sizeof(group_name),stdin) != NULL)
      {
      }
    }
  }
  //
  // Call the function
  //
fprintf(stderr," Create flag = %d\n",create_flag);
fprintf(stderr," Group Name= %s\n",group_name);
  
  int result=RD_ImportCart( host,
		user,
		passwd,
		(unsigned)cartnum,
		(unsigned)cutnum,
		(unsigned)2,
		0,
		0,
		0,
		create_flag,
		group_name,
                filename);

  if(result<0) {
    fprintf(stderr,"Something went wrong! Result Code = %d\n",result);
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart/Cut Exists! \n");
        break;
      case  401:
        fprintf(stderr, "ERROR:  Unauthorized Or Cart out of Range! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }

  //
  // List the Results
  //
    printf(" Cart: %ld  -  Cut: %ld  - Filename: %s was successfully imported!\n",cartnum,cutnum,filename);
    printf("\n");

  exit(0);
}
