#ifndef F_PRIVATE_H
#define F_PRIVATE_H

int _checkAvailableName();
void _splitIntoLines(char *buffer, size_t bufferLength, struct File *file);
size_t _copyLines(struct File *old, struct File *new);
int _goBackPath(char *path);
bool _isDefaultFileName();

#endif