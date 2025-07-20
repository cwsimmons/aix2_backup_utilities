
/* 

    Copyright 2024 Christopher Simmons

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program. If not, see <https://www.gnu.org/licenses/>.

*/

#include "backup2.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int min(int x, int y) {
    return (x < y) ? x : y;
}

int max(int x, int y) {
    return (x > y) ? x : y;
}

#define DIV_CEIL(x, y) ((x + y - 1) / y)

/* modes */
#define	IFMT	0170000		/* type of file */
#define		IFDIR	0040000	/* directory */
#define		IFCHR	0020000	/* character special */
#define		IFBLK	0060000	/* block special */
#define		IFREG	0100000	/* regular */
#define         IFMPC   0030000 /* multiplexed char special - obsolete */
#define         IFMPB   0070000 /* multiplexed block special - obsolete */
#define		IFIFO	0010000	/* fifo special */
#define	ISUID	04000		/* set user id on execution */
#define	ISGID	02000		/* set group id on execution */
#define ISVTX	01000		/* save swapped text even after use */
#define	IREAD	0400		/* read, write, execute permissions */
#define	IWRITE	0200
#define	IEXEC	0100

int mkdir_p(const char *path) {
    char temp[1024];
    char *p = NULL;
    size_t len;

    // Copy path to a buffer we can modify
    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);

    // Remove trailing slashes
    while (len > 0 && temp[len - 1] == '/') {
        temp[--len] = '\0';
    }

    // Create each directory in the path
    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
                perror("mkdir");
                return -1;
            }
            *p = '/';
        }
    }

    // Create the final directory
    // Skip because we will pass file paths to this function
    // (last component will be file name)

    // if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
    //     perror("mkdir");
    //     return -1;
    // }

    return 0;
}

int main(int argc, char** argv) {
    
    int num_files = argc - 1;

    int x = 0;

    union fs_rec name_header;
    FILE* current_output_file = NULL;
    int bytes_copied;

    char buffer[BUFFER_SIZE];

    for(int i = 0; i < num_files; i++) {

        printf("Opening file #%d\n", i+1);
        FILE* backup_file = fopen(argv[1+i], "rb");

        union fs_rec volume_header;
        fread(&volume_header, sizeof(volume_header.v), 1, backup_file);

        fseek(backup_file, 0, SEEK_END);
        int file_length = ftell(backup_file);
        printf("File length = %d\n", file_length);


        if (current_output_file != NULL) {
            // Finish extracting file from last file
            fseek(backup_file, volume_header.v.h.len * 8, SEEK_SET);

            while (bytes_copied < name_header.n.size) {
                int grab = min(name_header.n.size - bytes_copied, BUFFER_SIZE);
                fread(buffer, 1, grab, backup_file);
                fwrite(buffer, 1, grab, current_output_file);
                bytes_copied += grab;
            }

            fclose(current_output_file);
            current_output_file = NULL;

        }
        
        x += volume_header.v.h.len * 8;


        while(1) {

            fseek(backup_file, x, SEEK_SET);
            
            int num_read = fread(&name_header, sizeof(union fs_rec), 1, backup_file);
            if (num_read != 1) {
                break;
            }
            else if (name_header.h.type != FS_NAME) {
                printf("Warning: non FS_NAME header found (%d)\n", name_header.h.type);
                break;
            } else {
                printf("0x%.4x: %s  (length = %d) (mode=0x%.4x)\n", x, name_header.n.name, name_header.n.size, name_header.n.mode);

                x += name_header.h.len * 8;
                fseek(backup_file, x, SEEK_SET);
                int bytes_to_copy = min(name_header.n.size, file_length - x);
                x += DIV_CEIL(name_header.n.size, BPW) * BPW;

                mkdir_p(name_header.n.name);

                if (name_header.n.mode & IFDIR) {
                    mkdir(name_header.n.name, 0755);
                } else if (name_header.n.mode & IFREG) {

                    current_output_file = fopen(name_header.n.name, "wb");

                    if (current_output_file == 0) {
                        printf("Could not open for writing\n");
                        continue;;
                    }

                    bytes_copied = 0;

                    while (bytes_copied < bytes_to_copy) {
                        int grab = min(bytes_to_copy - bytes_copied, BUFFER_SIZE);
                        fread(buffer, 1, grab, backup_file);
                        fwrite(buffer, 1, grab, current_output_file);
                        bytes_copied += grab;
                    }

                    if (bytes_copied == name_header.n.size) {
                        fclose(current_output_file);
                        current_output_file = NULL;
                    } else {
                        x -= file_length;
                        break;
                    }

                }

                
            }
            

        }

        fclose(backup_file);

    }


    return 0;
}