# BaseLineSelector

Baseline selections and variable caching to reduce the MINIAOD files to a size that could be handled by single user. For the reasoning and details for what the selection are, read the [documentation](http://yichen.web.cern.ch/yichen/slides/tstar_summary/)

## structure:

* [`interface`](interface): Forwards declaration of EDM plugins.
* [`plugins`](plugins): EDM object interaction and main control flow defined here.
* [`src`](src): Object selection of variable caching
* [`python`](python): Default settings for the plugins
