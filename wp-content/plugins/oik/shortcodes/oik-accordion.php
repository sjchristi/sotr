<?php // (C) Copyright Bobbing Wide 2012, 2013
/** 
 * Return the next unique ID for the accordion selector
 */
function bw_accordion_id() { 
  static $accordion_id = 0;
  $accordion_id++;
  return( "bw_accordion-$accordion_id" );
}

/**
 * Display pages styled for jQuery accordion
 *
 * Basically we achieve what we can do manually using bw_jq
 * divs and headings
 
 <code>
[bw_jq selector=".fadein,.pages" method=accordion script=jquery-ui-accordion]

[div class=fadein]
<h3>First</h3><div>I am the first part of the accordion</div>
<h3><a href='#'>Second</a></h3><div>I am the second part of the accordion</div>
<h3>Third</h3><div>I am the third part of the accordion</div>
[ediv]
</code>


*/
function bw_accordion( $atts=null, $content=null, $tag=null ) {
  oik_require( "includes/bw_posts.inc" );
  $posts = bw_get_posts( $atts );
  if ( $posts ) {
    oik_require( "shortcodes/oik-jquery.php" );
    $debug = bw_array_get( $atts, "debug", false );
    bw_jquery_enqueue_script( "jquery-ui-accordion", $debug );
    bw_jquery_enqueue_style( "jquery-ui-accordion" );
    $selector = bw_accordion_id();
    bw_jquery( "#$selector", "accordion" );
    $class = bw_array_get( $atts, "class", "bw_accordion" );
    sdiv( $class, $selector );
    foreach ( $posts as $post ) {
      bw_format_accordion( $post, $atts );
    }
    ediv( $class );
    bw_clear_processed_posts();
  }
  return( bw_ret() );
}

/** 
 * Format an accordion block - for jQuery UI accordion 1.9.2 or higher
 *
 * @param object $post - A post object
 * @param array $atts - Attributes array - passed from the shortcode
 *
 * Should we not do this using 
 *  apply_filters( "bw_format_accordion", $post, $atts );
 *  or even do_action( "bw_format_accordion", ... ); ?
 */
function bw_format_accordion( $post, $atts ) {
  $atts['title'] = get_the_title( $post->ID );
  $thumbnail = bw_thumbnail( $post->ID, $atts );
  h3( $atts['title'] );
  sdiv();
    if ( $thumbnail ) {
      bw_format_thumbnail( $thumbnail, $post, $atts );
    }   
    e( bw_excerpt( $post ) );
    bw_format_read_more( $post, $atts ); 
  ediv();
}

function bw_accordion__syntax( $shortcode="bw_accordion" ) {
  return( _sc_posts() );
}

function bw_accordion__example( $shortcode="bw_accordion" ) {
  $text = "Display the two most recent pages";
  $example = "numberposts=2 post_type=page orderby=date order=DESC post_parent=0";
  bw_invoke_shortcode( $shortcode, $example, $text );
}

function bw_accordion__snippet( $shortcode="bw_accordion" ) {
  $example = "numberposts=2 post_type=page orderby=date order=DESC post_parent=0";
  //_sc__snippet( $shortcode, $example );
  e( "Snippet not produced for this shortcode: $shortcode" );
}




