#ifndef COUNT_H
#define COUNT_H

extern bool detail_flag;
extern bool recursion_flag;

char* get_current_dir(void);
long long count_lines_in_dir(const char *dir);

#endif
