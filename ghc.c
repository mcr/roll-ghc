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

        /* now copy until we detect something we can do better */
        while(cnt < klen
              && (inp[0]!=0 || inp[1]!=0)    /* zero compression */
              && (outp <= outbuf+1 || inp[0]!=outp[-1] || inp[1] != outp[0])
            ) {
            *++outp = *inp++;
            cnt++;
        }
        if(cnt > 0) {
            outp++;
            (*insts[instno])  = cnt;
            inlen -= cnt;
            instno++;
        }

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

        /* how many previous 2-character patterns can we repeat? */
        if(outp > outbuf+1) {
            unsigned char prev2 = outp[-2];
            unsigned char prev1 = outp[-1];
            while(inp[0] == prev2 && inp[1]== prev1) {
                cnt=0;
                inp += 2;

                insts[instno]    = outp++;
                *(insts[instno]) = 0xc0;   /* append previous 2 bytes */
                instno++;
                inlen -= cnt+2;
            }
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

    fprintf(stderr, "Input %u Output %u ratio=%u%%",
            inlen, outlen, inlen * 100 / outlen);
}

/*
 * Local Variables:
 * c-basic-offset:4
 * c-style: whitesmith
 * End:
 */
