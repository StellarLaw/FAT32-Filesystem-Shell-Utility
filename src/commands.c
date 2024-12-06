#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commands.h"
#include "fs.h"
#include "utils.h"

extern char current_path[512];

void run_shell() {
    char cmdline[256];
    char orig_line[256];
    char *args[16];

    while (1) {
        printf("%s%s> ", fsinfo.image_name, current_path);

        if (!fgets(cmdline, sizeof(cmdline), stdin)) break;
        trim_whitespace(cmdline);

        if (strlen(cmdline) == 0) continue;

        strcpy(orig_line, cmdline);

        int argc = 0;
        char *token = strtok(cmdline, " ");
        while (token && argc < 16) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        if (argc == 0) continue;

        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "info") == 0) {
            fs_info();
        } else if (strcmp(args[0], "cd") == 0) {
            if (argc != 2) {
                print_error("Usage: cd [DIRNAME]");
            } else {
                char old_path[512];
                strcpy(old_path, current_path);

                if (fs_cd(args[1]) == 0) {
                    if (strcmp(args[1], ".") == 0) {
                    } else if (strcmp(args[1], "..") == 0) {
                        if (strcmp(current_path, "/") != 0) {
                            char *last_slash = strrchr(current_path, '/');
                            if (last_slash && last_slash != current_path) {
                                *last_slash = '\0';
                            } else {
                                strcpy(current_path, "/");
                            }
                        }
                    } else {
                        if (strcmp(current_path, "/") == 0) {
                            snprintf(current_path, sizeof(current_path), "/%s", args[1]);
                        } else {
                            size_t len = strlen(current_path);
                            snprintf(current_path + len, sizeof(current_path) - len, "/%s", args[1]);
                        }
                    }
                } else {
                    strcpy(current_path, old_path);
                }
            }
        } else if (strcmp(args[0], "ls") == 0) {
            fs_ls();
        } else if (strcmp(args[0], "mkdir") == 0) {
            if (argc!=2) print_error("Usage: mkdir [DIRNAME]");
            else fs_mkdir(args[1]);
        } else if (strcmp(args[0], "creat") == 0) {
            if (argc!=2) print_error("Usage: creat [FILENAME]");
            else fs_creat(args[1]);
        } else if (strcmp(args[0], "open") == 0) {
            if (argc!=3) print_error("Usage: open [FILENAME] [FLAGS]");
            else fs_open(args[1], args[2]);
        } else if (strcmp(args[0], "close") == 0) {
            if (argc!=2) print_error("Usage: close [FILENAME]");
            else fs_close(args[1]);
        } else if (strcmp(args[0], "lsof") == 0) {
            fs_lsof();
        } else if (strcmp(args[0], "size") == 0) {
            if (argc!=2) print_error("Usage: size [FILENAME]");
            else fs_size(args[1]);
        } else if (strcmp(args[0], "lseek") == 0) {
            if (argc!=3) print_error("Usage: lseek [FILENAME] [OFFSET]");
            else {
                uint32_t off = strtoul(args[2],NULL,10);
                fs_lseek(args[1], off);
            }
        } else if (strcmp(args[0], "read") == 0) {
            if (argc!=3) print_error("Usage: read [FILENAME] [SIZE]");
            else {
                uint32_t sz = strtoul(args[2], NULL, 10);
                fs_read(args[1], sz);
            }
        } else if (strcmp(args[0], "write") == 0) {
            if (argc < 3) {
                print_error("Usage: write [FILENAME] \"[STRING]\"");
            } else {
                char *pos = strstr(orig_line, args[1]);
                if (!pos) {
                    print_error("File name not found in line.");
                    continue;
                }

                pos += strlen(args[1]);
                trim_whitespace(pos);

                char *start = strchr(pos, '\"');
                if (!start) {
                    print_error("STRING must be quoted.");
                    continue;
                }
                start++;
                char *end = strchr(start, '\"');
                if (!end) {
                    print_error("STRING must be quoted.");
                    continue;
                }
                *end = '\0';
                fs_write(args[1], start);
            }
        } else if (strcmp(args[0],"rename")==0) {
            if (argc!=3) print_error("Usage: rename [FILENAME] [NEW_FILENAME]");
            else fs_rename(args[1], args[2]);
        } else if (strcmp(args[0],"rm")==0) {
            if (argc!=2) print_error("Usage: rm [FILENAME]");
            else fs_rm(args[1]);
        } else if (strcmp(args[0],"rmdir")==0) {
            if (argc!=2) print_error("Usage: rmdir [DIRNAME]");
            else fs_rmdir(args[1]);
        } else {
            print_error("Unknown command.");
        }
    }
}
