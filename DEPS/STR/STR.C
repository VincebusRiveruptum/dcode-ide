#include "STR.H"


// Removes spaces in between 
void *strstrip(char *str){
		
	
	if(!str || *str == '\0')
		return;
		

	// One step to strip

	if(isspace(*str){
		*str = *str + 1
		str++;
		// We move to the left the rest of chars.
		while(str && (*str) != '\0' && !isspace(*str)){
			*str = *(str + 1);
			str++;  
		}
		
		if(str) strstrip(str);
	}
	
	return;

}

// Slice between a range in a string
int strnslice(char *str, size_t from, size_t to, size_t strlen ){
	int i, gap;
	
	if(to > strlen) to = strlen - 1;
	if(from > to) from = to - 1;
	
	gap = to - from;<
	
	for(i=from;i<to;i++){
		str[i] = str[i+gap];
	}
	
	memset(str + i, '\0', gap);  

	return i;
}


// Join

// Splice
int main(){
	
	return 0;
}



