# Fair and temper-proof random-number-generation in a distributed environment

As long as you can contribute an unpredictable (random) number of your choosing, the algorithm will make sure the outcome is unpredictable.


First a public "vote" is held, where everyone secretly generates an unpredictable number and shares a digest to later verify it (HMAC in this lib).
After receiving all HMACS, one sends out the number and starts receiving all the others and verifies them.
Now we have random numbers that need to be combined in a predetermined way that mangles them seemingly randomly and temper proof (very expensive).
For this a hashing chain is chosen.

# Algo

## init
InitialState (IS) contains a unique(-ish) id, to uniquly identify this random number generation
+ a list of all peers participating, ideally with a unique id
+ any extra data, usecase dependent. (like a set of cards we are choosing from)

## rng
Use an unpredictable rng. Simple prng dont cut it, since their state can be reconstructed form very few numbers.
(use system crng or seed own chacha crng with system crng ...)

This library provides `p2prng_gen_and_auth()`, that provides you with a random number and also directly computes mac and key.
IS is mixed into the random number.

## hmac
message is the rng (while possibly variable in size, should be same as output of H() )
key is random

send HMAC to everyone

`p2prng_auth_create()` can be used here, if you dont already use `p2prng_gen_and_auth()`.

## collection
wait for everyone elses HMAC
send out secret message (rng) and key
verify everyone elses message using `p2prng_auth_verify()`


do not proceed until everything is verified.
either hardblock if someone is not responding (to prevent a retry-attack) or exclude unresponsive/lying peer in next (retry) generation.

## post processing (combining)
Combine IS with all the numbers

Using `p2prng_combine_init()` and `p2prng_combine_update()`.

effectively doing:
```
for each M do
  H(M + prevH)
result = H(IS + prevH)
```

