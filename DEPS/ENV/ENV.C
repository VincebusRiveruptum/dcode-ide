/* ENV/CFG handling by Vincebus - 2026

-- JUNE Revision ---

As in August 2025. the Configuration entries are hardcoded and if you want more
settings you have to modify the Config data structure, displayConf and the
loadEnv method.

*/

/*
  - Config should be a global variable
  - Should be a 2d

*/
#include "ENV.H"

Config *config = NULL;

int findIndex(char *str) {
  int i = 0;
  int index = 0;
  int length;

  if (!str)
    return -1;

  length = strlen(str);

  for (i = 0; i < length; i++) {
    if (str[i] == '=')
      return i;
  }

  return -1;
}

bool findValue(const char *key, const char *line) {
  int keyLen = 0;

  if (!key || !line)
    return false;

  keyLen = strlen(key);

  return (strncmp(line, key, keyLen) == 0 && line[keyLen] == '=') ? true : false;
}

char *getConfigKey(char *line) {
  int index = 0;
  char *key; 
  if (!line)
    return NULL;

  if (((index = findIndex(line)) == -1))
    return NULL;

  key = (char *)malloc(index + 1);
  if (!key) return NULL;
  strncpy(key, line, index);
  key[index] = '\0';

  return key;
}

char *getConfigValue(char *line) {
  int index = 0;
  char *val; 
  int valLen;
  if (!line)
    return NULL;

  if (((index = findIndex(line)) == -1))
    return NULL;

  valLen = strlen(line) - index - 1;
  val = (char *)malloc(valLen + 1);
  if (!val) return NULL;
  strncpy(val, line + index + 1, valLen);
  val[valLen] = '\0';
  
  // Strip trailing newline/carriage return
  while (valLen > 0 && (val[valLen-1] == '\n' || val[valLen-1] == '\r')) {
      val[--valLen] = '\0';
  }

  return val;
}

bool isfloat(const char *str) {
    bool has_digit = false;
    bool has_dot = false;
    int i = 0;

    if (str == NULL || *str == '\0') {
        return false;
    }
    
    // Check for optional sign
    if (str[i] == '+' || str[i] == '-') {
        i++;
    }
    
    // Check digits and decimal point
    while (str[i] != '\0') {
        if (isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) {
                return false;  // Multiple dots
            }
            has_dot = true;
        } else {
            return false;  // Invalid character
        }
        i++;
    }
    
    return has_digit;
}

unsigned char getConfigType(char *value) {
  if (!value)
    return NULL;

  if (isdigit(value[0]))
    return ENV_TYPE_INT;

  if (
    strcmp(value, "true") == 0 ||
    strcmp(value, "True") == 0 ||
    strcmp(value, "TRUE") == 0 ||
    strcmp(value, "false") == 0 ||
    strcmp(value, "False") == 0 ||
    strcmp(value, "FALSE") == 0
  )
    return ENV_TYPE_BOOL;

  if (isfloat(value))
    return ENV_TYPE_FLOAT;

  if(isalpha(value[0]))
    return ENV_TYPE_STRING;

  return NULL;
}

Config *loadEnv() {
  // Look for a .env file in the current directory
  // Parse the contents of the .env
  // Return the Config type object

  FILE *fp = fopen(ENV_FILENAME, "r");
  char tmpBuffer[256];
  char *key;
  char *value;
  unsigned char type;
  int i = 0;

  log_init();
  
  if (!fp) {
    logger("[loadEnv]: %s not found, trying %s", ENV_FILENAME, CFG_FILENAME);
    fp = fopen(CFG_FILENAME, "r");
  }

  if (!fp) {
    logger("[loadEnv]: No config file found (%s or %s)", ENV_FILENAME, CFG_FILENAME);
    return NULL;
  }

  // We get the numeber of attributes in the config file
  while (fgets(tmpBuffer, sizeof(tmpBuffer), fp) != NULL) {
    i++;
  }
  
  config = (Config *)malloc(sizeof(Config));
  if(!config) return NULL;

  config->entries = (ConfigEntry *)malloc(sizeof(ConfigEntry) * i);
  if(!config->entries) {
    free(config);
    return NULL;
  }
  config->length = i;
  
  // We reset the file pointer to the beginning
  i=0;
  rewind(fp);
  
  // We parse the config file
  while (fgets(tmpBuffer, sizeof(tmpBuffer), fp) != NULL) {
    // ignore comments
    if (tmpBuffer[0] == '#' || tmpBuffer[0] == '\n' || tmpBuffer[0] == '\r') continue;
    
    key = getConfigKey(tmpBuffer);
    if (!key) continue;
    
    value = getConfigValue(tmpBuffer);

    if (!value) {
        free(key);
        continue;
    }
    
    type = getConfigType(value);

    // Add to global configEntries
    //    config->entries = realloc(config->entries, sizeof(ConfigEntry) * (i + 1));
    config->entries[i].key = key;
    config->entries[i].value = value;
    config->entries[i].type = type;
    i++;
  }

  config->length = i;
  fclose(fp);

  return config;
}


void freeConf(Config *conf) {
  int i=0;
  if (!conf) return;

  for(i=0; i< conf->length; i++) {
    free(conf->entries[i].key);
    free(conf->entries[i].value);
  }
  free(conf->entries);
  free(conf);
}

char *getEnv(char *key, const char *defaultValueFormat, ...){
  int i=0;
  char defvalue[255] = {'\0'};
  char *value = NULL;
  
  va_list args;

  if(config){ 
    for(i=0; i< config->length; i++) {
      if (config->entries[i].key && strcmp(config->entries[i].key, key) == 0) {
        return config->entries[i].value;
      }
    }
  }

  if(defaultValueFormat){
    va_start(args, defaultValueFormat);
    vsprintf(defvalue, defaultValueFormat, args); 
    va_end(args);
    
    value = defvalue;

  }else{
    value = "";
  }
  
  if(value == defvalue){
    return strdup(defvalue);  
  }else{
    return value;
  }

  return value;
}

#ifdef STANDALONE

void displayConf(Config *conf) {
  int i=0;

  printf("\nConfiguration content:\n");

  for(i=0; i< conf->length; i++) {
    switch(conf->entries[i].type){
      case ENV_TYPE_INT:
        printf("%s: %s [INT]\n", conf->entries[i].key, conf->entries[i].value); 
        break;
      case ENV_TYPE_FLOAT:
        printf("%s: %s [FLOAT]\n", conf->entries[i].key, conf->entries[i].value); 
        break;
      case ENV_TYPE_BOOL:
        printf("%s: %s [BOOL]\n", conf->entries[i].key, conf->entries[i].value); 
        break;
      case ENV_TYPE_STRING:
        printf("%s: %s [STRING]\n", conf->entries[i].key, conf->entries[i].value); 
        break;
    }
  }
}

int main() {
  printf("\n\n.ENV/CFG File reader");
  printf("\nVincebus Riveruptum, 2025.");

  config = loadEnv();

  if (config == NULL) {
    printf("\nNo ENV/CFG file found!.");
    return 0;
  }
  
  displayConf(config);

  freeConf(config);

  return 0;
}
#endif
