#include "pparser.h"
#include "../kernel.h"
#include "../string/string.h"
#include "../status.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"

/*
 * NOTES
 * Everything is passed by value in C.
 * If you pass a pointer then you get the address of the value that the
 * pointer points to. The pointer can't be modified because the pointer
 * itself is passed by value. To modify the pointer it needs to be passed
 * by reference. This is done by passing a pointer to that pointer.
 * 
 * EXAMPLE
 * Passing a pointer to a character
 * You can modify the characters within the string but you can't
 * modify where the pointer to the character points after the function
 * has finished executing because you received a copy of the pointer.
 * The original pointer will still point to the first character.
 * To modify and remember its 'place' you must pass a pointer to that
 * pointer.
 * 
 */

/**
 * @brief
 * Checks if the path is of a valid format
 *
 * @param filename
 * @return int
 * @todo strnlen is returning 0
 */
static int pathparser_path_valid_format(const char *filename)
{
    int len = strnlen(filename, GRAPEOS_MAX_PATH);
    return (len >= 3 && isdigit(filename[0]) && memcmp((void *)&filename[1], ":/", 2) == 0);
}

/**
 * @brief
 * Extracts the drive number from the path and reposition the path variable 3
 * bytes ahead to point to the next part
 * @param path
 * @return int
 */
static int pathparser_get_drive_by_path(const char **path)
{
    if (!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }
    int drive_no = tonumericdigit(*path[0]);

    // Add 3 bytes to skip drive number 0:/ 1:/ 2:/
    *path += 3;

    return drive_no;
}

/**
 * @brief
 * Creates the path root given the drive number
 *
 * @param drive_number
 * @return struct path_root*
 */
static struct path_root *pathparser_create_root(int drive_number)
{
    struct path_root *path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}

/**
 * @brief
 * Extracts one part of the path then moves the pointer forward to the next
 * part
 *
 * @param path
 * @return const char*
 */
static const char *pathparser_get_path_part(const char **path)
{
    char *result_path_part = kzalloc(GRAPEOS_MAX_PATH);
    int i = 0;
    while (**path != '/' && **path != 0x00)
    {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if (**path == '/')
    {
        // Skip the forward slash to avoid problems
        *path += 1;
    }

    if (i == 0)
    {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

/**
 * @brief
 * Creates a path part struct from the given path. This path_part is set as the
 * next part of last_part.
 * @param last_part
 * @param path
 * @return struct path_part* The part struct created
 */
struct path_part *pathparser_parse_path_part(struct path_part *last_part, const char **path)
{
    const char *path_part_str = pathparser_get_path_part(path);
    if (!path_part_str)
    {
        return 0;
    }
    struct path_part *part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;

    if (last_part)
    {
        last_part->next = part;
    }

    return part;
}

/**
 * @brief
 * Frees the memory of the path
 * @param root
 */
void pathparser_free(struct path_root *root)
{
    struct path_part *part = root->first;

    while (part)
    {
        struct path_part *next_part = part->next;
        kfree((void *)part->part);
        kfree(part);
        part = next_part;
    }

    kfree(root);
}

/**
 * @brief
 * Parses the path into a path_root struct
 * @param path
 * @param current_directory_path
 * @return struct path_root*
 */
struct path_root *pathparser_parse(const char *path, const char *current_directory_path)
{
    int res = 0;
    const char *tmp_path = path;
    struct path_root *path_root = 0;

    if (strlen(path) > GRAPEOS_MAX_PATH)
    {
        print("1");
        goto out;
    }

    res = pathparser_get_drive_by_path(&tmp_path);
    if (res < 0)
    {
        goto out;
    }

    path_root = pathparser_create_root(res);
    if (!path_root)
    {
        print("3");
        goto out;
    }

    struct path_part *first_part = pathparser_parse_path_part(NULL, &tmp_path);

    if (!first_part)
    {
        goto out;
    }

    path_root->first = first_part;
    struct path_part *part = pathparser_parse_path_part(first_part, &tmp_path);
    while (part)
    {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

out:
    return path_root;
}