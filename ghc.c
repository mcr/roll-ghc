/*
 * read some binary from argv[1], and then
 * compresses it with ghc, and writes the result to
 * argv[2] file.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ghc_compress(unsigned char *inbuf, unsigned char *outbuf, int inlen)
{
    unsigned char *insts[128];
    int i;
    int instno = 0;
    unsigned char *outp = outbuf;
    unsigned char *inp  = inbuf;

    i = inlen;
    do {
        int klen = i;
        if(klen > 95) klen = 95;

        /* set the instruction */
        *outp         = klen;
        insts[instno] = outp + 1;
        memcpy(insts[instno], inp, klen);

        i   -= klen;
        inp += klen;
        outp += klen+1;
        instno++;
    } while(i > 0);

    return outp - outbuf;
}


void main(int argc, char *argv[])
{
    FILE *in, *out;
    unsigned char  packbuf[2048];
    unsigned char  outbuf[2048];
    int inlen, outlen;

    if(argc != 3 || argv[1]==NULL || argv[2]==NULL) {
        fprintf(stderr, "Usage: ghc in out\n");
        exit(5);
    }

    in = fopen(argv[1], "r");
    if(!in) {
        perror(argv[1]);
        exit(6);
    }
    out= fopen(argv[2], "w");
    if(!out) {
        perror(argv[2]);
        exit(7);
    }

    inlen = fread(packbuf, 1, 2048, in);
    outlen = ghc_compress(packbuf, outbuf, inlen);
    fwrite(outbuf, 1, outlen, out);
    fclose(in);
    fclose(out);
}

/*
 * Local Variables:
 * c-basic-offset:4
 * c-style: whitesmith
 * End:
 */
