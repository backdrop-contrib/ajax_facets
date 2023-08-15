<?php
/**
 * @file
 * Theme functions for the Ajax Facets module.
 */

/**
 * Theme function to render the input field for ranges.
 */
function theme_ajax_facets_ranges_input($variables) {
  $output = '';
  if (!empty($variables['title'])) {
    $output .=  '<label>' . $variables['title'] . '</label>';
  }
  $output .= '<input type="text" ' . backdrop_attributes($variables['attributes']) . ' value="' . $variables['value'] . '" />';

  return '<p>' . $output . '</p>';
}
