// 
// 
// 

#include "Reed_Solomon.h"

static int pp[RS_MM + 1] = { 1, 0, 1, 1, 0, 0, 0, 1, 1 }; /* GF(2^8)‚Ìirreducible polynominal */
static int alpha_to[RS_NN + 1], index_of[RS_NN + 1], gg[RS_NN - RS_KK + 1];
static int recd[RS_NN];
//static int data[RS_KK];
//static int bb[RS_NN - RS_KK];

void Reed_SolomonClass::init()
{
	generate_gf();
	gen_poly();

	return;
}

void Reed_SolomonClass::generate_gf(void)
{
	register int i, mask;
	unsigned int mm = RS_MM;
	unsigned int nn = RS_NN;
	unsigned int tt = RS_TT;
	unsigned int kk = RS_KK;

	mask = 1;
	alpha_to[mm] = 0;
	for (i = 0; i<mm; i++) {
		alpha_to[i] = mask;
		index_of[alpha_to[i]] = i;
		if (pp[i] != 0)
			alpha_to[mm] ^= mask;
		mask <<= 1;
	}
	index_of[alpha_to[mm]] = mm;
	mask >>= 1;
	for (i = mm + 1; i<nn; i++) {
		if (alpha_to[i - 1] >= mask)
			alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i - 1] ^ mask) << 1);
		else alpha_to[i] = alpha_to[i - 1] << 1;
		index_of[alpha_to[i]] = i;
	}
	index_of[0] = -1;
}

void Reed_SolomonClass::gen_poly(void)
{
	register int i, j;
	unsigned int mm = RS_MM;
	unsigned int nn = RS_NN;
	unsigned int tt = RS_TT;
	unsigned int kk = RS_KK;

	gg[0] = 2;
	gg[1] = 1;
	for (i = 2; i <= nn - kk; i++) {
		gg[i] = 1;
		for (j = i - 1; j>0; j--)
			if (gg[j] != 0)  gg[j] = gg[j - 1] ^ alpha_to[(index_of[gg[j]] + i) % nn];
			else gg[j] = gg[j - 1];
			gg[0] = alpha_to[(index_of[gg[0]] + i) % nn];
	}

	for (i = 0; i <= nn - kk; i++)  gg[i] = index_of[gg[i]];
}

void Reed_SolomonClass::encode(unsigned char *data, unsigned int size, unsigned char *bb, unsigned int *rs_size) {
	register int i, j;
	int feedback;

	unsigned int mm = RS_MM;
	unsigned int nn = RS_NN;
	unsigned int tt = RS_TT;
	unsigned int kk = RS_KK;

	generate_gf();
	gen_poly();

	*rs_size = nn - kk;

	for (i = 0; i<nn - kk; i++)   bb[i] = 0;
	for (i = kk - 1; i >= 0; i--) {
		feedback = index_of[data[i] ^ bb[nn - kk - 1]];
		if (feedback != -1) {
			for (j = nn - kk - 1; j>0; j--)
				if (gg[j] != -1)
					bb[j] = bb[j - 1] ^ alpha_to[(gg[j] + feedback) % nn];
				else
					bb[j] = bb[j - 1];
			bb[0] = alpha_to[(gg[0] + feedback) % nn];
		}
		else {
			for (j = nn - kk - 1; j>0; j--)
				bb[j] = bb[j - 1];
			bb[0] = 0;
		};
	};
}
Reed_SolomonClass Reed_Solomon;

