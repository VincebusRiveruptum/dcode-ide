#ifndef ED_PRIVATE_H
#define ED_PRIVATE_H

struct Line;
struct File;
struct Node;

int _get_tab_counts_until(int col);
int _get_tab_counts_someline(struct Line *someLine, int col);
int _get_auto_close_pos();

// New line handling
struct Line *_createLine(struct File *file);
struct Node *_createLineNode(struct File *file);
struct Node *_resolveNewLine(struct File *file);

int _calculateVisualOffset(int col);
int _calculateTabCount();
int _calculateTabStart();
void _updateCurrentCursorY();
void _updateCurrentCursorX();

#endif
