/*
 * OpenPairing is an implementation of a bilinear pairing over OpenSSL
 * Copyright (C) 2015 OpenPairing Authors
 *
 * This file is part of OpenPairing. OpenPairing is legal property of its
 * developers, whose names are not listed here. Please refer to the COPYRIGHT
 * file for contact information.
 *
 * OpenPairing is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * OpenPairing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenPairing. If not, see <http://www.gnu.org/licenses/>.
 */

#include "op.h"

void FP12_init(FP12 *a) {
	FP6_init(&a->f[0]);
	FP6_init(&a->f[1]);
}

void FP12_free(FP12 *a) {
	FP6_free(&a->f[0]);
	FP6_free(&a->f[1]);
}

int FP12_rand(FP12 *a) {
	if (!FP6_rand(&a->f[0])) {
		return 0;
	}
	if (!FP6_rand(&a->f[1])) {
		return 0;
	}
	return 1;
}

void FP12_print(FP12 *a) {
	FP6_print(&a->f[0]);
	FP6_print(&a->f[1]);
}

int FP12_zero(FP12 *a) {
	if (!FP6_zero(&a->f[0])) {
		return 0;
	}
	if (!FP6_zero(&a->f[1])) {
		return 0;
	}
	return 1;
}

int FP12_cmp(FP12 *a, FP12 *b) {
	if (FP6_cmp(&a->f[0], &b->f[0]) != 0) {
		return 1;
	}
	if (FP6_cmp(&a->f[1], &b->f[1]) != 0) {
		return 1;
	}
	return 0;
}

void FP12_copy(FP12 *a, FP12 *b) {
	FP6_copy(&a->f[0], &b->f[0]);
	FP6_copy(&a->f[1], &b->f[1]);
}

int FP12_is_zero(FP12 *a) {
	return FP6_is_zero(&a->f[0]) && FP6_is_zero(&a->f[1]);
}

int FP12_add(FP12 *r, FP12 *a, FP12 *b) {
	if (!FP6_add(&r->f[0], &a->f[0], &b->f[0])) {
		return 0;
	}
	if (!FP6_add(&r->f[1], &a->f[1], &b->f[1])) {
		return 0;
	}
	return 1;
}

int FP12_sub(FP12 *r, FP12 *a, FP12 *b) {
	if (!FP6_sub(&r->f[0], &a->f[0], &b->f[0])) {
		return 0;
	}
	if (!FP6_sub(&r->f[1], &a->f[1], &b->f[1])) {
		return 0;
	}
	return 1;
}

int FP12_neg(FP12 *r, FP12 *a) {
	if (!FP6_neg(&r->f[0], &a->f[0])) {
		return 0;
	}
	if (!FP6_neg(&r->f[1], &a->f[1])) {
		return 0;
	}	
	return 1;
}

int FP12_mul(FP12 *r, FP12 *a, FP12 *b) {
	FP6 t0, t1, t2;
	int code = 0;

	FP6_init(&t0);
	FP6_init(&t1);
	FP6_init(&t2);

	/* Karatsuba algorithm. */

	/* t0 = a_0 * b_0. */
	if (!FP6_mul(&t0, &a->f[0], &b->f[0])) {
		goto err;
	}
	/* t1 = a_1 * b_1. */
	if (!FP6_mul(&t1, &a->f[1], &b->f[1])) {
		goto err;
	}
	/* t2 = b_0 + b_1. */
	if (!FP6_add(&t2, &b->f[0], &b->f[1])) {
		goto err;
	}

	/* c_1 = a_0 + a_1. */
	if (!FP6_add(&r->f[1], &a->f[0], &a->f[1])) {
		goto err;
	}

	/* c_1 = (a_0 + a_1) * (b_0 + b_1) */
	if (!FP6_mul(&r->f[1], &r->f[1], &t2)) {
		goto err;
	}
	if (!FP6_sub(&r->f[1], &r->f[1], &t0)) {
		goto err;
	}
	if (!FP6_sub(&r->f[1], &r->f[1], &t1)) {
		goto err;
	}

	/* c_0 = a_0b_0 + v * a_1b_1. */
	if (!FP6_mul_art(&t1, &t1)) {
		goto err;
	}
	if (!FP6_add(&r->f[0], &t0, &t1)) {
		goto err;
	}

	code = 1;

err:
	FP6_free(&t0);
	FP6_free(&t1);
	FP6_free(&t2);
	return code;
}

int FP12_mul_dxs(FP12 *r, FP12 *a, FP12 *b) {
	FP6 t0, t1, t2;
	int code = 0;

	FP6_init(&t0);
	FP6_init(&t1);
	FP6_init(&t2);

	/* t0 = a_0 * b_0 */
	if (!FP2_mul(&t0.f[0], &a->f[0].f[0], &b->f[0].f[0])) {
		goto err;
	}
	if (!FP2_mul(&t0.f[1], &a->f[0].f[1], &b->f[0].f[0])) {
		goto err;
	}
	if (!FP2_mul(&t0.f[2], &a->f[0].f[2], &b->f[0].f[0])) {
		goto err;
	}

	/* t2 = b_0 + b_1. */
	if (!FP2_add(&t2.f[0], &b->f[0].f[0], &b->f[1].f[0])) {
		goto err;
	}
	FP2_copy(&t2.f[1], &b->f[1].f[1]);

	/* t1 = a_1 * b_1. */
	if (!FP6_mul_dxs(&t1, &a->f[1], &b->f[1])) {
		goto err;
	}
	/* c_1 = a_0 + a_1. */
	if (!FP6_add(&r->f[1], &a->f[0], &a->f[1])) {
		goto err;
	}
	/* c_1 = (a_0 + a_1) * (b_0 + b_1) - a_0 * b_0 - a_1 * b_1. */
	if (!FP6_mul_dxs(&r->f[1], &r->f[1], &t2)) {
		goto err;
	}
	if (!FP6_sub(&r->f[1], &r->f[1], &t0)) {
		goto err;
	}
	if (!FP6_sub(&r->f[1], &r->f[1], &t1)) {
		goto err;
	}
	/* c_0 = a_0 * b_0 + v * a_1 * b_1. */
	if (!FP6_mul_art(&t1, &t1)) {
		goto err;
	}
	if (!FP6_add(&r->f[0], &t0, &t1)) {
		goto err;
	}

	code = 1;
err:
	FP6_free(&t0);
	FP6_free(&t1);
	FP6_free(&t2);
	return code;
}

int FP12_inv(FP12 *c, FP12 *a) {
	FP6 t0, t1;
	int code = 0;

	FP6_init(&t0);
	FP6_init(&t1);

	if (!FP6_sqr(&t0, &a->f[0])) {
		goto err;
	}
	if (!FP6_sqr(&t1, &a->f[1])) {
		goto err;
	}	
	if (!FP6_mul_art(&t1, &t1)) {
		goto err;
	}
	if (!FP6_sub(&t0, &t0, &t1)) {
		goto err;
	}
	if (!FP6_inv(&t0, &t0)) {
		goto err;
	}
	if (!FP6_mul(&c->f[0], &a->f[0], &t0)) {
		goto err;
	}
	if (!FP6_neg(&c->f[1], &a->f[1])) {
		goto err;
	}
	if (!FP6_mul(&c->f[1], &c->f[1], &t0)) {
		goto err;
	}

	code = 1;

err:
	FP6_free(&t0);
	FP6_free(&t1);
	return code;
}

int FP12_inv_uni(FP12 *c, FP12 *a) {
	FP6_copy(&c->f[0], &a->f[0]);
	if (!FP6_neg(&c->f[1], &a->f[1])) {
		return 0;
	}
	return 1;
}

int FP12_frb(FP12 *r, FP12 *a) {
	int code = 0;
	if (!FP2_inv_uni(&r->f[0].f[0], &a->f[0].f[0])) {
		goto err;
	}
	if (!FP2_inv_uni(&r->f[1].f[0], &a->f[1].f[0])) {
		goto err;
	}
	if (!FP2_inv_uni(&r->f[0].f[1], &a->f[0].f[1])) {
		goto err;
	}
	if (!FP2_inv_uni(&r->f[1].f[1], &a->f[1].f[1])) {
		goto err;
	}
	if (!FP2_inv_uni(&r->f[0].f[2], &a->f[0].f[2])) {
		goto err;
	}
	if (!FP2_inv_uni(&r->f[1].f[2], &a->f[1].f[2])) {
		goto err;
	}
	if (!FP2_mul_frb(&r->f[1].f[0], &r->f[1].f[0], 1)) {
		goto err;
	}
	if (!FP2_mul_frb(&r->f[0].f[1], &r->f[0].f[1], 2)) {
		goto err;
	}
	if (!FP2_mul_frb(&r->f[1].f[1], &r->f[1].f[1], 3)) {
		goto err;
	}
	if (!FP2_mul_frb(&r->f[0].f[2], &r->f[0].f[2], 4)) {
		goto err;
	}
	if (!FP2_mul_frb(&r->f[1].f[2], &r->f[1].f[2], 5)) {
		goto err;
	}
	code = 1;
err:
	return code;
}

int FP12_cyc(FP12 *r, FP12 *a) {
	FP12 t;
	int code = 0;

	FP12_init(&t);

	if (!FP12_inv(&t, a)) {
		goto err;
	}
	if (!FP12_inv_uni(r, a)) {
		goto err;
	}
	if (!FP12_mul(r, r, &t)) {
		goto err;
	}

	if (!FP12_frb(&t, r)) {
		goto err;
	}	
	if (!FP12_frb(&t, &t)) {
		goto err;
	}
	if (!FP12_mul(r, r, &t)) {
		goto err;
	}

	code = 1;
err:
	FP12_free(&t);
	return code;
}

int FP12_exp_cyc(FP12 *r, FP12 *a) {
	int i, code = 0;
	FP12 t0, t1;

	FP12_init(&t0);
	FP12_init(&t1);

	FP12_copy(&t0, a);
	for (i = 0; i < 55; i++) {
		if (!FP12_sqr_pck(&t0, &t0)) {
			goto err;
		}
	}
	FP12_copy(&t1, &t0);
	for (i = 55; i < 62; i++) {
		if (!FP12_sqr_pck(&t1, &t1)) {
			goto err;
		}
	}
	if (!FP12_back(&t0, &t1, &t0, &t1)) {
		goto err;
	}

	if (!FP12_mul(&t0, &t0, &t1)) {
		goto err;
	}
	if (!FP12_mul(r, &t0, a)) {
		goto err;
	}

	code = 1;
err:
	FP12_free(&t0);
	FP12_free(&t1);
	return code;
}

int FP12_sqr(FP12 *r, FP12 *a) {
	FP6 t0, t1;
	int code = 0;

	FP6_init(&t0);
	FP6_init(&t1);

	if (!FP6_add(&t0, &a->f[0], &a->f[1])) {
		goto err;
	}

	if (!FP6_add(&t0, &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP6_mul_art(&t1, &a->f[1])) {
		goto err;
	}
	if (!FP6_add(&t1, &a->f[0], &t1)) {
		goto err;
	}
	if (!FP6_mul(&t0, &t0, &t1)) {
		goto err;
	}
	if (!FP6_mul(&r->f[1], &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP6_sub(&r->f[0], &t0, &r->f[1])) {
		goto err;
	}
	if (!FP6_mul_art(&t1, &r->f[1])) {
		goto err;
	}
	if (!FP6_sub(&r->f[0], &r->f[0], &t1)) {
		goto err;
	}
	if (!FP6_add(&r->f[1], &r->f[1], &r->f[1])) {
		goto err;
	}
	code = 1;
err:
	FP6_free(&t0);
	FP6_free(&t1);
	return code;
}

int FP12_sqr_pck(FP12 *r, FP12 *a) {
	FP2 t0, t1, t2, t3, t4, t5, t6;
	int code;

	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);
	FP2_init(&t3);
	FP2_init(&t4);
	FP2_init(&t5);
	FP2_init(&t6);

	if (!FP2_sqr(&t0, &a->f[0].f[1])) {
		goto err;
	}
	if (!FP2_sqr(&t1, &a->f[1].f[2])) {
		goto err;
	}
	if (!FP2_add(&t5, &a->f[0].f[1], &a->f[1].f[2])) {
		goto err;
	}
	if (!FP2_sqr(&t2, &t5)) {
		goto err;
	}

	if (!FP2_add(&t3, &t0, &t1)) {
		goto err;
	}
	if (!FP2_sub(&t5, &t2, &t3)) {
		goto err;
	}

	if (!FP2_add(&t6, &a->f[1].f[0], &a->f[0].f[2])) {
		goto err;
	}
	if (!FP2_sqr(&t3, &t6)) {
		goto err;
	}
	if (!FP2_sqr(&t2, &a->f[1].f[0])) {
		goto err;
	}

	if (!FP2_mul_nor(&t6, &t5)) {
		goto err;
	}
	if (!FP2_add(&t5, &t6, &a->f[1].f[0])) {
		goto err;
	}
	if (!FP2_add(&t5, &t5, &t5)) {
		goto err;
	}
	if (!FP2_add(&r->f[1].f[0], &t5, &t6)) {
		goto err;
	}

	if (!FP2_mul_nor(&t4, &t1)) {
		goto err;
	}
	if (!FP2_add(&t5, &t0, &t4)) {
		goto err;
	}
	if (!FP2_sub(&t6, &t5, &a->f[0].f[2])){
		goto err;
	}

	if (!FP2_sqr(&t1, &a->f[0].f[2])) {
		goto err;
	}

	if (!FP2_add(&t6, &t6, &t6)) {
		goto err;
	}
	if (!FP2_add(&r->f[0].f[2], &t5, &t6)) {
		goto err;
	}

	if (!FP2_mul_nor(&t4, &t1)) {
		goto err;
	}
	if (!FP2_add(&t5, &t2, &t4)) {
		goto err;
	}
	if (!FP2_sub(&t6, &t5, &a->f[0].f[1])){
		goto err;
	}
	if (!FP2_add(&t6, &t6, &t6)) {
		goto err;
	}
	if (!FP2_add(&r->f[0].f[1], &t5, &t6)) {
		goto err;
	}

	if (!FP2_add(&t0, &t2, &t1)) {
		goto err;
	}
	if (!FP2_sub(&t5, &t3, &t0)){
		goto err;
	}
	if (!FP2_add(&t6, &t5, &a->f[1].f[2])) {
		goto err;
	}
	if (!FP2_add(&t6, &t6, &t6)) {
		goto err;
	}	
	if (!FP2_add(&r->f[1].f[2], &t5, &t6)) {
		goto err;
	}

	code = 1;
err:
	FP2_free(&t0);
	FP2_free(&t1);
	FP2_free(&t2);
	FP2_free(&t3);
	FP2_free(&t4);
	FP2_free(&t5);
	FP2_free(&t6);
	return code;
}

int FP12_back(FP12 *r, FP12 *s, FP12 *a, FP12 *b) {
	FP2 t0[2], t1[2], t2[2];
	FP12 t[2], u[2];
	int i, code;

	FP2_init(&t0[0]);
	FP2_init(&t0[1]);
	FP2_init(&t1[0]);
	FP2_init(&t1[1]);
	FP2_init(&t2[0]);
	FP2_init(&t2[1]);
	FP12_init(&t[0]);
	FP12_init(&t[1]);
	FP12_init(&u[0]);
	FP12_init(&u[1]);

	FP12_copy(&u[0], a);
	FP12_copy(&u[1], b);

	for (i = 0; i < 2; i++) {
		/* t0 = g4^2. */
		if (!FP2_sqr(&t0[i], &u[i].f[0].f[1])) {
			goto err;
		}
		/* t1 = 3 * g4^2 - 2 * g3. */
		if (!FP2_sub(&t1[i], &t0[i], &u[i].f[0].f[2])) {
			goto err;
		}
		if (!FP2_add(&t1[i], &t1[i], &t1[i])) {
			goto err;
		}
		if (!FP2_add(&t1[i], &t1[i], &t0[i])) {
			goto err;
		}
		/* t0 = E * g5^2 + t1. */
		if (!FP2_sqr(&t2[i], &u[i].f[1].f[2])) {
			goto err;
		}
		if (!FP2_mul_nor(&t0[i], &t2[i])) {
			goto err;
		}
		if (!FP2_add(&t0[i], &t0[i], &t1[i])) {
			goto err;
		}
		/* t1 = (4 * g2). */
		if (!FP2_add(&t1[i], &u[i].f[1].f[0], &u[i].f[1].f[0])) {
			goto err;
		}
		if (!FP2_add(&t1[i], &t1[i], &t1[i])) {
			goto err;
		}
	}

	/* t1 = 1 / t1. */
	if (!FP2_inv_sim(&t1[0], &t1[1], &t1[0], &t1[1])) {
		goto err;
	}

	for (i = 0; i < 2; i++) {
		/* t0 = g1. */
		if (!FP2_mul(&t[i].f[1].f[1], &t0[i], &t1[i])) {
			goto err;
		}
		/* t1 = g3 * g4. */
		if (!FP2_mul(&t1[i], &u[i].f[0].f[2], &u[i].f[0].f[1])) {
			goto err;
		}
		/* t2 = 2 * g1^2 - 3 * g3 * g4. */
		if (!FP2_sqr(&t2[i], &t[i].f[1].f[1])) {
			goto err;
		}
		if (!FP2_sub(&t2[i], &t2[i], &t1[i])) {
			goto err;
		}
		if (!FP2_add(&t2[i], &t2[i], &t2[i])) {
			goto err;
		}
		if (!FP2_sub(&t2[i], &t2[i], &t1[i])) {
			goto err;
		}
		/* t1 = g2 * g5. */
		if (!FP2_mul(&t1[i], &u[i].f[1].f[0], &u[i].f[1].f[2])) {
			goto err;
		}
		/* t2 = E * (2 * g1^2 + g2 * g5 - 3 * g3 * g4) + 1. */
		if (!FP2_add(&t2[i], &t2[i], &t1[i])) {
			goto err;
		}
		if (!FP2_mul_nor(&t[i].f[0].f[0], &t2[i])) {
			goto err;
		}
		if (!BN_add(&t[i].f[0].f[0].f[0], &t[i].f[0].f[0].f[0], ctx.one)) {
			goto err;
		}
		FP2_copy(&t[i].f[0].f[1], &u[i].f[0].f[1]);
		FP2_copy(&t[i].f[0].f[2], &u[i].f[0].f[2]);
		FP2_copy(&t[i].f[1].f[0], &u[i].f[1].f[0]);
		FP2_copy(&t[i].f[1].f[2], &u[i].f[1].f[2]);
	}

	FP12_copy(r, &t[0]);
	FP12_copy(s, &t[1]);

	code = 1;
err:
	FP2_free(&t0[0]);
	FP2_free(&t0[1]);
	FP2_free(&t1[0]);
	FP2_free(&t1[1]);
	FP2_free(&t2[0]);
	FP2_free(&t2[1]);
	FP12_free(&t[0]);
	FP12_free(&t[1]);
	FP12_free(&u[0]);
	FP12_free(&u[1]);
	return code;
}
