#include "tlpi_hdr.h"
#define BUF_SIZE 256

ssize_t sendbigfile(int out_fd, int in_fd, off_t *offset, size_t count) {
    off_t orig;
    char buf[BUF_SIZE];
    size_t toRead, numRead, numSent, totSent;

    if (offset != NULL) {

        /* Save current file offset and set offset to value in '*offset' */
        orig = lseek(in_fd, 0, SEEK_CUR);
        if (orig == -1)
            return -1;
        if (lseek(in_fd, *offset, SEEK_SET) == -1)
            return -1;
    }

    totSent = 0;

    while (count > 0) {
        toRead = min(BUF_SIZE, count);

        numRead = read(in_fd, buf, toRead);
        printf("READ %d\n", numRead);

        if (numRead == -1)
            return -1;
        if (numRead == 0){
            int chunk_size = snprintf(NULL, 0, "%zx\r\n", numRead);
            char chunk[chunk_size];
            sprintf(chunk, "%zx\r\n", numRead);
            int b = write(out_fd, chunk, strlen(chunk));
            printf("HEAD: %d\n", b);
            
            break;
        }

                                  /* EOF */

        int chunk_size = snprintf(NULL, 0, "%zx\r\n", numRead);
        char chunk[chunk_size];
        sprintf(chunk, "%zx\r\n", numRead);
        int b = write(out_fd, chunk, strlen(chunk));
        printf("HEAD: %d\n", b);

        numSent = write(out_fd, buf, numRead);

        printf("SENT %d\n", numSent);

        int a = write(out_fd, "\r\n", strlen("\r\n"));
        printf("CRLF: %d\n", a);

        if (numSent == -1)
            return -1;
        if (numSent == 0)               /* Should never happen */
            printf("sendfile: write() transferred 0 bytes");

        count -= numSent;
        totSent += numSent;
        memset(buf, 0, BUF_SIZE);
    }

    if (offset != NULL) {

        /* Return updated file offset in '*offset', and reset the file offset
           to the value it had when we were called. */

        *offset = lseek(in_fd, 0, SEEK_CUR);
        if (*offset == -1)
            return -1;
        if (lseek(in_fd, orig, SEEK_SET) == -1)
            return -1;
    }

    return totSent;
}