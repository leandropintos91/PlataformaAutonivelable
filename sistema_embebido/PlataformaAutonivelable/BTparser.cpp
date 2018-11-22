#include "BTparser.h"
#include "arduino.h"

char * BTparser::getCode()	{return code; }
float BTparser::getVal1()	{return (float)atof(val1);}
float BTparser::getVal2()	{return (float)atof(val2);}	
float BTparser::getVal3()	{return (float)atof(val3);}
float BTparser::getVal4()	{return (float)atof(val4);}
float BTparser::getVal5()	{return (float)atof(val5);}
float BTparser::getVal6()	{return (float)atof(val6);}

void BTparser::parseString(char * in)
{
	int i=0;
	int j=0;
	int f=0;
	char aux[40];
	while (!strcmp(in+i, "\r\n") || *(in+i) != '\0')
	{	
		if (*(in+i) != 32) //32 es el ascii de el caracter espacio
		{
			switch(f)
			{
				case 0:
					*(code+j)=*(in+i);
					j++;
					break;
				case 1:
					*(val1+j)=*(in+i);
					j++;
					break; 		
				case 2:
					*(val2+j)=*(in+i);
					j++;
					break; 		
				case 3:
					*(val3+j)=*(in+i);
					j++;
					break;
				case 4:
					*(val4+j)=*(in+i);
					j++;
					break;
				case 5:
					*(val1+j)=*(in+i);
					j++;
					break;
				case 6:
					*(val1+j)=*(in+i);
					j++;
					break; 	
			}

		}
		else
		{
			f++;
			j = 0;
		}

		i++;

    Serial.print("|");

	}
}
		
