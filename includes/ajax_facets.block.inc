<?php

/**
 * @file
 * Functions, related with block API.
 */

/**
 * Implements hook_block_info().
 */
function ajax_facets_block_info() {
  return [
    'ajax_facets_block' => [
      'info' => t('Ajax facets block'),
    ],
  ];
}

/**
 * Implements hook_block_configure().
 */
function ajax_facets_block_configure($delta = '') {
  $form = [];

  switch ($delta) {
    case 'ajax_facets_block':
      $form['ajax_facets_reset_all_link_text'] = [
        '#type' => 'textfield',
        '#title' => t('"Reset all facets" link text'),
        '#default_value' => ajax_facets_block_link_text('reset'),
      ];

      $form['ajax_facets_submit_all_link_text'] = [
        '#type' => 'textfield',
        '#title' => t('"Submit all facets" link text'),
        '#default_value' => ajax_facets_block_link_text('submit'),
      ];

      $form['ajax_facets_auto_update_facets'] = [
        '#type' => 'checkbox',
        '#title' => t('Auto-update available facets and counts after facet select'),
        '#default_value' => variable_get('ajax_facets_auto_update_facets', 1),
      ];

      break;
  }

  return $form;
}

/**
 * Implements hook_block_save().
 */
function ajax_facets_block_save($delta = '', $edit = []) {
  switch ($delta) {
    case 'ajax_facets_block':
      variable_set('ajax_facets_reset_all_link_text', $edit['ajax_facets_reset_all_link_text']);
      variable_set('ajax_facets_submit_all_link_text', $edit['ajax_facets_submit_all_link_text']);
      variable_set('ajax_facets_auto_update_facets', $edit['ajax_facets_auto_update_facets']);
      // Update the i18n string if need.
      if (function_exists('i18n_string_update')) {
        $name = [
          'ajax_facets',
          'ajax_facets_block',
          'facet_reset_all_link_text',
          'text',
        ];
        i18n_string_update($name, $edit['ajax_facets_reset_all_link_text']);

        $name = [
          'ajax_facets',
          'ajax_facets_submit_block',
          'facet_submit_all_link_text',
          'text',
        ];

        i18n_string_update($name, $edit['ajax_facets_submit_all_link_text']);
      }
      break;
  }
}

/**
 * Implements hook_block_view().
 */
function ajax_facets_block_view($delta = '') {
  $block = [];

  switch ($delta) {
    case 'ajax_facets_block':
      $reset_link = l(
        ajax_facets_block_link_text('reset'),
        current_path(),
        [
          'attributes' => [
            'class' => [
              'ajax-facets-reset-all-link',
            ],
          ],
        ]
      );

      $submit_link = l(
        ajax_facets_block_link_text('submit'),
        current_path(),
        [
          'attributes' => [
            'class' => [
              'ajax-facets-submit-all-link',
            ],
          ],
        ]
      );

      $block['content'] = [
        '#markup' => theme(
          'item_list',
          [
            'items' => [
              $reset_link,
              $submit_link
            ]
          ]
        )
      ];

      break;
  }

  return $block;
}

/**
 * Returns text for Ajax Facets submit & reset links.
 */
function ajax_facets_block_link_text($type) {
  $text = '';
  if ($type == 'reset') {
    $text = variable_get('ajax_facets_reset_all_link_text', t('Reset all facets'));
    if (function_exists('i18n_string_translate')) {
      $text = i18n_string_translate(
        [
          'ajax_facets',
          'ajax_facets_block',
          'facet_reset_all_link_text',
          'text'
        ],
        $text
      );
    }
  }
  elseif ($type == 'submit') {
    $text = variable_get('ajax_facets_submit_all_link_text', t('Submit all facets'));

    if (function_exists('i18n_string_translate')) {
      $text = i18n_string_translate(
        [
          'ajax_facets',
          'ajax_facets_submit_block',
          'facet_submit_all_link_text',
          'text',
        ],
        $text
      );
    }
  }

  return $text;
}


/**
 * Implements hook_preprocess_block().
 */
function ajax_facets_preprocess_block(&$variables) {
  if (!empty($variables['block']->module) && $variables['block']->module == 'facetapi') {
    // Add BEM style class to facet block.
    // This class is using to show hidden facet block.
    $variables['classes_array'][] = 'block--ajax_facets';
  }
}

/**
 * Implements hook_preprocess_panels_pane().
 */
function ajax_facets_preprocess_panels_pane(&$variables) {
  // Add special class to identify panels pane as block by module ajax facets.
  if (!empty($variables['content']['#facet'])) {
    // subtype should contain something like "facetapi-ZsnSXo0R09p45W7e4f70B4qN67wJgjZQ".
    if (!empty($variables['pane']->subtype)) {
      // Add BEM style class to facet block.
      // This class is using to show hidden facet block.
      $variables['classes_array'][] = 'block--ajax_facets';
      // We set class but not ID, because drupal blocks have such ID by default,
      // but panes can have other ID and it can be a cause of potential problems.
      $variables['classes_array'][] = strtolower("block-{$variables['pane']->subtype}");
    }
  }
}