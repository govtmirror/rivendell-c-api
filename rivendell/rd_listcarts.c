/* rd_listcarts.c
 *
 * Implementation of the LastCarts Rivendell Access Library
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

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <expat.h>

#include "rd_common.h"
#include "rd_listcarts.h"

struct xml_data {
  unsigned carts_quan;
  char elem_name[256];
  char strbuf[1024];
  struct rd_cart *carts;
};


static void XMLCALL __ListCartsElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"cart")==0) {    // Allocate a new cart entry
    xml_data->carts=realloc(xml_data->carts,
			   (xml_data->carts_quan+1)*sizeof(struct rd_cart));
    (xml_data->carts_quan)++;
  }
  strncpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}


static void XMLCALL __ListCartsElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __ListCartsElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_cart *carts=xml_data->carts+(xml_data->carts_quan-1);

  if(strcasecmp(el,"number")==0) {
    sscanf(xml_data->strbuf,"%u",&carts->cart_number);
  }
  if(strcasecmp(el,"type")==0) {
    if(strcasecmp(xml_data->strbuf,"audio")==0) {
      carts->cart_type=TYPE_AUDIO;
    }
    if(strcasecmp(xml_data->strbuf,"macro")==0) {
      carts->cart_type=TYPE_MACRO;
    }
    else
    {
      /*  This is ALL type */
      carts->cart_type=TYPE_ALL;
    }
  }

  if(strcasecmp(el,"groupName")==0) {
    strncpy(carts->cart_grp_name,xml_data->strbuf,11);
  }
  if(strcasecmp(el,"title")==0) {
    strncpy(carts->cart_title,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"artist")==0) {
    strncpy(carts->cart_artist,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"album")==0) {
    strncpy(carts->cart_album,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"year")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_year);
  }
  if(strcasecmp(el,"label")==0) {
    strncpy(carts->cart_label,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"client")==0) {
    strncpy(carts->cart_client,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"agency")==0) {
    strncpy(carts->cart_agency,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"publisher")==0) {
    strncpy(carts->cart_publisher,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"composer")==0) {
    strncpy(carts->cart_composer,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"userDefined")==0) {
    strncpy(carts->cart_user_defined,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"usageCode")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_usage_code);
  }
  if(strcasecmp(el,"forcedLength")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_forced_length);
  }
  if(strcasecmp(el,"averageLength")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_average_length);
  }
  if(strcasecmp(el,"lengthDeviation")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_length_deviation);
  }
  if(strcasecmp(el,"averageSegueLength")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_average_segue_length);
  }
  if(strcasecmp(el,"averageHookLength")==0) {
    sscanf(xml_data->strbuf,"%d",&carts->cart_average_hook_length);
  }
  if(strcasecmp(el,"cutQuantity")==0) {
    sscanf(xml_data->strbuf,"%u",&carts->cart_cut_quantity);
  }
  if(strcasecmp(el,"lastCutPlayed")==0) {
    sscanf(xml_data->strbuf,"%u",&carts->cart_last_cut_played);
  }
  if(strcasecmp(el,"validity")==0) {
    sscanf(xml_data->strbuf,"%u",&carts->cart_validity);
  }
  if(strcasecmp(el,"enforceLength")==0) {
    carts->cart_enforce_length=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"asyncronous")==0) {
    carts->cart_asyncronous=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"owner")==0) {
    strncpy(carts->cart_owner,xml_data->strbuf,66);
  }
  if(strcasecmp(el,"metadataDatetime")==0) {
    strncpy(carts->cart_metadata_datetime,xml_data->strbuf,26);
  }
}


size_t __ListCartsCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_ListCarts(struct rd_cart *carts[],
		  	const char hostname[],
			const char username[],
			const char passwd[],
			const char group_name[],
			const char filter[],
			const char type[],
			unsigned *numrecs)
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;

  /*  Set number of recs so if fail already set */
  *numrecs = 0;
  
   /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__ListCartsElementStart,
			__ListCartsElementEnd);
  XML_SetCharacterDataHandler(parser,__ListCartsElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=6&LOGIN_NAME=%s&PASSWORD=%s&GROUP_NAME=%s&FILTER=%s&TYPE=%s",
	   username,passwd,group_name,filter,type);
  if((curl=curl_easy_init())==NULL) {
    return -1;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__ListCartsCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    fprintf(stderr, "\nlibcurl error: (%d)", res);
    if (len)
        fprintf(stderr, "%s%s", errbuf,
            ((errbuf[len-1] != '\n') ? "\n" : ""));
    else
        fprintf(stderr, "%s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return -1;
  }

/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {
    *carts=xml_data.carts;
    *numrecs = xml_data.carts_quan;
    return 0;
  }
  else {
    fprintf(stderr," Call Returned Error: %s\n",xml_data.strbuf);
    return (int)response_code;
  }
}
