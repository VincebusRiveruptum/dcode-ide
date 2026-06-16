#include "STR.H"

// 
#define TYPE_INT 0
#define TYPE_CHAR 1
#define TYPE_FLOAT 2
#define TYPE_STR 3

// Splits string into array of string by a separator
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
char *strjoindouble(double *arr, size_t arrlen, char *separator){
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
		sprintf(numbuff, "%f%s", arr[i], separator);
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
			sprintf(numbuff, "%f%s", arr[i], separator);
		}else{
			sprintf(numbuff, "%d", arr[i]);
		}

		strcat(newstr, numbuff);
	}

	return newstr;
}

char *strjoin(void *arr, unsigned char type, char *separator){
	if(!arr) return NULL;
	if(!separator) return NULL;

	switch(type){
		case TYPE_INT:
			return strjoinint((int*)arr, 10, separator);
		case TYPE_CHAR:
			return strjoinint((char*)arr, 20, separator);
		case TYPE_FLOAT:
			return strjoinint((float*)arr, 20, separator);
		case TYPE_STR:
			return strjoinint((char*)arr, 20, separator);
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
	int testnums[9] = {10, 25,40, 8, 1, 3 ,666, -5, -5999};

	printf("\n%s", str);
	
	printf("\n%s", 	strinv(str));
	
	printf("\n%s", strnslice(renastr, 6,7, strlen(renastr)));
	
	printf("\n%s", strjoinint(testnums, 9, ", "));

	return 0;
}

#endif