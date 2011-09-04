/*
 * read some binary from argv[1], and then
 * compresses it with ghc, and writes the result to
 * argv[2] file.
 *
 * instead, should read/write pcap files, maybe add 6lowpan too
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ghc_compress(unsigned char *inbuf, unsigned char *outbuf, int inlen)
{
    unsigned char *(insts[128]);              /* 128 pointers to char * */
    int i;
    int instno = 0;
    unsigned char *outp = outbuf;
    unsigned char *inp  = inbuf;
    memset(insts, 0, sizeof(insts));

    do {
        int klen = inlen;
        int cnt;
        if(klen > 95) klen = 95;

        cnt = 0;

        /* set the instruction */
        insts[instno] = outp;
        outp++;

        /* now copy up to two consecutive 0s */
        while(cnt < klen && (inp[0]!=0 || inp[1]!=0)) {
            *outp++ = *inp++;
            cnt++;
        }
        (*insts[instno])  = cnt;
        inlen -= cnt;
        instno++;

        /* how many zeros can we eat? */
        if(inp[0] == 0 && inp[1]==0) {
            cnt=0;
            inp++;
            while(*++inp == 0 && cnt < 16) cnt++;
            insts[instno] = outp++;
            *(insts[instno]) = 0x80 + cnt;
            instno++;
            inlen -= cnt+2;
        }
    } while(inlen > 0);

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
