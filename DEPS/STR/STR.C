#include "STR.H"

// Iterative version
void strstrip(char *str) {
	char *src = NULL;
	char *dst = NULL;

    if (!str) return;

    src = str;
    dst = str;

    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

// Removes spaces in between 
// This approach needs to be refactored.. long string could
// overflow the stack.

void strstrip_old(char *str){
	char *auxptr;

	if(!str || *str == '\0')
		return;
		
	// One step to strip
	if(isspace(*str)){
		// We move to the left the rest of chars.
		auxptr = str;

		while(*auxptr != '\0'){
			*auxptr = *(auxptr + 1);
			auxptr++;  
		}

		strstrip(str);
	}else{
		strstrip(str+1);
	}
	
	return;
}

// Slice between a range in a string
int strnslice(char *str, size_t from, size_t to, size_t strlen ){
	int i, gap;
	
	if(to > strlen) to = strlen - 1;
	if(from > to) from = to - 1;
	
	gap = to - from;
	
	for(i=from;i<to;i++){
		str[i] = str[i+gap];
	}
	
	memset(str + i, '\0', gap);  

	return i;
}


// Join

// Splice
#ifdef STANDALONE

int main(){
	char str[64] = "This is another string";

	printf("\n%s", str);

	strstrip(str);

	printf("\n%s", str);

	return 0;
}

#endif