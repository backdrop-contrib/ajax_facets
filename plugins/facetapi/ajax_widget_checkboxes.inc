<?php

/**
 * @file
 * The facetapi_links and facetapi_checkbox_links widget plugin classes.
 */

/**
 * Widget that renders facets as a list of clickable links.
 *
 * Links make it easy for users to narrow down their search results by clicking
 * on them. The render arrays use theme_item_list() to generate the HTML markup.
 */
class FacetapiAjaxWidgetCheckboxes extends FacetapiWidgetCheckboxLinks {

  /**
   * Overrides FacetapiWidgetCheckboxLinks::init().
   */
  public function init() {
    parent::init();
    $this->jsSettings['makeCheckboxes'] = null;
    $this->jsSettings['makeMultiCheckboxes'] = 1;
    ajax_facets_add_ajax_checkboxes_js($this->facet);
  }

  /**
   * Overrides FacetapiWidget::settingsForm().
   */
  function settingsForm(&$form, &$form_state) {
    $form['widget']['widget_settings']['links'][$this->id]['index_id'] = array(
      '#type' => 'value',
      '#title' => t('Index id'),
      '#default_value' => $form['#facetapi']['facet']['map options']['index id'],
      '#description' => t('Limits the number of displayed facets via JavaScript.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
  }

  /**
   * Transforms the render array for use with theme_item_list().
   *
   * The recursion allows this function to act on the various levels of a
   * hierarchical data set.
   *
   * @param array $build
   *   The items in the facet's render array being transformed.
   *
   * @return array
   *   The "items" parameter for theme_item_list().
   */
  function buildListItems($build) {

    $have_active = FALSE;
    // Builds rows.
    $items = array();
    $items_count = count($build);
    $active_items = array();
    foreach ($build as $value => $item) {
      $row = array('class' => array());
      $attributes = array(
        'class' => array('facet-multiselect-checkbox'),
      );
      // Respect current selection.
      if ($item['#active']) {
        $attributes['checked'] = 'checked';
        $have_active = TRUE;
        $active_items[] = $this->key . ':' . $value;
      }
      $checkbox = array(
        '#id' => 'facet-checkboxes-' . str_replace('_', '-', $this->key) . '-' . $value,
        '#name' => $this->key . ':' . $value,
        '#type' => 'checkbox',
        '#title' => $item['#markup'] . '(' . $item['#count'] . ')',
//        '#theme_wrappers' => array('form_element_no_wrapper'),
        '#attributes' => $attributes,
      );
      $row['data'] = drupal_render($checkbox);
      if ($items_count == 1) {
        $row['class'][] = 'single-leaf';
      }
      $items[] = $row;
    }

    $this->jsSettings['haveActiveSelection'] = $this->settings->settings['have_active_selection'] = $have_active;
    sort($active_items);
    $this->jsSettings['activeItems'] = $active_items;
    // Generate reset path on server side to make possible to use aliases.
    if ($have_active) {
      $this->jsSettings['resetPath'] = ajax_facets_facet_build_reset_path($this->facet->getFacet(), $this->facet->getAdapter());
    }
    return $items;
  }
}
