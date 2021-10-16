Ajax facets
===========

This module allows you to create facet filters which work through AJAX:
Filters and search results will be updated by AJAX.

Widgets:
* Ajax range slider
* Ajax multiple checkboxes
* Ajax selectbox
* Ajax links

Requirements
------------

This module requires that the following modules are also enabled:

* [Facet API](https://github.com/backdrop-contrib/facetapi)
* [Search API](https://github.com/backdrop-contrib/search_api)
* Views (in Backdrop Core)

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.

- Visit the configuration page under Administration > Configuration > Category >
  Foo (admin/config/category/foo) and enter the required information.

- Create and configure Search server in Drupal.

- Create Search Index, configure it. Add some fields into index and select few
  of them as facets. Select one of four ajax widgets for your facets. Enable
  option "Update results by ajax" for your facet.

- Create view based on your search index. Fastest way is select type of display
  "Page". Setting "Access" should be configured for your views display if you
  want to use them with ajax facets. It shouldn't have value "none". Otherwise,
  user will receive error "Search results cannot be filtered, please contact
  site administrator." each time, when he would try to filter results. Also,
  you should enable Ajax for your views display.

- Put block of your facet on the page where will be placed your view with search
  results. **Important** - view should be processed earlier than facet block
  will be rendered. Otherwise facets will not work.

- Put "Ajax facets block" block on the page if you want to have buttons "Reset
  all facets" and "Submit all facets".


Documentation
-------------

MODES OF WORK

**Mode of immediate updating of the results** - enabled by default.
When you will put blocks of the ajax facets on the page with search results,
but do not put the block with ajax facets buttons, the facets will be updated
immediately.

**Mode of updating by demand** - disabled by default.
If you will put blocks of the ajax facets and block with ajax facets buttons on
the page with search results, the facets will be updated right after any
filtering. But search results will be updated only after the click on the button
"Submit all facets".

Additional documentation is located in [the Wiki](https://github.com/backdrop-contrib/ajax_facets/wiki/Documentation).

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/ajax_facets/issues).

Current Maintainers
-------------------

- [Justin Christoffersen](https://github.com/larsdesigns).
- [Jen Lampton](https://github.com/jenlampton).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Justin Christoffersen](https://github.com/larsdesigns).
- Maintained for Drupal by [Eugene Ilyin](https://www.drupal.org/user/1767626).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
