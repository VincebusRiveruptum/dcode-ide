/*
        === Vincebus's STRING helpers ===
                version 1 - forever WIP !
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

// Splits string into array of string pointers by a separator
// This array is null terminated
char **strsplit(char *str, const char *separator){
        char **arr = NULL;
        char *strStart = NULL;
        char *strEnd = NULL;
        size_t sepLen = 0;
        size_t sepCount = 0;
        size_t strFoundLen = 0;
        int i = 0;
        int j = 0;

        if(!str) return NULL;
        if(!separator) return NULL;

        sepLen = strlen(separator);
        if(sepLen == 0) return NULL;

        // Count separators found.
        strStart = str;
        while((strStart = strstr(strStart, separator)) != NULL){
                sepCount++;
                strStart += sepLen;
        }
        
        // Allocate string pointer array plus NULL terminator.
        arr = (char**)malloc(sizeof(char*) * (sepCount + 2));
        
        if (!arr) return NULL;

        strStart = str;
        
        for(i=0; i <= sepCount; i++){
			strEnd = strstr(strStart, separator);
			if (strEnd) {
					strFoundLen = strEnd - strStart;
			} else {
					strFoundLen = strlen(strStart);
			}
			
			arr[i] = (char*)malloc(strFoundLen + 1);

			if(!arr[i]){
					for(j = 0; j < i; j++){
							free(arr[j]);
					}
					free(arr);
					return NULL;
			}

			strncpy(arr[i], strStart, strFoundLen);
			arr[i][strFoundLen] = '\0';
			
			if (strEnd) {
					strStart = strEnd + sepLen;
			}
        }

        // NULL-terminated pointer array.
        arr[sepCount + 1] = NULL;

        return arr;
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
        int numlen = 0;

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
        size_t bufflen = 0;

        if(!separator) return NULL;

        // We count the number of chars so we can get 
        // the total length for allocating our new string
        for(i=0; i<arrlen; i++){
                if(!arr[i]) continue;

                bufflen += (strlen(arr[i]) + strlen(separator));
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
                        return (char*)arr;      // lol
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

// Removes right spaces
char *strltrim(char *str){
        size_t len;
        char *strptr;
        char *newstr;

        if(!str) return NULL;

        strptr = str;

        while((*strptr) != '\0' && isspace((unsigned char)*strptr)) {
                strptr++;
        }

        len = strlen(strptr);

        newstr = (char*)malloc(len);

        if (!newstr) return NULL;

        memset(newstr, '\0', len);
        strncpy(newstr, strptr, len);

        return newstr;
}

// Removes left spaces
char *strrtrim(char *str){
        size_t len;
        char *strptr;
        char *newstr;

        if(!str) return NULL;

        len = strlen(str) + 1;
        strptr = str + len ;

        while(strptr && (isspace((unsigned char)*strptr) || (*strptr) == '\0')) {
                strptr--;
        }

        len = strptr - str + 1 ;

        newstr = (char*)malloc(len);
        memset(newstr, '\0', len);
        strncpy(newstr, str, len);

        *(newstr + len) = '\0';

        return newstr;
}


// Removes spaces from both sides
char *strtrim(char *str){
        size_t len;
        char *start, *end, *newstr;

        if(!str) return NULL;

        start = str;

        while((*start) != '\0' && isspace((unsigned char)*start)) {
                start++;
        }

        end = str + strlen(str);

        if(end == start) return str;

        while(end && (isspace((unsigned char)*end) || (*end) == '\0')) {
                end--;
        }

        len = end - start + 1;

        newstr = (char*)malloc(len);
        memset(newstr, '\0', len);
        strncpy(newstr,start, len);

        return newstr;
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
//      Join

// Splice
#ifdef STANDALONE

int main(){
        char str[64] = "This is another string";
        char *renastr = "WHo's Renamon?";
        char *renastr2 = "Renamon, is, a, furry, fox, digimon, known, in, the, furry, community.";
        char **arrStr2 = NULL;

        int i = 0;
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
        printf("\n%s",  strinv(str));

        // SLICE 
        printf("\n%s", strnslice(renastr, 6,7, strlen(renastr)));
        
        // JOIN
        printf("\n[%s]", strjoinint(testnums, 9, ", "));
        printf("\n[%s]", strjoin(teststrs, TYPE_STR, 5, ", "));
        printf("\n[%s]", strjoin(testfloats, TYPE_FLOAT, 4, ", "));
        printf("\n[%s]", strjoin(testdoubles, TYPE_DOUBLE, 4, ", "));
        
        // Trim
        printf("\n{%s}", strltrim(strLong));
        printf("\n{%s}", strrtrim(strLong2));
        printf("\n{%s}", strtrim(strLong3));

        arrStr2 = strsplit(renastr2, ", ");

        i = 0;
        while(arrStr2[i] != NULL){
                printf("\n%s", arrStr2[i]);
                i++;
        }

        free(teststrs[0]);
        free(teststrs[1]);
        free(teststrs[2]);
        free(teststrs[3]);
        free(teststrs[4]);

        // Free the array of strings returned by strsplit
        i = 0;
        while(arrStr2[i] != NULL){
                free(arrStr2[i]);
                i++;
        }
        free(arrStr2);

        return 0;
}

#endif
