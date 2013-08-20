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
class FacetapiAjaxWidgetSelect extends FacetapiWidgetCheckboxLinks {

  /**
   * Overrides FacetapiWidgetCheckboxLinks::init().
   */
  public function init() {
    parent::init();
    drupal_add_js(
      array(
        'facetapi' => array('ajax_select_box' => array('default_value' => t('Select')))
      ),
      'setting'
    );
    ajax_facets_add_ajax_js($this->facet);
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

    $form['widget']['widget_settings']['links'][$this->id]['update_results'] = array(
      '#type' => 'checkbox',
      '#title' => t('Update results by ajax'),
      '#default_value' => $this->settings->settings['update_results'],
      '#description' => t('If active, then search results will be updated by AJAX when facet was changed.
       If not active, then after than facet will be updated, user will see special popup with link for filter results.'),
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

    // Builds rows.
    $items = array();

    $items['values'][0] = t('Select');

    $active_items = array();
    $have_active = FALSE;
    foreach ($build as $value => $item) {
      // Respect current selection.
      if ($item['#active']) {
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

    // If name is empty - use label.
    if (empty($items['name'])) {
      $facet = $this->facet->getFacet();
      $items['name'] = $facet['label'];
    }

    $select = array(
      '#type' => 'select',
      '#title' => $this->build['#title'],
      '#options' => $items['values'],
      '#id' => 'ajax-facets-selectbox-' . str_replace('_', '-', $this->settings->facet),
      '#name' => urlencode($this->settings->facet),
      '#attributes' => array(
        'data-facet' => urlencode($this->settings->facet),
      ),
    );

    if (!empty($items['active_value'])) {
      $select['#value'] = $items['active_value'];
    }

    $element = array(
      '#markup' => '<div class="facet-wrapper-selectbox">'
        . '<a class="reset-link" href="#">' . t('Reset filter') . '</a>'
        . '<div id="' . $this->build['#attributes']['id'] .'" class="' . $this->build['#attributes']['id'] . '">' . render($select) . '</div>'
        . '</div>'
    );
  }
}
