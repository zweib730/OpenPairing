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

void FP6_init(FP6 *a) {
	FP2_init(&a->f[0]);
	FP2_init(&a->f[1]);
	FP2_init(&a->f[2]);
}

void FP6_free(FP6 *a) {
	FP2_free(&a->f[0]);
	FP2_free(&a->f[1]);
	FP2_free(&a->f[2]);
}

int FP6_rand(const PAIRING_GROUP *group, FP6 *a) {
	if (!FP2_rand(group, &a->f[0])) {
		return 0;
	}
	if (!FP2_rand(group, &a->f[1])) {
		return 0;
	}
	if (!FP2_rand(group, &a->f[2])) {
		return 0;
	}	
	return 1;
}

void FP6_print(const FP6 *a) {
	FP2_print(&a->f[0]);
	FP2_print(&a->f[1]);
	FP2_print(&a->f[2]);
}

int FP6_zero(FP6 *a) {
	if (!FP2_zero(&a->f[0])) {
		return 0;
	}
	if (!FP2_zero(&a->f[1])) {
		return 0;
	}
	if (!FP2_zero(&a->f[2])) {
		return 0;
	}
	return 1;
}

int FP6_cmp(const FP6 *a, const FP6 *b) {
	if (FP2_cmp(&a->f[0], &b->f[0]) != 0) {
		return 1;
	}
	if (FP2_cmp(&a->f[1], &b->f[1]) != 0) {
		return 1;
	}
	if (FP2_cmp(&a->f[2], &b->f[2]) != 0) {
		return 1;
	}
	return 0;
}

void FP6_copy(FP6 *a, const FP6 *b) {
	FP2_copy(&a->f[0], &b->f[0]);
	FP2_copy(&a->f[1], &b->f[1]);
	FP2_copy(&a->f[2], &b->f[2]);
}

int FP6_is_zero(const FP6 *a) {
	return FP2_is_zero(&a->f[0]) && FP2_is_zero(&a->f[1]) && FP2_is_zero(&a->f[2]);
}

int FP6_add(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, const FP6 *b) {
	if (!FP2_add(group, &r->f[0], &a->f[0], &b->f[0])) {
		return 0;
	}
	if (!FP2_add(group, &r->f[1], &a->f[1], &b->f[1])) {
		return 0;
	}
	if (!FP2_add(group, &r->f[2], &a->f[2], &b->f[2])) {
		return 0;
	}
	return 1;
}

int FP6_sub(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, const FP6 *b) {
	if (!FP2_sub(group, &r->f[0], &a->f[0], &b->f[0])) {
		return 0;
	}
	if (!FP2_sub(group, &r->f[1], &a->f[1], &b->f[1])) {
		return 0;
	}
	if (!FP2_sub(group, &r->f[2], &a->f[2], &b->f[2])) {
		return 0;
	}
	return 1;
}

int FP6_neg(const PAIRING_GROUP *group, FP6 *r, const FP6 *a) {
	if (!FP2_neg(group, &r->f[0], &a->f[0])) {
		return 0;
	}
	if (!FP2_neg(group, &r->f[1], &a->f[1])) {
		return 0;
	}
	if (!FP2_neg(group, &r->f[2], &a->f[2])) {
		return 0;
	}
	return 1;
}

int FP6_mul(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, const FP6 *b, BN_CTX *ctx) {
	FP2 v0, v1, v2, t0, t1, t2;
	int ret = 0;

	FP2_init(&v0);
	FP2_init(&v1);
	FP2_init(&v2);
	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);

	/* v0 = a_0b_0 */
	if (!FP2_mul(group, &v0, &a->f[0], &b->f[0], ctx)) {
		goto err;
	}

	/* v1 = a_1b_1 */
	if (!FP2_mul(group, &v1, &a->f[1], &b->f[1], ctx)) {
		goto err;
	}

	/* v2 = a_2b_2 */
	if (!FP2_mul(group, &v2, &a->f[2], &b->f[2], ctx)) {
		goto err;
	}

	/* t2 (c_0) = v0 + E((a_1 + a_2)(b_1 + b_2) - v1 - v2) */
	if (!FP2_add(group, &t0, &a->f[1], &a->f[2])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[1], &b->f[2])) {
		goto err;
	}
	if (!FP2_mul(group, &t2, &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &t2, &t2, &v1)) {
		goto err;
	}
	if (!FP2_sub(group, &t2, &t2, &v2)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &t0, &t2, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t2, &t0, &v0)) {
		goto err;
	}

	/* c_1 = (a_0 + a_1)(b_0 + b_1) - v0 - v1 + Ev2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[0], &b->f[1])) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[1], &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v0)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v1)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &t0, &v2, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[1], &r->f[1], &t0)) {
		goto err;
	}

	/* c_2 = (a_0 + a_2)(b_0 + b_2) - v0 + v1 - v2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[2])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[0], &b->f[2])) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[2], &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &v0)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[2], &r->f[2], &v1)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &v2)) {
		goto err;
	}

	/* c_0 = t2 */
	FP2_copy(&r->f[0], &t2);

	ret = 1;

err:
	FP2_free(&t2);
	FP2_free(&t1);
	FP2_free(&t0);
	FP2_free(&v2);
	FP2_free(&v1);
	FP2_free(&v0);
}

int FP6_mul_unr(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, const FP6 *b, BN_CTX *ctx) {
	FP2 v0, v1, v2, t0, t1, t2;
	int ret = 0;

	FP2_init(&v0);
	FP2_init(&v1);
	FP2_init(&v2);
	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);

	/* v0 = a_0b_0 */
	if (!FP2_mul_unr(group, &v0, &a->f[0], &b->f[0], ctx)) {
		goto err;
	}

	/* v1 = a_1b_1 */
	if (!FP2_mul_unr(group, &v1, &a->f[1], &b->f[1], ctx)) {
		goto err;
	}

	/* v2 = a_2b_2 */
	if (!FP2_mul_unr(group, &v2, &a->f[2], &b->f[2], ctx)) {
		goto err;
	}

	/* t2 (c_0) = v0 + E((a_1 + a_2)(b_1 + b_2) - v1 - v2) */
	if (!FP2_add(group, &t0, &a->f[1], &a->f[2])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[1], &b->f[2])) {
		goto err;
	}
	if (!FP2_mul_unr(group, &t2, &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &t2, &t2, &v1)) {
		goto err;
	}
	if (!FP2_sub(group, &t2, &t2, &v2)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &t0, &t2, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t2, &t0, &v0)) {
		goto err;
	}

	/* c_1 = (a_0 + a_1)(b_0 + b_1) - v0 - v1 + Ev2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[0], &b->f[1])) {
		goto err;
	}
	if (!FP2_mul_unr(group, &r->f[1], &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v0)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v1)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &t0, &v2, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[1], &r->f[1], &t0)) {
		goto err;
	}

	/* c_2 = (a_0 + a_2)(b_0 + b_2) - v0 + v1 - v2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[2])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[0], &b->f[2])) {
		goto err;
	}
	if (!FP2_mul_unr(group, &r->f[2], &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &v0)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[2], &r->f[2], &v1)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &v2)) {
		goto err;
	}

	/* c_0 = t2 */
	FP2_copy(&r->f[0], &t2);

	ret = 1;

err:
	FP2_free(&t2);
	FP2_free(&t1);
	FP2_free(&t0);
	FP2_free(&v2);
	FP2_free(&v1);
	FP2_free(&v0);
	return ret;
}

int FP6_mul_dxs(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, const FP6 *b, BN_CTX *ctx) {
	FP2 v0, v1, v2, t0, t1, t2;
	int ret = 0;

	FP2_init(&v0);
	FP2_init(&v1);
	FP2_init(&v2);
	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);

	/* v0 = a_0b_0 */
	if (!FP2_mul(group, &v0, &a->f[0], &b->f[0], ctx)) {
		goto err;
	}

	/* v1 = a_1b_1 */
	if (!FP2_mul(group, &v1, &a->f[1], &b->f[1], ctx)) {
		goto err;
	}

	/* v2 = a_2b_2 */

	/* t2 (c_0) = v0 + E((a_1 + a_2)(b_1 + b_2) - v1 - v2) */
	if (!FP2_add(group, &t0, &a->f[1], &a->f[2])) {
		goto err;
	}
	if (!FP2_mul(group, &t0, &t0, &b->f[1], ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &t0, &t0, &v1)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &t2, &t0, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t2, &t2, &v0)) {
		goto err;
	}

	/* c_1 = (a_0 + a_1)(b_0 + b_1) - v0 - v1 + Ev2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP2_add(group, &t1, &b->f[0], &b->f[1])) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[1], &t0, &t1, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v0)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[1], &r->f[1], &v1)) {
		goto err;
	}

	/* c_2 = (a_0 + a_2)(b_0 + b_2) - v0 + v1 - v2 */
	if (!FP2_add(group, &t0, &a->f[0], &a->f[2])) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[2], &t0, &b->f[0], ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &v0)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[2], &r->f[2], &v1)) {
		goto err;
	}

	/* c_0 = t2 */
	FP2_copy(&r->f[0], &t2);

	ret = 1;

err:
	FP2_free(&t2);
	FP2_free(&t1);
	FP2_free(&t0);
	FP2_free(&v2);
	FP2_free(&v1);
	FP2_free(&v0);
	return ret;
}

int FP6_rdc(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, BN_CTX *ctx) {
	int ret = 0;

	/* c_0 = t1 mod p. */
	if (!FP2_rdc(group, &r->f[0], &a->f[0], ctx)) {
		goto err;
	}
	if (!FP2_rdc(group, &r->f[1], &a->f[1], ctx)) {
		goto err;
	}
	if (!FP2_rdc(group, &r->f[2], &a->f[2], ctx)) {
		goto err;
	}

	ret = 1;

err:
	return ret;
}

int FP6_mul_art(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, BN_CTX *ctx) {
	FP2 t0;
	int ret = 0;

	FP2_init(&t0);

	FP2_copy(&t0, &a->f[0]);
	if (!FP2_mul_nor(group, &r->f[0], &a->f[2], ctx)) {
		goto err;
	}
	FP2_copy(&r->f[2], &a->f[1]);
	FP2_copy(&r->f[1], &t0);

	ret = 1;

err:
	FP2_free(&t0);
	return ret;
}

int FP6_sqr(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, BN_CTX *ctx) {
	FP2 t0, t1, t2, t3, t4;
	int ret = 0;

	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);
	FP2_init(&t3);
	FP2_init(&t4);

	/* t0 = a_0^2 */
	if (!FP2_sqr(group, &t0, &a->f[0], ctx)) {
		goto err;
	}

	/* t1 = 2 * a_1 * a_2 */
	if (!FP2_mul(group, &t1, &a->f[1], &a->f[2], ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t1, &t1, &t1)) {
		goto err;
	}

	/* t2 = a_2^2. */
	if (!FP2_sqr(group, &t2, &a->f[2], ctx)) {
		goto err;
	}

	/* c2 = a_0 + a_2. */
	if (!FP2_add(group, &r->f[2], &a->f[0], &a->f[2])) {
		goto err;
	}

	/* t3 = (a_0 + a_2 + a_1)^2. */
	if (!FP2_add(group, &t3, &r->f[2], &a->f[1])) {
		goto err;
	}
	if (!FP2_sqr(group, &t3, &t3, ctx)) {
		goto err;
	}

	/* c2 = (a_0 + a_2 - a_1)^2. */
	if (!FP2_sub(group, &r->f[2], &r->f[2], &a->f[1])) {
		goto err;
	}
	if (!FP2_sqr(group, &r->f[2], &r->f[2], ctx)) {
		goto err;
	}

	/* c2 = (c2 + t3)/2. */
	if (!FP2_add(group, &r->f[2], &r->f[2], &t3)) {
		goto err;
	}
	if (BN_is_bit_set(&r->f[2].f[0], 0)) {
		if (!BN_add(&r->f[2].f[0], &r->f[2].f[0], group->field)) {
			goto err;
		}
	}
	if (!BN_rshift1(&r->f[2].f[0], &r->f[2].f[0])) {
		goto err;
	}
	if (BN_is_bit_set(&r->f[2].f[1], 0)) {
		if (!BN_add(&r->f[2].f[1], &r->f[2].f[1], group->field)) {
			goto err;
		}
	}
	if (!BN_rshift1(&r->f[2].f[1], &r->f[2].f[1])) {
		goto err;
	}

	/* t3 = t3 - c2 - t1. */
	if (!FP2_sub(group, &t3, &t3, &r->f[2])) {
		goto err;
	}
	if (!FP2_sub(group, &t3, &t3, &t1)) {
		goto err;
	}

	/* c2 = c2 - t0 - t2. */
	if (!FP2_sub(group, &r->f[2], &r->f[2], &t0)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &t2)) {
		goto err;
	}

	/* c0 = t0 + t1 * E. */
	if (!FP2_mul_nor(group, &t4, &t1, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[0], &t0, &t4)) {
		goto err;
	}

	/* c1 = t3 + t2 * E. */
	if (!FP2_mul_nor(group, &t4, &t2, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[1], &t3, &t4)) {
		goto err;
	}

	ret = 1;
err:
	FP2_free(&t0);
	FP2_free(&t1);
	FP2_free(&t2);
	FP2_free(&t3);
	FP2_free(&t4);
	return ret;
}

int FP6_sqr2(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, BN_CTX *ctx) {
	FP2 t0, t1, t2, t3, t4;
	int ret = 0;

	FP2_init(&t0);
	FP2_init(&t1);
	FP2_init(&t2);
	FP2_init(&t3);
	FP2_init(&t4);

	/* t0 = a_0^2 */
	if (!FP2_sqr(group, &t0, &a->f[0], ctx)) {
		goto err;
	}

	/* t1 = 2 * a_0 * a_1 */
	if (!FP2_mul(group, &t1, &a->f[0], &a->f[1], ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t1, &t1, &t1)) {
		goto err;
	}

	/* t2 = (a_0 - a_1 + a_2)^2 */
	if (!FP2_sub(group, &t2, &a->f[0], &a->f[1])) {
		goto err;
	}
	if (!FP2_add(group, &t2, &t2, &a->f[2])) {
		goto err;
	}
	if (!FP2_sqr(group, &t2, &t2, ctx)) {
		goto err;
	}

	/* t3 = 2 * a_1 * a_2 */
	if (!FP2_mul(group, &t3, &a->f[1], &a->f[2], ctx)) {
		goto err;
	}
	if (!FP2_add(group, &t3, &t3, &t3)) {
		goto err;
	}

	/* t4 = a_2^2 */
	if (!FP2_sqr(group, &t4, &a->f[2], ctx)) {
		goto err;
	}

	/* c_0 = t0 + E * t3 */
	if (!FP2_mul_nor(group, &r->f[0], &t3, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[0], &r->f[0], &t0)) {
		goto err;
	}

	/* c_1 = t1 + E * t4 */
	if (!FP2_mul_nor(group, &r->f[1], &t4, ctx)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[1], &r->f[1], &t1)) {
		goto err;
	}

	/* c_2 = t1 + t2 + t3 - t0 - t4 */
	if (!FP2_add(group, &r->f[2], &t1, &t2)) {
		goto err;
	}
	if (!FP2_add(group, &r->f[2], &r->f[2], &t3)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &t0)) {
		goto err;
	}
	if (!FP2_sub(group, &r->f[2], &r->f[2], &t4)) {
		goto err;
	}

	ret = 1;

err:
	FP2_free(&t0);
	FP2_free(&t1);
	FP2_free(&t2);
	FP2_free(&t3);
	FP2_free(&t4);
	return ret;
}

int FP6_inv(const PAIRING_GROUP *group, FP6 *r, const FP6 *a, BN_CTX *ctx) {
	FP2 v0, v1, v2, t0;
	int ret;

	FP2_init(&v0);
	FP2_init(&v1);
	FP2_init(&v2);
	FP2_init(&t0);

	/* v0 = a_0^2 - E * a_1 * a_2. */
	if (!FP2_sqr(group, &t0, &a->f[0], ctx)) {
		goto err;
	}
	if (!FP2_mul(group, &v0, &a->f[1], &a->f[2], ctx)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &v2, &v0, ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &v0, &t0, &v2)) {
		goto err;
	}

	/* v1 = E * a_2^2 - a_0 * a_1. */
	if (!FP2_sqr(group, &t0, &a->f[2], ctx)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &v2, &t0, ctx)) {
		goto err;
	}
	if (!FP2_mul(group, &v1, &a->f[0], &a->f[1], ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &v1, &v2, &v1)) {
		goto err;
	}

	/* v2 = a_1^2 - a_0 * a_2. */
	if (!FP2_sqr(group, &t0, &a->f[1], ctx)) {
		goto err;
	}
	if (!FP2_mul(group, &v2, &a->f[0], &a->f[2], ctx)) {
		goto err;
	}
	if (!FP2_sub(group, &v2, &t0, &v2)) {
		goto err;
	}

	if (!FP2_mul(group, &t0, &a->f[1], &v2, ctx)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &r->f[1], &t0, ctx)) {
		goto err;
	}

	if (!FP2_mul(group, &r->f[0], &a->f[0], &v0, ctx)) {
		goto err;
	}

	if (!FP2_mul(group, &t0, &a->f[2], &v1, ctx)) {
		goto err;
	}
	if (!FP2_mul_nor(group, &r->f[2], &t0, ctx)) {
		goto err;
	}

	if (!FP2_add(group, &t0, &r->f[0], &r->f[1])) {
		goto err;
	}
	if (!FP2_add(group, &t0, &t0, &r->f[2])) {
		goto err;
	}
	if (!FP2_inv(group, &t0, &t0, ctx)) {
		goto err;
	}

	if (!FP2_mul(group, &r->f[0], &v0, &t0, ctx)) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[1], &v1, &t0, ctx)) {
		goto err;
	}
	if (!FP2_mul(group, &r->f[2], &v2, &t0, ctx)) {
		goto err;
	}

	ret = 1;
err:
	FP2_free(&v0);
	FP2_free(&v1);
	FP2_free(&v2);
	FP2_free(&t0);
	return ret;
}
