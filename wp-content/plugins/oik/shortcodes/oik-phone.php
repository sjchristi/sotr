<?php // (C) Copyright Bobbing Wide 2011,2013

/**
 * Return the telephone number in desired HTML markup, if set or passed as number=
 *
 * @param array atts
 *   prefix = type of number: free form e.g. Tel, Home, Work, Mob, Cell
 *   sep = separator default ': '
 *   number = telephone number override
 *   tag = 'div' or 'span' or other HTML wrapping tag - with start and end
 *   index = field to obtain ( telephone, fax, mobile, emergency )
 *   alt = 1 if alternative number are required
 *   user=id|email|login|nicename
 *
 * The telephone number is formatted using microformats
 * See ISBN: 978-1-59059-814-6 p. 140
 * 
 * Example:
 * [bw_telephone]
 * [bw_telephone number="023 92 410090" prefix="Portsmouth"]
 * [bw_telephone user=3]
 *
 */
function _bw_telephone( $atts=null ) {
  $prefix = bw_array_get_dcb( $atts, "prefix", "Tel" );
  $sep = bw_array_get( $atts, "sep", ": " );
  $number = bw_array_get( $atts, "number", null );
  $index = bw_array_get( $atts, "index", "telephone" );
  $tag = bw_array_get( $atts, "tag", "div" );
  $class = bw_array_get( $atts, "class", null );
  if ( !$number ) {
    $number = bw_get_option_arr( $index, "bw_options", $atts );
  } 
  if ( $number <> "" ) {
    stag( $tag,  "tel $class" );
    span( "type");
    e( $prefix );
    epan();
    span( "sep" );
    e( $sep );
    epan();
    span( "value" );
    e( $number );
    epan();
    etag( $tag );
  }
  return( bw_ret());
}

/**
 * Implement [bw_telephone] shortcode to display the telephone number, if set
 */
function bw_telephone( $atts = null ) { 
  $atts['index'] = bw_array_get( $atts, "index", "telephone" );
  return( _bw_telephone( $atts ) );
}  

/**
 * Implement [bw_fax] shortcode to display the fax number, if set
 */
function bw_fax( $atts = null ) {
  $atts['index'] = bw_array_get( $atts, "index", "fax" );
  $atts['prefix'] = bw_array_get_dcb( $atts, "prefix", "Fax" );
  return( _bw_telephone( $atts ) );
}

/**
 * Implement [bw_mobile] shortcode to display the mobile (cell) number, if set  
 */
function bw_mobile( $atts = null ) {
  $atts['index'] = bw_array_get( $atts, "index", "mobile" );
  $atts['prefix'] = bw_array_get_dcb( $atts, "prefix", "Mobile" );
  return( _bw_telephone( $atts ) );
}

/**
 * Implement [bw_emergency] shortcode to display the emergency number, if set
 */
function bw_emergency( $atts = null ) {
  $atts['index'] = bw_array_get( $atts, "index", "emergency" );
  $atts['prefix'] = bw_array_get_dcb( $atts, "prefix", "Emergency" );
  $atts['class'] = bw_array_get( $atts, "class", "bw_emergency" );
  return( _bw_telephone( $atts ) );
}

/**
 * Implement [bw_tel] shortcode to display an inline telephone number, using span
 */ 
function bw_tel( $atts=null ) {
  $atts['tag'] = bw_array_get( $atts, "tag", "span" );
  return _bw_telephone( $atts );
}

/**
 * Inline [bw_mob] shortcode to display an inline mobile number, using span
 */ 
function bw_mob( $atts=null ) {
  $atts['tag'] = bw_array_get( $atts, "tag", "span" );
  $atts['index'] = bw_array_get( $atts, "index", "mobile" );
  $atts['prefix'] = bw_array_get_dcb( $atts, "prefix", "Mobile" );
  return _bw_telephone( $atts );
}

/**
 * Implement [bw_skype] shortcode to display the Skype contact information
 * 
 * Skype Online Material: the Skype buttons and widgets available for download on the 
 * Skype Website at http://www.skype.com/share/buttons/ 
 * as such may be changed from time to time by Skype in its sole discretion.  
*/
function bw_skype( $atts=null, $content=null, $tag=null ) {
  $atts['index'] = bw_array_get( $atts, "index", "skype" );
  $atts['prefix'] = bw_array_get_dcb( $atts, "prefix", "Skype name" );
  $atts['class'] = bw_array_get( $atts, "class", "bw_skype" );
  $atts['number'] = bw_array_get( $atts, "number", null );
  if ( !$atts['number'] ) {
    $atts['number'] = bw_get_option_arr( $atts['index'], null, $atts );
  }
  return( _bw_telephone( $atts ) );
}  


