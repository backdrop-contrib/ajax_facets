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
class FacetapiAjaxWidgetCheckboxes extends FacetapiAjaxWidget {

  /**
   * Overrides FacetapiWidget::settingsForm().
   */
  function settingsForm(&$form, &$form_state) {
    parent::settingsForm($form, $form_state);

    if ($this->facet['hierarchy callback']) {
      $form['widget']['widget_settings']['links'][$this->id]['show_expanded'] = [
        '#type' => 'checkbox',
        '#title' => t('Expand hierarchy'),
        '#default_value' => !empty($this->settings->settings['show_expanded']),
        '#description' => t('Show the entire tree regardless of whether the parent items are active.'),
        '#states' => [
          'visible' => [
            'select[name="widget"]' => ['value' => $this->id],
          ],
        ],
      ];
    }

    $form['widget']['widget_settings']['links'][$this->id]['soft_limit'] = [
      '#type' => 'select',
      '#title' => t('Soft limit'),
      '#default_value' => $this->settings->settings['soft_limit'],
      '#options' => [0 => t('No limit')] + drupal_map_assoc([50, 40, 30, 20, 15, 10, 5, 3]),
      '#description' => t('Limits the number of displayed facets via JavaScript.'),
      '#states' => [
        'visible' => [
          'select[name="widget"]' => ['value' => $this->id],
        ],
      ],
    ];

    $last = end($form['widget']['widget_settings']['links']);
    foreach ($form['widget']['widget_settings']['links'] as $id => $element) {
      if ($last != $element) {
        $form['widget']['widget_settings']['links'][$id]['#attributes']['style'] = 'display: none;';
      }
    }
  }

  /**
   * Overrides FacetapiWidget::getDefaultSettings().
   */
  function getDefaultSettings() {
    return [
      'soft_limit' => 20,
      'show_expanded' => 0,
      'reset_link' => 0,
      'empty_behavior' => 'ajax_facets',
    ];
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
    $items = [];
    $items_count = count($build);
    $active_items = [];

    foreach ($build as $value => $item) {
      $row = ['class' => []];
      $attributes = [
        'class' => ['facet-multiselect-checkbox'],
        'data-facet-value' => $value,
        'data-facet-name' => rawurlencode($this->settings->facet),
        'data-raw-facet-name' => $this->settings->facet,
      ];

      // Mark as disabled if count is 0.
      if ($item['#count'] == 0) {
        $attributes['disabled'] = 'disabled';
        $row['class'][] = 'facetapi-disabled';
      }

      // Respect current selection.
      if (!empty($item['#active'])) {
        $attributes['checked'] = 'checked';
        $have_active = TRUE;
        $active_items[] = $this->key . ':' . $item['#markup'];
        $row['class'][] = 'facetapi-active';
      }

      // Show/hide counts according to the settings.
      if(!empty($this->settings->settings['display_count'])) {
        $item['#markup'] .= ' ' . theme('facetapi_count', (['count' => $item['#count']]));
      }

      $checkbox = [
        '#id' => $this->getAjaxFacetsUuid($value),
        '#name' => rawurlencode($this->key),
        '#type' => 'checkbox',
        '#title' => $item['#markup'],
        '#attributes' => $attributes,
      ];
      $row['data'] = drupal_render($checkbox);
      if ($items_count == 1) {
        $row['class'][] = 'single-leaf';
      }

      if (!empty($item['#item_children'])) {
        if (!empty($item['#active']) || !empty($this->settings->settings['show_expanded'])) {
          $row['class'][] = 'expanded';
          $row['children'] = $this->buildListItems($item['#item_children']);
        } else {
          $row['class'][] = 'collapsed';
        }
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

    $facet_settings = $this->facet->getSettings();
    $this->jsSettings['limit_active_items'] = $facet_settings->settings['limit_active_items'];
    return $items;
  }
}
