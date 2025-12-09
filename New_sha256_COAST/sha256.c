#include "uart.h"
#include "sha256.h"
#include "kernelsdk.h"

// Hash data input - main and TMR copies
extern uint8_t hash_data[HASH_DATA_LEN];
extern uint8_t hash_data_DWC[HASH_DATA_LEN];
extern uint8_t hash_data_TMR[HASH_DATA_LEN];

// SHA256 context data - main and TMR copies
extern unsigned char ctx_data[64];
extern unsigned char ctx_data_DWC[64];
extern unsigned char ctx_data_TMR[64];

// SHA256 state - main and TMR copies
extern uint32_t ctx_state[8];
extern uint32_t ctx_state_DWC[8];
extern uint32_t ctx_state_TMR[8];

// SHA256 bit length - main and TMR copies
extern uint32_t ctx_bitlen[2];
extern uint32_t ctx_bitlen_DWC[2];
extern uint32_t ctx_bitlen_TMR[2];

// Result hash - main and TMR copies
extern unsigned char hashResult[32];
extern unsigned char hashResult_DWC[32];
extern unsigned char hashResult_TMR[32];

// Golden hash (computed without TMR)
unsigned char golden_hash[32];

// K constants for SHA256
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

unsigned int tmr_mitigations = 0;
void sha256();

// Macros for SHA256
#define DBL_INT_ADD(a, b, c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

// Local SHA256 transform for golden computation
static void local_sha256_transform(uint32_t state[], unsigned char data[]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, s, t1, t2, m[64];
    uint32_t ep0, ep1, ch, maj, sig0, sig1;
    uint32_t temp;

    for (i = 0, j = 0; i < 16; ++i, j += 4) {
        temp = (((uint32_t)data[j]) << 24);
        temp |= (((uint32_t)data[j + 1]) << 16);
        temp |= (data[j + 2] << 8);
        temp |= (data[j + 3]);
        m[i] = temp;
    }

    for (; i < 64; ++i) {
        s = m[i - 2];
        sig1 = ROTRIGHT(s, 17);
        sig1 ^= ROTRIGHT(s, 19);
        sig1 ^= s >> 10;

        s = m[i - 15];
        sig0 = ROTRIGHT(s, 7);
        sig0 ^= ROTRIGHT(s, 18);
        sig0 ^= s >> 3;

        temp = sig1;
        temp += m[i - 7];
        temp += sig0;
        temp += m[i - 16];
        m[i] = temp;
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    for (i = 0; i < 64; ++i) {
        ep0 = ROTRIGHT(a, 2);
        ep0 ^= ROTRIGHT(a, 13);
        ep0 ^= ROTRIGHT(a, 22);
        ep1 = ROTRIGHT(e, 6);
        ep1 ^= ROTRIGHT(e, 11);
        ep1 ^= ROTRIGHT(e, 25);
        ch = (e & f) ^ (~e & g);
        maj = (a & b) ^ (a & c) ^ (b & c);
        t1 = h + ep1 + ch + k[i] + m[i];
        t2 = ep0 + maj;
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

// Local SHA256 hash for golden computation
static void local_sha256_hash(unsigned char data[], uint32_t len, unsigned char hash[]) {
    unsigned char local_ctx_data[64];
    uint32_t local_ctx_state[8];
    uint32_t local_ctx_bitlen[2];
    uint32_t ctx_datalen = 0;
    
    local_ctx_bitlen[0] = 0;
    local_ctx_bitlen[1] = 0;
    local_ctx_state[0] = 0x6a09e667;
    local_ctx_state[1] = 0xbb67ae85;
    local_ctx_state[2] = 0x3c6ef372;
    local_ctx_state[3] = 0xa54ff53a;
    local_ctx_state[4] = 0x510e527f;
    local_ctx_state[5] = 0x9b05688c;
    local_ctx_state[6] = 0x1f83d9ab;
    local_ctx_state[7] = 0x5be0cd19;

    uint32_t i;

    for (i = 0; i < len; ++i) {
        local_ctx_data[ctx_datalen] = data[i];
        ctx_datalen++;
        if (ctx_datalen == 64) {
            local_sha256_transform(local_ctx_state, local_ctx_data);
            DBL_INT_ADD(local_ctx_bitlen[0], local_ctx_bitlen[1], 512);
            ctx_datalen = 0;
        }
    }

    i = ctx_datalen;

    if (ctx_datalen < 56) {
        local_ctx_data[i++] = 0x80;
        while (i < 56)
            local_ctx_data[i++] = 0x00;
    } else {
        local_ctx_data[i++] = 0x80;
        while (i < 64)
            local_ctx_data[i++] = 0x00;
        local_sha256_transform(local_ctx_state, local_ctx_data);
        for (i = 0; i < 56; i++)
            local_ctx_data[i] = 0;
    }

    DBL_INT_ADD(local_ctx_bitlen[0], local_ctx_bitlen[1], ctx_datalen * 8);
    local_ctx_data[63] = local_ctx_bitlen[0];
    local_ctx_data[62] = local_ctx_bitlen[0] >> 8;
    local_ctx_data[61] = local_ctx_bitlen[0] >> 16;
    local_ctx_data[60] = local_ctx_bitlen[0] >> 24;
    local_ctx_data[59] = local_ctx_bitlen[1];
    local_ctx_data[58] = local_ctx_bitlen[1] >> 8;
    local_ctx_data[57] = local_ctx_bitlen[1] >> 16;
    local_ctx_data[56] = local_ctx_bitlen[1] >> 24;
    local_sha256_transform(local_ctx_state, local_ctx_data);

    for (i = 0; i < 4; ++i) {
        hash[i] = (local_ctx_state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (local_ctx_state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (local_ctx_state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (local_ctx_state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (local_ctx_state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (local_ctx_state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (local_ctx_state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (local_ctx_state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

void init(void)
{
    int64_t seed = block_counter * 1000;
    
    // Initialize hash data with pseudo-random values
    for (int i = 0; i < HASH_DATA_LEN; i++) {
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFFFFFFFFFF;
        uint8_t val = (uint8_t)(seed & 0xFF);
        hash_data[i] = val;
        hash_data_DWC[i] = val;
        hash_data_TMR[i] = val;
    }
    
    // Clear context and result buffers
    for (int i = 0; i < 64; i++) {
        ctx_data[i] = 0;
        ctx_data_DWC[i] = 0;
        ctx_data_TMR[i] = 0;
    }
    
    for (int i = 0; i < 8; i++) {
        ctx_state[i] = 0;
        ctx_state_DWC[i] = 0;
        ctx_state_TMR[i] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        ctx_bitlen[i] = 0;
        ctx_bitlen_DWC[i] = 0;
        ctx_bitlen_TMR[i] = 0;
    }
    
    for (int i = 0; i < 32; i++) {
        hashResult[i] = 0;
        hashResult_DWC[i] = 0;
        hashResult_TMR[i] = 0;
    }
}

void test(void)
{
    sha256();
}

void fill(void)
{
    // Compute golden hash
    local_sha256_hash(hash_data, HASH_DATA_LEN, golden_hash);
}

void check(void)
{
    // Check if computed hash matches golden
    for (int i = 0; i < 32; i++) {
        if (hashResult[i] != golden_hash[i]) {
            errors++;
        }
    }
}

void send_status(unsigned int cycles)
{
    // BLOCK RUN ERRORS CYCLES TMR_MITIGATIONS
    printf("%u\t%u\t%u\t%u\t%u\n", block_counter, run_counter, cycles, errors, TMR_ERROR_CNT);
}

