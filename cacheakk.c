#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cacheakk.h"

struct Block
{
	int valid;
	int tag;
	int dirty;
	int LRU;
};

struct Cache
{
	int hits;
	int misses;
	int cache_size;
	int block_size;
	int numblocks;
    int numlines;
    int reads; 
    int writes;
	int associativity;
	int write_policy;
	struct Block** blocks;
 
};



int main(int argc,char  *argv[])
{
   
    struct Cache* cache;
    int cache_size,block_size,associativity,write_policy;
    char line[LINES],*words=NULL,*RW=NULL,*address=NULL;
    char *str;
    int i,decaddress=0,num;

    printf("Enter Cache size  in MB \n");
    scanf("%d", &cache_size);
    printf("Enter Block size in Kbytes \n");
    scanf("%d",&block_size);
    printf("Enter Associativity \n");
    scanf("%d",&associativity);
	printf("Enter Write Policy: 1 for Write through , 2 for Write-Back\n");
    scanf("%d",&write_policy);
    
    /************************************   CREATING A CACHE  ***********************/
    cache = makeCache(cache_size,block_size,associativity,write_policy);
    //Address given in readfromcache
    
    FILE *fp= fopen("trace.txt","r");
    if(fp==NULL)
     printf("cant open text");
    else
    {
        while(fgets(line,LINES,fp)!=NULL)
        {
           
           words = strdup (line); 
           RW= strtok(words," ");
           address= strtok(NULL," ");
           decaddress=0;
           
           if(!strcmp(RW,"R"))
           {
                          
                for(i=2;i<strlen(address)-1;i++)
                	{
                		num=hextodec(address[i]);
                		decaddress += pow(16,((strlen(address)-2-i)))*num;
                	}
                	
                	printf("Decimal number is %d\n",decaddress);
                	readfromcache(cache,decaddress ,associativity);

           }
           else if(!strcmp(RW,"W")) 
           {
               
                for(i=2;i<strlen(address)-1;i++)
                	{
                		num=hextodec(address[i]);
                		decaddress += pow(16,((strlen(address)-2-i)))*num;
                	}
                	printf("Decimal number is %d\n",decaddress);
                	
                	writetocache(cache, decaddress ,associativity, write_policy);

           }
           
        }
           
    }
   printf("Hits : %d \n",cache->hits);
   printf("Misses : %d \n",cache->misses);
   printf("Read : %d \n",cache->reads);
   printf("Writes : %d \n",cache->writes);
   return 0;
}
struct Cache* makeCache(int cache_size,int block_size,int associativity,int write_policy)
{
   int i;
   struct Cache* cache;
 /**************  USE MALLOC TO CREATE THE CACHE  *******/
     cache= (struct Cache*) malloc( sizeof(struct Cache) );
     
  /*********************  INITIALIZE VARIABLES *******************/
    cache->cache_size=cache_size*1024*1024;
    cache->block_size=block_size*1024;
    cache->associativity=associativity;
    cache->write_policy=write_policy;
    cache->hits=0;
    cache->misses=0;
    cache->reads=0;
    cache->writes=0;
    cache->numblocks= (cache->cache_size/cache->block_size);
    cache->numlines = (cache->numblocks/associativity);
    
    printf(" Number of blocks %d\n",cache->numblocks);
    printf(" Number of Lines %d\n",cache->numlines);
    /*******************   Allocating memory to block *****************/
    cache->blocks = (struct Block**) malloc( sizeof(struct Block) * cache->numblocks);
    for(i = 0; i < cache->numblocks; i++)
    {
        cache->blocks[i] = (struct Block*) malloc( sizeof(struct Block) );
        cache->blocks[i]->valid = 0;
        cache->blocks[i]->dirty = 0;
        cache->blocks[i]->tag = 0;
    	cache->blocks[i]->LRU = 0;
    }

  return cache;
}


void writetocache(struct Cache* cache, int address, int associativity, int write_policy)
{
	int index,tag,offset;
    bool allblocksfilled=false,blockfound=false;
    
    index= ((address/cache->block_size) % cache->numlines);
    tag= (address/(cache->block_size* cache->numlines));
    printf(" TAG is %d\n",tag);
    printf(" Index is %d\n",index);
   

   
     for (offset=0;offset<associativity;offset++)
     {


     	if(cache->blocks[index+offset]->valid==1) 
    	   {

    	   	  if(cache->blocks[index+offset]->tag==tag)
    	   	  {
    	   	  	 
    	   	 	 blockfound=true;
    	 
    	   	     if(write_policy==1) //Write through
    	   			   cache->blocks[index+offset]->dirty=0;
    	   	     	
				 else //write-back
    	   	          cache->blocks[index+offset]->dirty=1;
    	   	     
    	   	     
    	   	     cache->blocks[index+offset]->valid=1;
    	   	     cache->blocks[index+offset]->LRU=0;
    	   	     printf("Hit Address in write is %d\n",address);
    	   	     cache->hits++;
    	   	     cache->writes++;
    	   	  	 break;
    	   	  }
    	   	  else
    	   	     cache->blocks[index+offset]->LRU++;
    	   }
    	   


      }

      if (blockfound==false)
        { 
	        
	         //checking if each block is filled up or not
        	
	         for (offset=0;offset<associativity;offset++)
	         {
	         	if(cache->blocks[index+offset]->valid==1)
	         		allblocksfilled=true;
	         	else
	         	{
	         		allblocksfilled=false;
	         		break;  //offset contains the relative offset where the new block could be placed
	         	}

	         }
	         if(allblocksfilled==true)  //LRU..replace the LRU block with this one
	         {
	         	int max=0;
	         	int pos=0;
	         	for (offset=0;offset<associativity;offset++)
	         	 {
	         	 	if(cache->blocks[index+offset]->LRU > max)
	         	 	{
	         	 		max=cache->blocks[index]->LRU;
	         	 		pos= offset;
	         	 	}
	         	 }
	         	
	         	//pos is the position of the block that needs to be replaced
	         	 cache->blocks[index+pos]->valid=1;
         		 cache->blocks[index+pos]->tag=tag;
     	      
             }
	 
	         else  //there is a place to keep this block
	         {
	         	//accessing memory and bring the block to this address
	         	cache->blocks[index+offset]->valid=1;
				cache->blocks[index+offset]->tag=tag;
	       
	         }

	        if(write_policy==1) //Write through
		   			   cache->blocks[index+offset]->dirty=0;
		   	     	
			 else //write-back
		   	          cache->blocks[index+offset]->dirty=1;
	       
	        printf(" Miss Address in write is %d\n",address);
	     	cache->writes++;
	     	cache->misses++;
	     	
     }
    
    
 }


void readfromcache(struct Cache* cache,int address,int associativity) 
{
    //Cache cache;
    int index,tag,offset;
    bool allblocksfilled=false,blockfound=false;
    index= ((address/cache->block_size) % cache->numlines);
    tag= (address/(cache->block_size* cache->numlines));
    
    for (offset=0;offset<associativity;offset++)
     {
     	if(cache->blocks[index+offset]->valid==1) 
    	   {
    	   	  if(cache->blocks[index+offset]->tag==tag)
    	   	  {
    	   	 	 blockfound=true;
    	   	 	 printf(" Hit Address in read is %d\n",address);
    	   	     cache->hits++;
    	   	     cache->reads++;
    	   	     cache->blocks[index+offset]->LRU=0;
    	   	  	 break;
    	   	  }
    	   	  else
    	   	     cache->blocks[index+offset]->LRU++;
    	   }
    
    
     }
    
    if (blockfound==false)
    {
        
         //checking if each block is filled up or not
         for (offset=0;offset<associativity;offset++)
         {
         	if(cache->blocks[index+offset]->valid==1)
         		allblocksfilled=true;
         	else
         	{
         		allblocksfilled=false;
         		break;  //offset contains the relative offset where the new block could be placed
         	}

         }
         if(allblocksfilled==true)  //LRU..replace the LRU block with this one
         {
         	int max=0;
         	int pos=0;
         	for (offset=0;offset<associativity;offset++)
         	 {
         	 	if(cache->blocks[index+offset]->LRU > max)
         	 	{
         	 		max=cache->blocks[index]->LRU;
         	 		pos= offset;
         	 	}
         	 }   	
         	//pos is the position of the block that needs to be replaced
         	cache->blocks[index+pos]->valid =1;
         	cache->blocks[index+pos]->tag =tag;
         	cache->blocks[index+pos]->dirty =0;
         	
         	
         }
 
         else  //there is a place to keep this block
         {
         	//accessing memory and bring the block to this address
         	cache->blocks[index+offset]->valid=1;
         	cache->blocks[index+offset]->tag=tag;
         	cache->blocks[index+offset]->dirty =0;
         }
         printf(" Miss Address in read is %d\n",address);
         cache->reads++;
         cache->writes++;
         cache->misses++;
     
    }
}

