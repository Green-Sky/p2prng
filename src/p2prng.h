#ifndef P2PRNG_H
#define P2PRNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// # generate random number and mac
// IS should include all peers participating, ideally with a unique id
//
// usage example:
//   uint8_t self_rng[P2PRNG_LEN];
//   uint8_t self_key[P2PRNG_MAC_KEY_LEN];
//   p2prng_gen_and_auth(self_rng, self_key, self_mac, is, is_len);
//   // wait for all other macs to arrive <here>
//   for (int i = 0; i < peer_count; i++) {
//       if (p2prng_auth_verify(peer_mac_key[i], peer_mac[i], peer_msgs[i], peer_msg_len[i]) != 0) {
//           // validation error, optionally rerequest
//           exit(1);
//       }
//   }
//   // proceed with rng combining

// # combine the random numbers and IS
// call with IS last
//
// usage example:
//   uint8_t final_hash[P2PRNG_COMBINE_LEN];
//   p2prng_combine_init(final_hash, peer_msgs[0], peer_msg_len[0]);
//   for (int i = 1; i < peer_count; i++) {
//       p2prng_combine_update(final_hash, final_hash, peer_msgs[i], peer_msg_len[i]);
//   }
//   p2prng_combine_update(final_hash, final_hash, is, is_len);

#define P2PRNG_LEN 32u
#define P2PRNG_MAC_KEY_LEN 32u
#define P2PRNG_MAC_LEN 32u
#define P2PRNG_COMBINE_LEN 32u

// TODO: merge key into msg
int p2prng_auth_create(uint8_t out_key[P2PRNG_MAC_KEY_LEN], uint8_t out_mac[P2PRNG_MAC_LEN], const uint8_t* msg, uint32_t msg_len);
int p2prng_auth_verify(const uint8_t key[P2PRNG_MAC_KEY_LEN], const uint8_t mac[P2PRNG_MAC_LEN], const uint8_t* msg, uint32_t msg_len);

int p2prng_combine_init(uint8_t out_hash[P2PRNG_COMBINE_LEN], const uint8_t* msg, uint32_t msg_len);
int p2prng_combine_update(uint8_t out_hash[P2PRNG_COMBINE_LEN], const uint8_t prev_hash[P2PRNG_COMBINE_LEN], const uint8_t* msg, uint32_t msg_len);

// helper that:
// - generates the random number (msg)
// - calls p2prng_auth_create()
int p2prng_gen_and_auth(uint8_t out_rng[P2PRNG_LEN], uint8_t out_key[P2PRNG_MAC_KEY_LEN], uint8_t out_mac[P2PRNG_MAC_LEN], const uint8_t* is, uint32_t is_len);

#ifdef __cplusplus
}
#endif

#endif // P2PRNG_H
