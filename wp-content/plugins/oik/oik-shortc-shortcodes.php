<?php // Copyright Bobbing Wide 2011,2013
/*
This file implements the code to create the oik ShortCodes button for Tiny MCE
This button allows oik users to choose what they want included from any shortcode that 
is currently active, including the PayPal and oik button shortcodes - which have their own buttons, if required.
  
*/
add_filter( 'mce_buttons', 'bw_shortc_filter_mce_button' );
add_filter( 'mce_external_plugins', 'bw_shortc_filter_mce_plugin' );


/**
 * Implement 'mce_buttons' filter to add the oik shortcodes button
 *
 */
function bw_shortc_filter_mce_button( $buttons ) {
  array_push( $buttons, 'bwshortc_button' );
  return $buttons;
}

/** 
 * Implement 'mce_external_plugins' filter to name the plugin file for the bwshortc_button
 * 
 * A side effect of this filter is to ensure that the jQuery for quicktags is also loaded
 * which means that the [] quicktag will always be active regardless of the value of its checkbox
 * when the oik shortcodes for Tiny MCE is checked.
 * 
*/ 
function bw_shortc_filter_mce_plugin( $plugins ) {
  bw_load_admin_scripts();
  $plugins['bwshortc'] = plugin_dir_url( __FILE__ ) . 'admin/oik_shortc_plugin.js';
  return $plugins;
}
