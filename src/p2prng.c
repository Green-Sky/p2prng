#include "./p2prng.h"

#include <sodium.h>

#include <assert.h>

static_assert(P2PRNG_MAC_KEY_LEN == crypto_auth_KEYBYTES, "key sizes differ");
static_assert(P2PRNG_MAC_LEN == crypto_auth_BYTES, "mac sizes differ");

static_assert(P2PRNG_MAC_KEY_LEN == P2PRNG_LEN, "key not 256");
static_assert(P2PRNG_MAC_LEN == P2PRNG_LEN, "mac not 256");

static_assert(P2PRNG_COMBINE_LEN == P2PRNG_LEN, "combine not 256");

int p2prng_auth_create(uint8_t out_key[P2PRNG_MAC_KEY_LEN], uint8_t out_mac[P2PRNG_MAC_LEN], const uint8_t* msg, uint32_t msg_len) {
	// could replace with randombytes_buf() to avoid key size check
	crypto_auth_keygen(out_key);

	return crypto_auth(out_mac, msg, msg_len, out_key);
}

int p2prng_auth_verify(const uint8_t key[P2PRNG_MAC_KEY_LEN], const uint8_t mac[P2PRNG_MAC_LEN], const uint8_t* msg, uint32_t msg_len) {
	return crypto_auth_verify(mac, msg, msg_len, key);
}


int p2prng_combine_init(uint8_t out_hash[P2PRNG_COMBINE_LEN], const uint8_t* msg, uint32_t msg_len) {
	return crypto_generichash(out_hash, P2PRNG_COMBINE_LEN, msg, msg_len, NULL, 0);
}

int p2prng_combine_update(uint8_t out_hash[P2PRNG_COMBINE_LEN], const uint8_t prev_hash[P2PRNG_COMBINE_LEN], const uint8_t* msg, uint32_t msg_len) {
	// either combine msg and hash, or use streaming interface

	/*alignas(64)*/ crypto_generichash_state state;

	// TODO: we could use the first part as key here, should be functionally identical
	// or we hardcode a custom key here, mostly for vanity
	int res = crypto_generichash_init(&state, NULL, 0, P2PRNG_COMBINE_LEN);
	if (res != 0) {
		return res;
	}

	// H(msg || Hprev)

	// first msg
	res = crypto_generichash_update(&state, msg, msg_len);
	if (res != 0) {
		return res;
	}

	// then hash from prev round
	res = crypto_generichash_update(&state, prev_hash, P2PRNG_COMBINE_LEN);
	if (res != 0) {
		return res;
	}

	return crypto_generichash_final(&state, out_hash, P2PRNG_COMBINE_LEN);
}

int p2prng_gen_and_auth(uint8_t out_rng[P2PRNG_LEN], uint8_t out_key[P2PRNG_MAC_KEY_LEN], uint8_t out_mac[P2PRNG_MAC_LEN], const uint8_t* is, uint32_t is_len) {
	// generate a (hopefully) cryptogrpahiclly random number
	randombytes_buf(out_rng, P2PRNG_LEN);

	// combine IS into rng
	int res = p2prng_combine_update(out_rng, out_rng, is, is_len);
	if (res != 0) {
		return res;
	}

	return p2prng_auth_create(out_key, out_mac, out_rng, P2PRNG_LEN);
}
