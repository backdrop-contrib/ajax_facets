(function ($) {

  /**
   * Class containing functionality for Facet API.
   */
  Drupal.ajax_facets = {};
  // Store current query value.
  Drupal.ajax_facets.queryState = null;
  // State of each facet.
  Drupal.ajax_facets.facetQueryState = null;

  Drupal.behaviors.ajax_facets = {
    attach: function(context, settings) {

      $('div.block-facetapi-content-wrapper').once(function () {
        $(this).each(function() {
          if ($(this).height() > 200) {
            $(this).mCustomScrollbar({set_height: 200, scrollEasing:'swing', scrollInertia:0});
          }
        })
      });
      if (!Drupal.ajax_facets.queryState) {
        if (settings.facetapi.defaultQuery != undefined && settings.facetapi.defaultQuery) {
          Drupal.ajax_facets.queryState = {'f' : settings.facetapi.defaultQuery};
        }
        else {
          Drupal.ajax_facets.queryState = {'f':[]};
        }
        // We will send original search path to server to get back proper reset links.
        if (settings.facetapi.searchPath != undefined) {
          Drupal.ajax_facets.queryState['searchPath'] = settings.facetapi.searchPath;
        }
        if (settings.facetapi.index_id != undefined) {
          Drupal.ajax_facets.queryState['index_id'] = settings.facetapi.index_id;
        }
        if (settings.facetapi.view_name != undefined) {
          Drupal.ajax_facets.queryState['view_name'] = settings.facetapi.view_name;
        }
        if (settings.facetapi.facet_field != undefined) {
          Drupal.ajax_facets.queryState['facet_field'] = settings.facetapi.facet_field;
        }
        if (settings.facetapi.display_name != undefined) {
          Drupal.ajax_facets.queryState['display_name'] = settings.facetapi.display_name;
        }
        // Respect search keywords in AJAX queries.
        if (settings.facetapi.searchKeys != undefined) {
          Drupal.ajax_facets.queryState['search_api_views_fulltext'] = settings.facetapi.searchKeys;
        }

        Drupal.ajax_facets.applyPath = '';
      }
      // Iterates over facet settings, applies functionality like the "Show more"
      // links for block realm facets.
      // @todo We need some sort of JS API so we don't have to make decisions
      // based on the realm.
      if (settings.facetapi) {
        for (var index in settings.facetapi.facets) {
          if (null != settings.facetapi.facets[index].makeMultiCheckboxes) {
            if (settings.facetapi.facets[index].haveActiveSelection) {
              Drupal.ajax_facets.bindResetLink(settings.facetapi.facets[index].id, index, settings);
            }

            $('#' + settings.facetapi.facets[index].id + ' input.facet-multiselect-checkbox:not(.processed)').change([settings.facetapi.facets[index]], Drupal.ajax_facets.processCheckboxes).addClass('processed');
            // Process colors facet links.
            $('#' + settings.facetapi.facets[index].id + ' a.facet-colors-ajax-link:not(.processed)').click(function () {
              $(this).parent().toggleClass('selected').find('input[type="checkbox"]').click().trigger('change');
              return false;
            }).addClass('processed');
          }
          if (null != settings.facetapi.facets[index].limit) {
            // Applies soft limit to the list.
            Drupal.facetapi.applyLimit(settings.facetapi.facets[index]);
          }
        }
      }
    }
  };

  Drupal.ajax_facets.bindResetLink = function(parentId, index, settings) {
    $('#' + parentId).parents('.block-facetapi').find('a.reset-link:not(".processed")').addClass('processed').click(function() {
      if (Drupal.ajax_facets.applyFlag) {
        window.location = settings.facetapi.facets[index].resetPath;
      }
      return false;
    });
  };

  // Detect could we click apply or not.
  Drupal.ajax_facets.applyFlag = true;

  /**
   * Just compare two arrays.
   */
  Drupal.ajax_facets.compareArrays = function(a, b) {
    if (a.length != b.length) {
      return false;
    }
    for (i in a) {
      if (a[i] != b[i]) {
        return false;
      }
    }

    return true;
  }

  Drupal.ajax_facets.hideApplyLink = function ($link, $resetLink) {
    if ($link.size()) {
      $link.hide();
      if ($resetLink.size()) {
        $resetLink.show();
      }
    }
  }

  Drupal.ajax_facets.addApplyLink = function($this, facetOptions) {
    var $parent = $this.parents('div.block-facetapi').find('div.block-facet-title');
    // Each time we check if enw apply path is equals to default page (created on page load), we will not show link.
    if ($parent.size()) {
      var $link = $('a.apply-link', $parent);
      var $resetLink = $('a.reset-link', $parent);
      if (Drupal.settings.facetapi.applyPath != undefined && Drupal.settings.facetapi.applyPath != Drupal.ajax_facets.applyPath) {
        if (Drupal.ajax_facets.facetQueryState[facetOptions.facetName] != undefined && !Drupal.ajax_facets.compareArrays(Drupal.ajax_facets.facetQueryState[facetOptions.facetName], facetOptions.activeItems)) {
          if ($resetLink.size()) {
            $resetLink.hide();
          }
          if ($link.size()) {
            $link.show();
          }
          else {
            $parent.append('<a href="#" class="block-title-link apply-link">' + Drupal.t('Apply') + '</a>');
            $('a.apply-link', $parent).click(function () {
              if (Drupal.ajax_facets.applyFlag) {
                window.location = Drupal.ajax_facets.applyPath;
              }
              return false;
            });
          }
        }
        else {
          Drupal.ajax_facets.hideApplyLink($link, $resetLink);
        }
      }
      // If pathes are equal we hide apply link.
      else {
        Drupal.ajax_facets.hideApplyLink($link, $resetLink);
      }
    }
  };

  Drupal.ajax_facets.updateBlockScroll = function() {
    $('div.block-facetapi-content-wrapper').each(function() {
      $this = $(this);
      if ($('div.mCSB_container', $this).size()) {
        $this.mCustomScrollbar("update", {set_height: 200});
      }
      // Because some blocks could have no scroll on page load, we should init them from scratch.
      else if ($this.height() > 200) {
        $this.mCustomScrollbar({set_height: 200, scrollEasing:'swing', scrollInertia:0});
      }
    });
  };

  /**
   * Process click on each checkbox.
   * 1. Send Ajax to refresh facets.
   * 2. Control Apply link state.
   */
  Drupal.ajax_facets.processCheckboxes = function(event) {
    var $this = $(this);
    var facetOptions = event.data[0];
    var facetCheckboxName = $this.attr('name');
    // Show loader on request start.
    $('div.block-facetapi div.loader').show();

    if (Drupal.ajax_facets.queryState['f'] != undefined) {
      var queryNew = new Array();
      if ($this.is(':checked')) {
        var addCurrentParam = true;
        console.log(Drupal.settings.facetapi.facets);
        console.log(Drupal.ajax_facets.queryState['f']);
        Drupal.settings.facetapi.facets;
        for (var index in Drupal.ajax_facets.queryState['f']) {
          if (Drupal.ajax_facets.queryState['f'][index] == facetCheckboxName) {
            addCurrentParam = false;
          }
        }
        if (addCurrentParam) {
          Drupal.ajax_facets.queryState['f'][Drupal.ajax_facets.queryState['f'].length] = facetCheckboxName;
        }
      }
      // If we unset filter, remove them from query.
      else {
        for (var index in Drupal.ajax_facets.queryState['f']) {
          if (Drupal.ajax_facets.queryState['f'][index] != facetCheckboxName) {
            queryNew[queryNew.length] = Drupal.ajax_facets.queryState['f'][index];
          }
        }
        Drupal.ajax_facets.queryState['f'] = queryNew;
      }
    }
//    console.log(Drupal.ajax_facets.queryState);
    // Deny any filtering during refresh.
    Drupal.ajax_facets.applyFlag = false;
    $.ajax({
      type: 'GET',
      url: Drupal.settings.basePath + 'ajax/ajax_facets/refresh/' + $this.attr('name') + '/' + ($this.is(':checked') ? '1' : '0'),
      dataType: 'json',
      // We copy all params to force search query with proper arguments.
      data: Drupal.ajax_facets.queryState,
      success: function (response) {
//        if (response.facets != undefined) {
//          Drupal.ajax_facets.queryState['f'] = response.facets;
//        }
        if (response.activeItems != undefined) {
          Drupal.ajax_facets.facetQueryState = response.activeItems;
        }
        // After Ajax success we should update reset, apply link to handle proper redirects.
        if (response.resetUrls != undefined && Drupal.settings.facetapi.facets != undefined) {
          for (index in Drupal.settings.facetapi.facets) {
            if (response.resetUrls[Drupal.settings.facetapi.facets[index].facetName] != undefined) {
              // Update path from responce.
              Drupal.settings.facetapi.facets[index].resetPath = response.resetUrls[Drupal.settings.facetapi.facets[index].facetName];
            }
          }
        }

        // Handle apply link url.
        if (response.applyUrl != undefined) {
          Drupal.ajax_facets.applyPath = response.applyUrl;
        }
        Drupal.ajax_facets.applyFlag = true;
        Drupal.ajax_facets.addApplyLink($this, facetOptions);

        if (response.newContent != undefined && response.newContent) {
          for (var id in response.newContent) {
            var $blockToReplace = $('#' + id).parent();
            if ($blockToReplace.size()) {
              $blockToReplace.replaceWith(response.newContent[id]);
            }
            var $block = $('#' + id).parents('div.block-facetapi-facetapi-ajax-multi:not(:visible)');
            if ($block.size()) {
              $block.show();
            }
          }
          Drupal.attachBehaviors(response.newContent);
        }
        $('.view-id-' + response.views_name + '.view-display-id-' + response.display_id).replaceWith(response.views_content);
        // As some blocks could be empty in results of filtering - hide them.
        if (response.hideBlocks != undefined && response.hideBlocks) {
          for (var id in response.hideBlocks) {
            var $block = $('#' + response.hideBlocks[id]);
            if ($block.size()) {
              $block.hide();
            }
          }
        }
        // Hide loader on request success.
        $('div.block-facetapi div.loader').hide();
        Drupal.ajax_facets.updateBlockScroll();
      },
      error: function (xmlhttp) {
        Drupal.ajax_facets.applyFlag = true;
        // Hide loader on request success.
        $('div.block-facetapi div.loader').hide();
        Drupal.ajax_facets.updateBlockScroll();
      }
    });
  };
})(jQuery);
