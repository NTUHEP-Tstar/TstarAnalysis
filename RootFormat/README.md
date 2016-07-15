# RootFormat

## Classes used to represent reconstruction results :

### `FitParticle`
This particle stores the topology it was placed during the mass reconstruction as well as it's fitted four momentum and the four momentum obtained from the particle flow algorithms.

In the case of Monte Carlo, it also stores the generator level topology and generator level four momentum if availiable.

### `RecoResults`
Essentially a list of `FitParticle` with utility functions to get the four momentum of the derived objects, such as the supposed top and supposed excited top.
