<?php

/**
 * @file
 * The facetapi_links and facetapi_checkbox_links widget plugin classes.
 */

/**
 * Widget that renders facets as a range of values.
 */
class FacetapiAjaxWidgetRanges extends FacetapiAjaxWidget {

  /**
   * Overrides FacetapiWidget::getDefaultSettings().
   */
  function getDefaultSettings() {
    return [
      'show_reset_link' => 0,
    ];
  }

  /**
   * Transforms the render array for use with theme_item_list().
   */
  function buildListItems($build, $level = 0) {

    // Builds rows.
    $items = [];

    $active_items = [];
    $have_active = FALSE;
    foreach ($build as $value => $item) {
      // Respect current selection.
      if (!empty($item['#active'])) {
        $items['active_value'] = $value;
        $have_active = TRUE;
        $active_items[] = $this->key . ':' . $item['#markup'];
      }

      $items['values'][$item['#indexed_value']] = $item['#indexed_value'];
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

  /**
   * Implements FacetapiWidget::execute().
   *
   * Transforms the render array into something that can be themed by
   * theme_item_list().
   *
   * @see FacetapiWidgetLinks::setThemeHooks()
   * @see FacetapiWidgetLinks::buildListItems()
   */
  public function execute() {
    $element = &$this->build[$this->facet['field alias']];

    $items = $this->buildListItems($element);

    $slider_attrs = [
      'data-facet-name' => rawurlencode($this->settings->facet),
      'data-raw-facet-name' => $this->settings->facet,
      'class' => ['slider-wrapper'],
    ];

    // Get min and max values.
    $raw_values = array_keys($element);
    $raw_values = array_filter($raw_values, 'is_numeric');

    // Check that we have values for the facet widget.
    if (!empty($raw_values)) {
      $global_min = min($raw_values);
      $global_max = max($raw_values);

      $slider_attrs['data-min'] = $global_min;
      $slider_attrs['data-max'] = $global_max;

      // Get selected min and max for active value.
      if (!empty($items['active_value'])) {
        $values = explode(' TO ', $items['active_value']);
        $slider_attrs['data-min-val'] = preg_replace('/[^0-9\.]/', '', $values[0]);
        $slider_attrs['data-max-val'] = preg_replace('/[^0-9\.]/', '', $values[1]);
      }
      // Get selected min and max by default.
      else {
        $slider_attrs['data-min-val'] = $global_min;
        $slider_attrs['data-max-val'] = $global_max;
      }

      // @todo move it into the other place?
      backdrop_add_library('system', 'ui.slider');

      $min_input = theme(
        'ajax_facets_ranges_input',
        [
        'title' => t('From:'),
        'attributes' => ['class' => 'ajax-facets-slider-amount-min'],
        'value' => $slider_attrs['data-min-val'],
      ]
      );

      $max_input = theme(
        'ajax_facets_ranges_input',
        [
        'title' => t('To:'),
        'attributes' => ['class' => 'ajax-facets-slider-amount-max'],
        'value' => $slider_attrs['data-max-val'],
      ]
      );

      // We cannot use backdrop_html_id to save the same id for each facet.
      $wrapper_id = $this->build['#attributes']['id'] . '-wrapper';

      $element = [
        '#markup' => '<div id="' . $wrapper_id . '">'
          . $this->getResetLink()
          . '<div class="ajax-facets-controls-wrapper">'
          . $min_input
          . $max_input
          . '<div ' . backdrop_attributes($slider_attrs) . '></div>'
          . '</div>'
          . '</div>',
      ];

      ajax_facets_add_ajax_js($this->facet);
    }
    // We don't have data for facet widget, hide it.
    else {
      $element = [];
    }
  }
}
