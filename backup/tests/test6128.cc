#ident "Copyright (c) 2012-2013 Tokutek Inc.  All rights reserved."
#ident "$Id$"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "backup.h"
#include "backup_test_helpers.h"

#define FIRSTBYTES "first bytes\n"
#define MOREBYTES  "more bytes\n"

int test_main (int argc __attribute__((__unused__)), const char *argv[] __attribute__((__unused__))) {
    cleanup_dirs();
    setup_source();
    setup_destination();

    char *src = get_src();
    char *dst = get_dst();
    int fd0 = openf(O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO, "%sfile0", src);
    check(fd0>=0);
    {
        ssize_t r = write(fd0, FIRSTBYTES, sizeof(FIRSTBYTES)-1);
        check(r==sizeof(FIRSTBYTES)-1);
    }
    
    pthread_t th;
    start_backup_thread(&th);
    usleep(10000);
    {
        ssize_t r = write(fd0, MOREBYTES, sizeof(MOREBYTES)-1);
        check(r==sizeof(MOREBYTES)-1);
    }
    finish_backup_thread(th);
    {
        int status = systemf("diff -r %s %s", src, dst);
        check(status!=-1);
        check(WIFEXITED(status));
        check(WEXITSTATUS(status)==0);
    }
    free(src);
    free(dst);
    return 0;
}