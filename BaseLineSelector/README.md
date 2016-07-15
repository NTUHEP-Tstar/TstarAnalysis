# BaseLineSelector

Baseline selections and variable caching to reduce the MINIAOD files to file that could be handled by single user. For the reasoning and details for what the selection are, read the [documentation](http://yichen.web.cern.ch/yichen/slides/tstar_summary/)

## structure:

* [`plugins`](plugins): All EDM plugins defined and implemened in single file here
* [`src`](src) and [`interface`](interface): Functions for variable caching.
* [`python`](python): Default settings for the plugins
