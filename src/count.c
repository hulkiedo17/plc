#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include "../include/check.h"
#include "../include/count.h"
#include "../include/misc.h"

bool detail_flag = false;
bool recursion_flag = true;
bool config_and_shell_files_flag = false;
bool count_without_spaces = false;
bool clean_output = false;
//static long long max_count = 0;

/* STATIC FUNCTIONS */

static char* concat_name_and_path(const char *path, char *name)
{
    size_t len = 0, len_path;
    char* full_path = NULL;

    len_path = strlen(path) + 1;
    len = len_path + strlen(name) + 1;
    full_path = malloc(len * sizeof(char));
    if(full_path == NULL) {
        fail(stderr, "concat_name_and_path(): memory allocation error (malloc() failed)\n");
    }

    strcpy(full_path, path);
    if(path[len_path - 2] != '/') {
        strcat(full_path, "/");
    }
    strcat(full_path, name);

    return full_path;
}

/*
static void print_path_and_count(const char *path, long long count) 
{
    const char *temp_pointer = clean_output ? (path + 2) : path;
    printf("%4lld = %s\n", count, temp_pointer);
}*/

static long long count_lines_in_file(const char *path)
{
    FILE *fp = NULL;
    long long count = 0;
    char buffer[BUFSIZE];

    if(path == NULL) {
        fail(stderr, "count_lines_in_file(): the function argument was passed the NULL value (const char *path)\n");
    }

    fp = fopen(path, "r");
    if(fp == NULL) {
        fail(stderr, "count_lines(): an error occurred while opening file (%s)\n", path);
    }

    while(fgets(buffer, BUFSIZE, fp) != NULL) {
        if(strcmp(buffer, "\n") == 0) {
            if(!count_without_spaces) {
                count++;
            }
        } else {
            count++;
        }
    }

    if(detail_flag) {
        //print_path_and_count(path, count);
        const char *temp_ptr = clean_output ? (path + 2) : path;
	printf("%lld = %s\n", count, temp_ptr);
    }

    fclose(fp);
    return count;
}

/* NON-STATIC FUNCTIONS */

long long count_lines_in_dir(const char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char *full_path = NULL;
    long long count = 0;
    long long local_count = 0;

    if(path == NULL) {
        warning(stderr, "count_lines_in_dir(): the function argument was passed the NULL value (const char *path)\n");
	return -1;
    }

    dir = opendir(path);
    if(dir == NULL) {
        warning(stderr, "count_lines_in_dir(): an error occurred while opening directory (%s)", path);
        return -1;
    }

    while((entry = readdir(dir)) != NULL) {
        full_path = concat_name_and_path(path, entry->d_name);

        // if it's a file, then count the lines in it
        if(is_file(full_path)) {
            if(is_normal_file(entry->d_name)) {
                if((local_count = count_lines_in_file(full_path)) != -1) {
                    count += local_count;
		}
            }
        }

        // if recursion is allowed for dirs, then check
        // the dir if it is
        if(recursion_flag) {
            // if it's a dir, then open it and count the
            // lines in the files there
            if(is_dir(full_path)) {
                if(is_normal_dir(entry->d_name)) {
                    if((local_count = count_lines_in_dir(full_path)) != -1) {
                        count += local_count;
		    }
                }
            }
        }

        free(full_path);
    }

    closedir(dir);
    return count;
}
