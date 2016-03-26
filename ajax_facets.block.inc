<?php

/**
 * @file
 * Functions, related with block API.
 */

/**
 * Implements hook_block_info().
 */
function ajax_facets_block_info() {
  return array(
    'ajax_facets_block' => array(
      'info' => t('Ajax facets block'),
    ),
  );
}

/**
 * Implements hook_block_configure().
 */
function ajax_facets_block_configure($delta = '') {
  $form = array();

  switch ($delta) {
    case 'ajax_facets_block':
      $form['ajax_facets_reset_all_link_text'] = array(
        '#type' => 'textfield',
        '#title' => t('"Reset all facets" link text'),
        // @todo add support for i18n
        '#default_value' => variable_get('ajax_facets_reset_all_link_text', t('Reset all facets')),
      );

      break;
  }

  return $form;
}

/**
 * Implements hook_block_save().
 */
function ajax_facets_block_save($delta = '', $edit = array()) {
  switch ($delta) {
    case 'ajax_facets_block':
      variable_set('ajax_facets_reset_all_link_text', $edit['ajax_facets_reset_all_link_text']);
      break;
  }
}

/**
 * Implements hook_block_view().
 */
function ajax_facets_block_view($delta = '') {
  $block = array();

  switch ($delta) {
    case 'ajax_facets_block':
      $block['content'] = array(
        '#markup' => l(variable_get('ajax_facets_reset_all_link_text', t('Reset all facets')), '<front>', array(
          'attributes' => array(
            'class' => array(
              'ajax-facets-reset-all-link',
            ),
          ),
        )),
      );

      break;
  }

  return $block;
}
