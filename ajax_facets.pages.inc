<?php

/**
 * @file
 * User page callbacks for the ajax_facets module.
 */

/**
 * Callback to update facets content by AJAX.
 */
function ajax_facets_refresh_facets_content() {

  $name = $_GET['view_name'];
  $display_id = $_GET['display_name'];
  // Execute view query to fill facets.
  $view = views_get_view($name);
  $view->execute_display($display_id, array());
  $page = $view->preview($display_id);

  $map = facetapi_get_delta_map();
  $realm_name = 'block';
  $searcher = 'search_api@' . $_GET['index_id'];
  $facets_to_proceed = array();
  $enabled_facets = facetapi_get_enabled_facets($searcher, $realm_name);
  foreach ($enabled_facets as $facet) {
    $facets_to_proceed[] = $facet['name'];
  }
  $facet_blocks = array();
  $reset_urls = array();
  $hide_blocks = array();
  $active_items = array();

  $group = $searcher . ':' . $realm_name;
  // Process values once per searcher-realm group.
  $adapter = facetapi_adapter_load($searcher);
  $builds[$group] = ($adapter) ? $adapter->buildRealm($realm_name) : array();
  $realm = facetapi_realm_load($realm_name);
  $update_results = array();
  foreach ($facets_to_proceed as $facet_name) {
    $facet = $adapter->getFacet(array('name' => $facet_name));
    $reset_urls[$facet_name] = ajax_facets_facet_build_reset_path($facet, $adapter);
    if (!empty($builds[$group][$facet_name])) {
      $build = $facet->getBuild();
      $active_items[$facet_name] = array();
      foreach ($build as $key => $value) {
        if ($value['#active']) {
          $active_items[$facet_name][] = "$facet_name:$key";
        }
      }
      if (!empty($active_items[$facet_name])) {
        sort($active_items[$facet_name]);
      }

      // Skip currently checked facet - we will not refresh them.
      $facet_blocks[$builds[$group][$facet_name]['#attributes']['id']] = drupal_render($builds[$group][$facet_name]);
    }
    else {
      $facet_name = urlencode($facet_name);
      $delta = array_search("$searcher:$realm_name:$facet_name", $map);
      $hide_blocks[] = 'block-facetapi-' . strtolower($delta);
    }

    // Settings for update view.
    $facet_settings = $facet->getSettings($realm);
    $update_results[$facet_name] = isset($facet_settings->settings['update_results']) ? $facet_settings->settings['update_results'] : 0;
  }

  $result = array(
    'applyUrl' => ajax_facets_facet_build_apply_path($adapter),
    'resetUrls' => $reset_urls,
    'newContent' => $facet_blocks,
    'hideBlocks' => $hide_blocks,
    'activeItems' => $active_items,
    'views_content' => $page,
    'display_id' => $display_id,
    'views_name' => $name,
    'facets' => !empty($_GET['f']) ? $_GET['f'] : '',
    'update_results' => $update_results,
    'total_results' => $view->total_rows,
    'settings' => array(),
  );

  // @see template_preprocess_views_view();
  // Update settings when view will be reloaded. Specifically $view->dom_id.
  if ($view->use_ajax) {
    $result['settings'] = array(
      'views' => array(
        'ajax_path' => url('views/ajax'),
        'ajaxViews' => array(
          'views_dom_id:' . $view->dom_id => array(
            'view_name' => $view->name,
            'view_display_id' => $view->current_display,
            'view_args' => check_plain(implode('/', $view->args)),
            'view_path' => check_plain($_GET['q']),
            // Pass through URL to ensure we get e.g. language prefixes.
            'view_base_path' => $view->get_path(),
            'view_dom_id' => $view->dom_id,
            // To fit multiple views on a page, the programmer may have
            // overridden the display's pager_element.
            'pager_element' => isset($view->query->pager) ? $view->query->pager->get_pager_id() : 0,
          ),
        ),
      ),
    );
  }

  return $result;
}
