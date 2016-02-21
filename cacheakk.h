


typedef int bool;
#define true 1
#define false 0
#define LINES 900

struct Cache* makeCache(int cache_size,int block_size,int associativity,int write_policy);

void writetocache(struct Cache* cache, int address, int associativity, int write_policy);

void readfromcache(struct Cache* cache,int address,int associativity) ;

int hextodec(char hex)
{


  int dec;
  //printf(" Character is %c ",hex);
  
  switch(hex)
  {
  	case'a':
	case'A':
			 dec = 10;
			break;
	case'b':
	case'B':
			dec = 11;
			break;
	case'c':
	case'C':
			dec = 12;
			break;
	case'd':
	case'D':
			dec = 13;
			break;
	case'e':
	case'E':
			dec = 14;
			break;
	case'f':
	case'F':
			dec = 15;
			break;
	case'0':
			dec = 0;
			break;
	case'1':
			dec = 1;
			break;
	case'2':
			dec = 2;
			break;
	case'3':
			dec = 3;
			break;
	case'4':
			dec = 4;
			break;
	case'5':
			dec = 5;
			break;
	case'6':
			dec = 6;
			break;
	case'7':
			dec = 7;
			break;
	case'8':
			dec = 8;
			break;
	case'9':
			dec=9;
			break;
	default:
		     dec = 0;
  }
  
  return dec;
}

