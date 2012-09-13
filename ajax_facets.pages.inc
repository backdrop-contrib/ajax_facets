<?php

/**
 * @file
 * User page callbacks for the ajax_facets module.
 */

/**
 * Callback to update facets content by AJAX.
 */
function ajax_facets_refresh_facets_content($field_name, $state) {

  $name = $_GET['view_name'];
  $display_id = $_GET['display_name'];
  // Execute view query to fill facets.
  $view = views_get_view($name);
  $view->execute_display($display_id, array());
  $page = $view->preview($display_id);
  $views = array_pop(search_api_current_search("search_api_views:$name:$display_id"));
  if (!empty($_GET['f'])) {
    foreach ($_GET['f'] as $key => $facet) {
      list($type, $value) = explode(':', $facet);
      if (!empty($views['search_api_facets'][$type])) {
        $delete_facet = TRUE;
        foreach ($views['search_api_facets'][$type] as $search_facet) {
          if ($value == str_replace('"', '', $search_facet['filter'])) {
            $delete_facet = FALSE;
            break;
          }
        }
        if ($delete_facet) {
//          unset($_GET['f'][$key]);
        }
      }
    }
  }
  $map = facetapi_get_delta_map();
  $realm_name = 'block';
  $searcher = 'search_api@' . $_GET['index_id'];
  $facets_to_proceed = array();
  $enabled_facets = facetapi_get_enabled_facets($searcher, $realm_name);
  foreach ($enabled_facets as $facet) {
    $widget = facetapi_adapter_load($searcher)->getFacetSettings($facet, facetapi_realm_load($realm_name))->settings['widget'];
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
      $delta = array_search("$searcher:$realm_name:$facet_name", $map);
      $hide_blocks[] = 'block-facetapi-' . strtolower($delta);
    }
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
  );
  return $result;
}
