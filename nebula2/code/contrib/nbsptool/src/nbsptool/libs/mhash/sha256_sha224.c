/* sha256_sha224.c
 *
 * The sha256 and sha224 hash functions.
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001 Niels M�ller
 *  
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

/* Modelled after the sha1.c code by Peter Gutmann. */

/* This has been modified in order to fit in mhash.
 * --nmav.
 */

/* The SHA224 support was added by B. Poettering in April 2004. */

#include "nbsptool/libs/mhash/libdefs.h"

#ifdef ENABLE_SHA256_SHA224

#include "nbsptool/libs/mhash/mhash_sha256_sha224.h"
#include "nbsptool/libs/mhash/mhash_sha1.h"

/* A block, treated as a sequence of 32-bit words. */
#define SHA256_SHA224_DATA_LENGTH 16

#define ROTR(n,x) ((x)>>(n) | ((x)<<(32-(n))))
#define SHR(n,x) ((x)>>(n))

/* The SHA256/224 functions. The Choice function is the same as the SHA1
   function f1, and the majority function is the same as the SHA1 f3
   function. They can be optimized to save one boolean operation each
   - thanks to Rich Schroeppel, rcs@cs.arizona.edu for discovering
   this */

/* #define Choice(x,y,z) ( ( (x) & (y) ) | ( ~(x) & (z) ) ) */
#define Choice(x,y,z)   ( (z) ^ ( (x) & ( (y) ^ (z) ) ) )
/* #define Majority(x,y,z) ( ((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)) ) */
#define Majority(x,y,z) ( ((x) & (y)) ^ ((z) & ((x) ^ (y))) )

#define S0(x) (ROTR(2,(x)) ^ ROTR(13,(x)) ^ ROTR(22,(x)))
#define S1(x) (ROTR(6,(x)) ^ ROTR(11,(x)) ^ ROTR(25,(x)))

#define s0(x) (ROTR(7,(x)) ^ ROTR(18,(x)) ^ SHR(3,(x)))
#define s1(x) (ROTR(17,(x)) ^ ROTR(19,(x)) ^ SHR(10,(x)))

/* Generated by the shadata program. */
static const word32 K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0xfc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x6ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};

/* The initial expanding function.  The hash function is defined over an
   64-word expanded input array W, where the first 16 are copies of the input
   data, and the remaining 48 are defined by

        W[ t ] = s1(W[t-2] + W[t-7] + s0(W[i-15] + W[i-16]

   This implementation generates these values on the fly in a circular
   buffer - thanks to Colin Plumb, colin@nyx10.cs.du.edu for this
   optimization.
*/

#define EXPAND(W,i) \
( W[(i) & 15 ] += (s1(W[((i)-2) & 15]) + W[((i)-7) & 15] + s0(W[((i)-15) & 15])) )

/* The prototype SHA sub-round.  The fundamental sub-round is:

        T1 = h + S1(e) + Choice(e,f,g) + K[t] + W[t]
    T2 = S0(a) + Majority(a,b,c)
    a' = T1 + T2
    b' = a
    c' = b
    d' = c
    e' = d + T1
    f' = e
    g' = f
    h' = g

   but this is implemented by unrolling the loop 8 times and renaming
   the variables
   ( h, a, b, c, d, e, f, g ) = ( a, b, c, d, e, f, g, h ) each
   iteration. This code is then replicated 8, using the next 8 values
   from the W[] array each time */

#define ROUND(a,b,c,d,e,f,g,h,k,data) do {        \
  word32 T1 = h + S1(e) + Choice(e,f,g) + k + data;    \
  d += T1;                        \
  h = T1 + S0(a) + Majority(a,b,c);            \
} while (0)

/* Initialize the SHA values */

void sha256_init(struct sha256_sha224_ctx *ctx)
{
    /* Initial values, also generated by the shadata program. */
    static const word32 H0[_SHA256_SHA224_DIGEST_LENGTH] = {
        0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
        0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL,
    };

    memcpy(ctx->state, H0, sizeof(H0));

    /* Initialize bit count */
    ctx->bitcount = 0;

    /* Initialize buffer */
    ctx->index = 0;
}

void sha224_init(struct sha256_sha224_ctx *ctx)
{
    /* Initial values, copied from fips180-2. */
    static const word32 H0[_SHA256_SHA224_DIGEST_LENGTH] = {
      0xc1059ed8UL, 0x367cd507UL, 0x3070dd17UL, 0xf70e5939UL, 
      0xffc00b31UL, 0x68581511UL, 0x64f98fa7UL, 0xbefa4fa4U
    };

    memcpy(ctx->state, H0, sizeof(H0));

    /* Initialize bit count */
    ctx->bitcount = 0;

    /* Initialize buffer */
    ctx->index = 0;
}

/* Perform the SHA transformation.  Note that this code, like MD5, seems to
   break some optimizing compilers due to the complexity of the expressions
   and the size of the basic block.  It may be necessary to split it into
   sections, e.g. based on the four subrounds

   Note that this function destroys the data area */

static void sha256_sha224_transform(word32 * state, word32 * data)
{
    word32 A, B, C, D, E, F, G, H;    /* Local vars */
    unsigned i;
    const word32 *k;
    word32 *d;

    /* Set up first buffer and local data buffer */
    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];
    E = state[4];
    F = state[5];
    G = state[6];
    H = state[7];

    /* Heavy mangling */
    /* First 16 subrounds that act on the original data */

    for (i = 0, k = K, d = data; i < 16; i += 8, k += 8, d += 8) {
        ROUND(A, B, C, D, E, F, G, H, k[0], d[0]);
        ROUND(H, A, B, C, D, E, F, G, k[1], d[1]);
        ROUND(G, H, A, B, C, D, E, F, k[2], d[2]);
        ROUND(F, G, H, A, B, C, D, E, k[3], d[3]);
        ROUND(E, F, G, H, A, B, C, D, k[4], d[4]);
        ROUND(D, E, F, G, H, A, B, C, k[5], d[5]);
        ROUND(C, D, E, F, G, H, A, B, k[6], d[6]);
        ROUND(B, C, D, E, F, G, H, A, k[7], d[7]);
    }

    for (; i < 64; i += 16, k += 16) {
        ROUND(A, B, C, D, E, F, G, H, k[0], EXPAND(data, 0));
        ROUND(H, A, B, C, D, E, F, G, k[1], EXPAND(data, 1));
        ROUND(G, H, A, B, C, D, E, F, k[2], EXPAND(data, 2));
        ROUND(F, G, H, A, B, C, D, E, k[3], EXPAND(data, 3));
        ROUND(E, F, G, H, A, B, C, D, k[4], EXPAND(data, 4));
        ROUND(D, E, F, G, H, A, B, C, k[5], EXPAND(data, 5));
        ROUND(C, D, E, F, G, H, A, B, k[6], EXPAND(data, 6));
        ROUND(B, C, D, E, F, G, H, A, k[7], EXPAND(data, 7));
        ROUND(A, B, C, D, E, F, G, H, k[8], EXPAND(data, 8));
        ROUND(H, A, B, C, D, E, F, G, k[9], EXPAND(data, 9));
        ROUND(G, H, A, B, C, D, E, F, k[10], EXPAND(data, 10));
        ROUND(F, G, H, A, B, C, D, E, k[11], EXPAND(data, 11));
        ROUND(E, F, G, H, A, B, C, D, k[12], EXPAND(data, 12));
        ROUND(D, E, F, G, H, A, B, C, k[13], EXPAND(data, 13));
        ROUND(C, D, E, F, G, H, A, B, k[14], EXPAND(data, 14));
        ROUND(B, C, D, E, F, G, H, A, k[15], EXPAND(data, 15));
    }

    /* Update state */
    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
    state[4] += E;
    state[5] += F;
    state[6] += G;
    state[7] += H;
}

static void sha256_sha224_block(struct sha256_sha224_ctx *ctx, const byte * block)
{
    word32 data[SHA256_SHA224_DATA_LENGTH];
    int i;

    /* Update block count */
    ctx->bitcount += 512;

    /* Endian independent conversion */
    for (i = 0; i < SHA256_SHA224_DATA_LENGTH; i++, block += 4)
        data[i] = STRING2INT(block);

    sha256_sha224_transform(ctx->state, data);
}

void
sha256_sha224_update(struct sha256_sha224_ctx *ctx, const byte * buffer, unsigned length)
{
    if (ctx->index) {    /* Try to fill partial block */
        unsigned left = SHA256_SHA224_DATA_SIZE - ctx->index;
        if (length < left) {
            memcpy(ctx->block + ctx->index, buffer, length);
            ctx->index += length;
            return;    /* Finished */
        } else {
            memcpy(ctx->block + ctx->index, buffer, left);
            sha256_sha224_block(ctx, ctx->block);
            buffer += left;
            length -= left;
        }
    }
    while (length >= SHA256_SHA224_DATA_SIZE) {
        sha256_sha224_block(ctx, buffer);
        buffer += SHA256_SHA224_DATA_SIZE;
        length -= SHA256_SHA224_DATA_SIZE;
    }
    /* Buffer leftovers */
    /* NOTE: The corresponding sha1 code checks for the special case length == 0.
     * That seems supoptimal, as I suspect it increases the number of branches. */

    memcpy(ctx->block, buffer, length);
    ctx->index = length;
}

/* Final wrapup - pad to SHA256_SHA224_DATA_SIZE-byte boundary with the bit pattern
   1 0* (64-bit count of bits processed, MSB-first) */

void sha256_sha224_final(struct sha256_sha224_ctx *ctx)
{
    word32 data[SHA256_SHA224_DATA_LENGTH];
    int i;
    int words;

    i = ctx->index;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */

/*  assert(i < SHA256_SHA224_DATA_SIZE);
 */
    ctx->block[i++] = 0x80;

    /* Fill rest of word */
    for (; i & 3; i++)
        ctx->block[i] = 0;

    /* i is now a multiple of the word size 4 */
    words = i >> 2;
    for (i = 0; i < words; i++)
        data[i] = STRING2INT(ctx->block + 4 * i);

    if (words > (SHA256_SHA224_DATA_LENGTH - 2)) {    /* No room for length in this block. 
                            * Process it and pad with another one */
        for (i = words; i < SHA256_SHA224_DATA_LENGTH; i++)
            data[i] = 0;
        sha256_sha224_transform(ctx->state, data);
        for (i = 0; i < (SHA256_SHA224_DATA_LENGTH - 2); i++)
            data[i] = 0;
    } else
        for (i = words; i < SHA256_SHA224_DATA_LENGTH - 2; i++)
            data[i] = 0;

    ctx->bitcount += 8 * ctx->index;
    data[SHA256_SHA224_DATA_LENGTH - 2] =
      ctx->bitcount >> 32;
    data[SHA256_SHA224_DATA_LENGTH - 1] =
      ctx->bitcount & 0xffffffff;
    sha256_sha224_transform(ctx->state, data);
}

void sha256_sha224_digest(const struct sha256_sha224_ctx *ctx, byte * s, int len)
{
    int i;

    if (s!=NULL)
        for (i = 0; i < len; i++) {
            *s++ = ctx->state[i] >> 24;
            *s++ = 0xff & (ctx->state[i] >> 16);
            *s++ = 0xff & (ctx->state[i] >> 8);
            *s++ = 0xff & ctx->state[i];
        }
}

void sha256_digest(const struct sha256_sha224_ctx *ctx, byte * s)
{
  sha256_sha224_digest(ctx, s, SHA256_DIGEST_SIZE / 4);
}

void sha224_digest(const struct sha256_sha224_ctx *ctx, byte * s)
{
  sha256_sha224_digest(ctx, s, SHA224_DIGEST_SIZE / 4);
}

#endif /* ENABLE_SHA256_SHA224 */
