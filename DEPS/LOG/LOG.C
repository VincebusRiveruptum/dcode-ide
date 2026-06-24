/*

    Audit module    -   Vincebus Riveruptum

    - Error logging
    - Activity registering

*/
#include "LOG.H"

unsigned char log_enable = 1;

FILE *logFp = NULL;

void log_init(){
    if(log_enable != 1) return;
    
    logFp = fopen("logs.txt", "a+"); // Start fresh on every run
    if(!logFp){
        printf("\n[log_init]: FATAL ERROR: Could not open logs.txt for writing.\n");
    }
}

void log_shutdown(){
    if(log_enable != 1) return;

    if(logFp){
        fclose(logFp);
        logFp = NULL;
    }
}

void logToFile(char *outputString){
    if(log_enable != 1) return;

    if(logFp){
        fputs(outputString, logFp);
        fflush(logFp); // Ensure data is written even if app crashes
    }
}

void logToConsole(char *outputString){
    if(log_enable != 1) return;

    printf("%s", outputString);
}

void logger(const char *format, ...){ // Modified signature for variadic arguments
    char *logType = NULL;
    char userMessage[1024]; // Increased buffer
    char logString[2048];  // Increased buffer
    char dateString[64];   
    va_list args;          
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    if(log_enable != 1) return;

    va_start(args, format);
    vsprintf(userMessage, format, args); 
    va_end(args);

    logType = (char*)getEnv("LOGS", "file");
    
    sprintf(dateString, "[%04d-%02d-%02d %02d:%02d:%02d]", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    {
        char *p = userMessage;
        while(*p == '\n' || *p == '\r') p++; 
        sprintf(logString, "%s %s\n", dateString, p);
    }

    if(logType){
        if(strcmp(logType, "file") == 0 || strcmp(logType, "fileConsole") == 0){
            logToFile(logString);
        }
        if(strcmp(logType, "console") == 0 || strcmp(logType, "fileConsole") == 0){
            logToConsole(logString);
        }       
    }else{
        logToFile(logString);
    }
}

#ifdef STANDALONE
int main(){
    printf("\nThis module is not standalone, but this main placeholder is for testing purposes.");
    return 0;
}
#endif

