//#include "Config.h"
#include "string.h"
#include <stdlib.h>
//#include "bluetooth.h"
//using namespace std;

class BTparser {
private:
	char code[5]="";
	char val1[5]=""; 
	char val2[5]="";
	char val3[5]="";
	char val4[5]="";
	char val5[5]="";
	char val6[5]="";

public:
	char * getCode();
	float getVal1();	
	float getVal2();	
	float getVal3();
	float getVal4();
	float getVal5();
	float getVal6();

	void parseString(char * in);
};
