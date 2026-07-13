#ifndef ED_PRIVATE_H
#define ED_PRIVATE_H

struct Line;

int _get_tab_counts_until(int col);
int _get_tab_counts_someline(struct Line *someLine, int col);
int _get_auto_close_pos();
int _calculateVisualOffset(int col);
int _calculateTabCount();
int _calculateTabStart();
void _updateCurrentCursorY();
void _updateCurrentCursorX();

#endif
