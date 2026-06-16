/*
	=== Vincebus's STRING helpers ===
		version 0.5 - forever WIP !
	=================================

	Use them at your own risk !
	Jun 2026.

	strsplit
	strinv
	strjoinint
	strjoinfloat
	strjoinfloat
	strjoindouble
	strjoinstr
	strjoin
	strltrim
	strrtrim
	strtrim
	strstrip
	strnslice
*/

#include "STR.H"

// 
#define TYPE_INT 0
#define TYPE_CHAR 1
#define TYPE_FLOAT 2
#define TYPE_DOUBLE 3
#define TYPE_STR 4

// Splits string into array of string by a separator AND null terminated
char *strsplit(char *str, const char *separator){
	return NULL;
}

// Reverses an string
char *strinv(char *str){
	size_t len;
	char *newstr = NULL;
	char *stroffset;
	char *strptr;
	char *strbuff = NULL;

	if(!str) return NULL;

	newstr = strdup(str);

	if(!newstr) return NULL;
	
	strptr = newstr;
	
	len = strlen(newstr);
	strbuff = strdup(newstr);
	stroffset = newstr + len - 1;

	while((*strbuff) != '\0'){
		*(stroffset--) = *(strbuff++);
	}

	free(strbuff);

	return newstr;
}

// Joins array into a string by a separator
char *strjoinint(int *arr, size_t arrlen, char *separator){
	char *newstr = NULL;
	size_t len = 0;
	int i;
	char numbuff[48];
	int numlen;

	if(!separator) return NULL;

	// We count the number of chars so we can get 
	// the total length for allocating our new string
	for(i=0; i<arrlen; i++){
		memset(numbuff, '\0', 48);
		sprintf(numbuff, "%d%s", arr[i], separator);
		numlen = strlen(numbuff);
		len += numlen;
	}

	newstr = (char*)malloc(sizeof(char)*len);

	if(!newstr) return NULL;

	memset(newstr, '\0', len);
	memset(numbuff, '\0', 48);

	// We start appeding
	for(i=0; i<arrlen;i++){
		if(i<arrlen-1){
			sprintf(numbuff, "%d%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%d", arr[i]);
		}

		strcat(newstr, numbuff);
	}

	return newstr;
}

// Joins array into a string by a separator
char *strjoinfloat(float *arr, size_t arrlen, char *separator){
	char *newstr = NULL;
	size_t len = 0;
	int i;
	char numbuff[64];
	int numlen;

	if(!separator) return NULL;

	// We count the number of chars so we can get 
	// the total length for allocating our new string
	for(i=0; i<arrlen; i++){
		memset(numbuff, '\0', 64);
		if(i<arrlen-1){
			sprintf(numbuff, "%.5f%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%.5f", arr[i]);
		}
		numlen = strlen(numbuff);
		len += numlen;
	}

	newstr = (char*)malloc(sizeof(char)*len);

	if(!newstr) return NULL;

	memset(newstr, '\0', len);
	memset(numbuff, '\0', 64);

	// We start appeding
	for(i=0; i<arrlen;i++){
		if(i<arrlen-1){
			sprintf(numbuff, "%.5f%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%.5f", arr[i]);
		}

		strcat(newstr, numbuff);
	}

	return newstr;
}

char *strjoindouble(double *arr, size_t arrlen, char *separator){
	char *newstr = NULL;
	size_t len = 0;
	int i;
	char numbuff[64];
	int numlen;

	if(!separator) return NULL;

	// We count the number of chars so we can get 
	// the total length for allocating our new string
	for(i=0; i<arrlen; i++){
		memset(numbuff, '\0', 64);
		if(i<arrlen-1){
			sprintf(numbuff, "%.5f%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%.5f", arr[i]);
		}
		numlen = strlen(numbuff);
		len += numlen;
	}

	newstr = (char*)malloc(sizeof(char)*len);

	if(!newstr) return NULL;

	memset(newstr, '\0', len);
	memset(numbuff, '\0', 64);

	// We start appeding
	for(i=0; i<arrlen;i++){
		if(i<arrlen-1){
			sprintf(numbuff, "%.5f%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%.5f", arr[i]);
		}

		strcat(newstr, numbuff);
	}

	return newstr;
}

// Joins array of pointed strings into a string by a separator
char *strjoinstr(char **arr, size_t arrlen, char *separator){
	char *newstr = NULL;
	size_t len = 0;
	int i;
	size_t bufflen;

	if(!separator) return NULL;

	// We count the number of chars so we can get 
	// the total length for allocating our new string
	for(i=0; i<arrlen; i++){
		if(!arr[i]) continue;

		bufflen += (strlen(arr + i) + strlen(separator));
	}

	newstr = (char*)malloc(sizeof(char)*bufflen);

	if(!newstr) return NULL;

	memset(newstr, '\0', bufflen);

	bufflen = 0;
	// We start appeding
	for(i=0; i<arrlen;i++){
		if(!arr[i]) continue;

		if(i<arrlen-1){
			bufflen += sprintf(newstr + bufflen, "%s%s", arr[i], separator);
		}else{
			bufflen += sprintf(newstr + bufflen, "%s", arr[i]);
		}
	}

	return newstr;
}

char *strjoin(void *arr,unsigned char type, size_t arrlen,  char *separator){
	if(!arr) return NULL;
	if(!separator) return NULL;

	switch(type){
		case TYPE_INT:
			return strjoinint((int*)arr, arrlen, separator);
		case TYPE_CHAR:
			return (char*)arr;	// lol
		case TYPE_FLOAT:
			return strjoinfloat((float*)arr, arrlen, separator);
		case TYPE_DOUBLE:
			return strjoindouble((double*)arr, arrlen, separator);
		case TYPE_STR:
			return strjoinstr((char*)arr, arrlen, separator);
		default:
			return NULL;
	}

	return NULL;
}

// Removes left spaces
char *strltrim(char *str){
	return NULL;
}

// Removes right spaces
char *strrtrim(char *str){
	return NULL;
}

// Removes spaces from both sides
char *strtrim(char *str){
	return NULL;
}

// Iterative version
char *strstrip(char *str) {
	char *newstr = NULL;
	char *src = NULL;
	char *dst = NULL;

    if (!str) return NULL;
	
	newstr = strdup(str);

    if (!newstr) return NULL;

    src = newstr;
    dst = newstr;

    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';

	return newstr;
}

// Slice between a range in a string
char *strnslice(char *str, size_t from, size_t step, size_t strlen ){
	char *newstr = NULL;
	
	if(from + step > strlen) step = strlen - from - 1;

	newstr = (char*)malloc(sizeof(char)*step+1);

	memset(newstr, '\0', step+1);	

	strncpy(newstr, str + from, step);

	return newstr;
}

//	
//	Join

// Splice
#ifdef STANDALONE

int main(){
	char str[64] = "This is another string";
	char *renastr = "WHo's Renamon?";
	int testnums[11] = {10, 25,40, 8, 1, 3, 0 ,666, -5, -5999};
	float testfloats[4] = { 1.8, 4.32, 7.14, 83.33 };
	double testdoubles[4] = { 0.25, 3.14693, 16.666, 89.362 };

	char *teststrs[5];

	char *strLong = "\tLong One";
	char *strLong2 = "Another one, but well\t\t";
	char *strLong3 = "          Both sides   ";
	
	teststrs[0] = strdup("This is a string");
	teststrs[1] = strdup("This is another");
	teststrs[2] = strdup("An another");
	teststrs[3] = strdup("Farts");
	teststrs[4] = strdup("Foobar");

	// SAMPLE STR
	printf("\n%s", str);
	
	// INVERT
	printf("\n%s", 	strinv(str));

	// SLICE 
	printf("\n%s", strnslice(renastr, 6,7, strlen(renastr)));
	
	// JOIN
	printf("\n[%s]", strjoinint(testnums, 9, ", "));
	printf("\n[%s]", strjoin(teststrs, TYPE_STR, 5, ", "));
	printf("\n[%s]", strjoin(testfloats, TYPE_FLOAT, 4, ", "));
	printf("\n[%s]", strjoin(testdoubles, TYPE_DOUBLE, 4, ", "));

	free(teststrs[0]);
	free(teststrs[1]);
	free(teststrs[2]);
	free(teststrs[3]);
	free(teststrs[4]);
	free(teststrs);

	return 0;
}

#endif