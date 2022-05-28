#include "stdinout.h"

// ----------------------------------------------------------
// print: Imprime el String que le pase
//        Utiliza: strlength()   
// ----------------------------------------------------------
// Argumentos:
//      1. El string a imprimir
// ----------------------------------------------------------
void printColor(char * string, char color) {
	if (string[0]) {
		int len = strlength(string);
		system_write(STDOUT, string, len, color);
	}
    	
}

void print(char * string) {
	printColor(string,GREY);
}



int strlength(char * string) {
	int i;
	for (i=0 ; string[i] ; i++);
	return i;
}

int strcat(char * target, char * source) {
   int c, d;
   
   c = 0;
 
   while (target[c] != '\0') {
      c++;      
   }
 
   d = 0;
 
   while (source[d] != '\0') {
      target[c] = source[d];
      d++;
      c++;    
   }
 
   target[c] = '\0';
   return c;
}



void swap(char* a, char* b) {
	char aux = *a;
	*a = *b;
	*b = aux;
}
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(str+start, str+end);
        start++;
        end--;
    }
}
 
// Implementation of itoa()
int numToStr(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    if (num == 0)
    {
        str[i++] = '0';
        return 1;
    }
 
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
 
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    if (isNegative)
        str[i++] = '-';
 

    reverse(str, i);
 
    return i;
}

char * itoa(int num, char* str, int base) {
    int len = numToStr(num, str, base);
    str[len] = 0;
    return str;
}



int power(int num, int p) {
	int res = 1;
	for(int i = 0; i < p; i++) {
		res *= 10;
	}
	return num*res;
}


int strToInt(char * buff, int len) {
	int res = 0;
	for(int i = 0; i < len; i++) {
		if (buff[i] < '0' || buff[i] > '9') {
			return -1;
		}
		res += power(buff[i]-'0', len-i-1);
	}
	return res;
}

int atoi(char * s) {
	return strToInt(s,strlength(s));
}

 
int get_digit(char c, int digits_in_base)
{
    int max_digit;
    if (digits_in_base <= 10)
        max_digit = digits_in_base + '0';
    else
        max_digit = digits_in_base - 10 + 'a';
 
    if (c >= '0' && c <= '9' && c <= max_digit)
        return (c - '0');
    else if (c >= 'a' && c <= 'f' && c <= max_digit)
        return (10 + c - 'a');
    else
        return (-1);
}
 
int atoi_base(const char *str, int str_base)
{
    int result = 0;
    int sign = 1;
    int digit;
 
    if (*str == '-')
    {
        sign = -1;
        ++str;
    }
 
    while ((digit = get_digit(toLower(*str), str_base)) >= 0)
    {
        result = result * str_base;
        result = result + (digit * sign);
        ++str;
    }
    return (result);
}


int isDigit(char c) {
    return c <= '9' && c >= '0';
}


int charToDigit(char c) {
    if(isDigit(c)) {
		return c - '0';
	}
	return -1;
}


char toLower(char c) {
    if(c >= 'A' && c <= 'Z') {
        c = c + ('a'-'A');
    }
    return c;
}

char toUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c + ('A' - 'a');
    }
    return c;
}



int strcmp(char *X, char *Y)
{
    while (*X)
    {

        if (*X != *Y) {
            break;
        }
 

        X++;
        Y++;
    }
 

    return *(const unsigned char*)X - *(const unsigned char*)Y;
}


char * strcpy(char* destination, char* source)
{

    if (destination == NULL) {
        return NULL;
    }
 
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return source;
}


int getCharSys() {
    return get_char(1);
}

int getRealChar() {
    return get_char(0);
}

int putCharColor(char c, char color) {
	if (!c)
		return 0;
    return system_write(STDOUT,&c,1,color);
}

int putChar(char c) {
	return putCharColor(c, GREY);
}

char intToChar(unsigned int num) {
	if (num > 9)
		return -1;
	return '0' + num;
}

int split(char * buf,char c, char * target[]) {
	int j=0, flag = 1;
	for (int i=0 ; buf[i] ; i++) {
		if (buf[i]==c) {
			buf[i] = 0;
            flag = 1;
		}
		else if (flag){
			target[j++] = &buf[i];
            flag = 0;
		}
	}
	return j;
}

long secondsElapsed() {
	return timer_tick(0)/18;
}

int ticks() {
	return timer_tick(0);
}

