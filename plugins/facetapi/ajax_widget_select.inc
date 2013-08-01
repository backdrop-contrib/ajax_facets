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

    // Builds rows.
    $items = array();

    $items['values'][0] = t('Select');

    foreach ($build as $value => $item) {
      // Respect current selection.
      if ($item['#active']) {
        $items['active_value'] = $value;
      }

      $items['values'][$item['#indexed_value']] = $item['#indexed_value'];
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
        . '<div class="' . $this->build['#attributes']['id'] . '">' . render($select) . '</div>'
        . '</div>'
    );
  }
}
