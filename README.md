# Fair and temper-proof random-number-generation in a distributed environment

As long as you can contribute an unpredictable (random) number of your choosing, the algorithm will make sure the outcome is unpredictable.


First a public "vote" is held, where everyone secretly generates an unpredictable number and shares a digest to later verify it (HMAC in this proposal).
After receiving all HMACS, one sends out the number and starts receiving all the others and verifies them.
Now we have random numbers that need to be combined in a predetermined way that mangles them seemingly randomly and temper proof (very expensive).
For this a hashing chain is chosen.

# Algo

## init
InitialState (IS) contains a unique(-ish) id, to uniquly identify this random number
+ any extra data thats usecase dependent. (like the set of cards we are choosing from)

-> hashed to get a fixed sized SI
SI = H(id + user data)

## rng?
use an unpredictable rng. Simple prng dont cut it, since their state can be reconstructed form very few numbers.
(use system crng or seed own chacha crng with system crng ...)

## hmac
message is the rng (while possibly variable in size, should be same as output of H() )
key is random

send HMAC to everyone

## collection
wait for everyone elses HMAC
send out secret message (rng) and key
verify everyone elses message


do not proceed until everything is verified.
either hardblock if someone is not responding (to prevent a retry-attack) or exclude unresponsive/lying peer in next (retry) generation.

## post processing

combine IS with all the numbers

for each M do
  H(M + prevH)
result = H(IS + prevH)

