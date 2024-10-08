#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

// #define PATH_MAX = 1024;

/*
Citations:
  - https://pubs.opengroup.org/onlinepubs/009696799/functions/stat.html
  - https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
  - https://www.geeksforgeeks.org/strpbrk-in-c/
  - https://pubs.opengroup.org/onlinepubs/009604499/functions/closedir.html
*/

void list_directory(const char* directory_name);
void print_perms(struct stat stat_buf);

int main(int argc, char** argv) {
  // Process arguments
  if (argc == 1) {
    list_directory(".");
  } else if (argc == 2) {
    list_directory(argv[1]);
  } else {
    fprintf(stderr, "Please specify one or zero command line arguments.\n");
  }

  return 0;
}


void print_perms(struct stat stat_buf) {
    printf( (S_ISDIR(stat_buf.st_mode)) ? "d" : "-");
    printf( (stat_buf.st_mode & S_IRUSR) ? "r" : "-");
    printf( (stat_buf.st_mode & S_IWUSR) ? "w" : "-");
    printf( (stat_buf.st_mode & S_IXUSR) ? "x" : "-");
    printf( (stat_buf.st_mode & S_IRGRP) ? "r" : "-");
    printf( (stat_buf.st_mode & S_IWGRP) ? "w" : "-");
    printf( (stat_buf.st_mode & S_IXGRP) ? "x" : "-");
    printf( (stat_buf.st_mode & S_IROTH) ? "r" : "-");
    printf( (stat_buf.st_mode & S_IWOTH) ? "w" : "-");
    printf( (stat_buf.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");
}

void list_directory(const char* directory_name) {
  // TODO: Print the contents of the specified directory here
  DIR* dir;
  struct dirent* entry;
  struct stat stat_buf;
  struct passwd* pwd;
  // struct passwd* temp_pwd;
  struct group* grp;

  if ((dir = opendir(directory_name)) == NULL) {
    perror("Failed to open directory");
    return;
  }
  while ((entry = readdir(dir))!= NULL) {
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", directory_name, entry->d_name);
    if (stat(full_path, &stat_buf) == -1) {
      perror("Failed to open directory");
      continue;
    }

    // Permissions
    print_perms(stat_buf);

    // Owner
    if ((pwd = getpwuid(stat_buf.st_uid)) != NULL) {
      printf(" %-8.8s", pwd->pw_name);
    } else {
      printf(" Owner not found");
    }

    // Group
    if ((grp = getgrgid(stat_buf.st_gid))!= NULL) {
      printf(" %-8.8s", grp->gr_name);
    } else {
      printf(" Group not found");
    }

    // File size
    printf(" %9jd", (intmax_t)stat_buf.st_size);

   // file name
    printf(" %s\n", entry->d_name);
  }
  closedir(dir);
}
