<?php
/*
    Copyright 2011-2013 Bobbing Wide (email : herb@bobbingwide.com )

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2,
    as published by the Free Software Foundation.

    You may NOT assume that you can use any other version of the GPL.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    The license for this software can likely be found here:
    http://www.gnu.org/licenses/gpl-2.0.html

*/

oik_require( "includes/bw_images.inc" );


/** 
 * Return the global post ID
 * @return ID - the global post ID or 0
 */
function bw_global_post_id() {
  if ( isset( $GLOBALS['post'] )) {
    $post_id = $GLOBALS['post']->ID;
  } else {
    $post_id = 0;
  }  
  return( $post_id ) ;
}  

/** 
 * Return the global post's post_type
 * @return string/NULL - the global post's post_type or NULL
 */
function bw_global_post_type() {
  if ( isset( $GLOBALS['post'] )) {
    $post_type = $GLOBALS['post']->post_type;
  } else {  
    $post_type = NULL;
  }  
  return( $post_type ) ;
}

/** 
 * Return the value of the GLOBAL post's excerpt field setting it to the new value
 * 
 * @param string $excerpt - the new value that we want to set
 * @return string the previously stored value
 * 
 * Use this function in pairs to save and then reset the global value.
 * 
 */
function bw_global_excerpt( $excerpt=null ) {
  if ( isset( $GLOBALS['post'] )) {
    $excerpt_to_return = $GLOBALS['post']->post_excerpt;
    $GLOBALS['post']->post_excerpt = $excerpt;
  } else {
    $excerpt_to_return = null;
  }  
  return( $excerpt_to_return );   
}

function bw_process_this_post( $id ) {
  global $processed_posts;
  $processed = bw_array_get( $processed_posts, $id, false );
  if ( !$processed ) {
    $processed_posts[$id] = $id ;
  }
  bw_trace2( $processed_posts );
  return( !$processed );
}

function bw_clear_processed_posts() {
  global $processed_posts;
  $processed_posts = array();
}

/**
 * Safely invoke the "get_the_excerpt" filter
 * 
 * The reason we apply filters here is to expand shortcodes.
 * And we want to expand only those shortcodes that are allowed to expand in excerpts.
 * 
 * There is no need to do this if there aren't any shortcodes. So we do a simple check for the starting character '['
 *
 * Note: We don't want other filters to try doing things based on the global post, so we use bw_global_excerpt() to make
 * it look as if there isn't an excerpt
 *
 */
function bw_get_the_excerpt( $excerpt ) { 
  $doit = strpos( $excerpt, "[" );
  if ( $doit !== false ) { 
    $saved_excerpt = bw_global_excerpt( null ); 
    $excerpt = apply_filters( "get_the_excerpt", $excerpt );
    // bw_trace2( $excerpt, "excerpt after", false );
    bw_global_excerpt( $saved_excerpt );
  }  
  return( $excerpt );
}

/**
 * Set and retrieve the custom "read more" text
 *
 * Whenever we call bw_excerpt() call bw_more_text() to set the custom "read more" text
 * The custom "read more" text can then be accessed using the same function when creating the read more link
 * 
 * @param string $more_text - the new value for the read more text
 * @param array $atts - contains the default "read_more" text. This is null when setting and not null when retrieving 
 * @return string the current value of $bw_more_text or the default value when null
 *  
 */
function bw_more_text( $more_text=null, $atts=null ) {   
  static $bw_more_text = null;
  if ( ( $atts !== null ) && !$bw_more_text ) { 
    $bw_more = bw_array_get_dcb( $atts, "read_more", "read more" );
  } else {
    $bw_more = $bw_more_text;
  }
  $bw_more_text = $more_text; 
  bw_trace2( $bw_more, "bw_more" );
  return( $bw_more );
}

/**
 * Return the excerpt from the $post 
 * @param  post $post- post from which to extract the excerpt
 * @return string $excerpt the excerpt from the post
 *
 * Note: Most of the routines that implement the "get_the_excerpt" filter assume we're dealing with the excerpt of the main post
 * Here we are getting the excerpt of something else. We only want shortcodes to be expanded. See bw_get_the_excerpt();
 * In order to achieve this we have to make sure that the global post's excerpt is not set
 */
function bw_excerpt( $post ) {
  if ( bw_process_this_post( $post->ID ) ) {
    if ( empty( $post->post_excerpt ) ) {
      $excerpt = $post->post_content;
    } else {
      $excerpt = $post->post_excerpt;
    }
    $content = get_extended( $excerpt ); 
    $excerpt = $content['main'];
    bw_more_text( $content['more_text'] );
    
    $excerpt = bw_get_the_excerpt( $excerpt );
  } else {
    $excerpt = "post already processed: " . $post->ID;
  }  
  return( $excerpt );
}

/**
 * Format the "post" - basic first version
 *
 * Format the 'post' in a block or div with title, image with link, excerpt and read more button
 *
 * @param object $post - A post object
 * @param array $atts - Attributes array - passed from the shortcode
 * 
 *
 */
function bw_format_post( $post, $atts ) {
  bw_trace( $post, __FUNCTION__, __LINE__, __FILE__, "post" );
  $atts['title'] = get_the_title( $post->ID );
  $thumbnail = bw_thumbnail( $post->ID, $atts );
  $in_block = bw_validate_torf( bw_array_get( $atts, "block", TRUE ));
  if ( $in_block ) {
    oik_require( "shortcodes/oik-blocks.php" );
    e( bw_block( $atts ));
    if ( $thumbnail ) {
      bw_format_thumbnail( $thumbnail, $post, $atts );
    }  
  } else {
    $class = bw_array_get( $atts, "class", "" );
    sdiv( $class );
    if ( $thumbnail ) {
      bw_format_thumbnail( $thumbnail, $post, $atts );
    }
    span( "title" );
    strong( $atts['title'] );
    epan();
    br();
  }
  e( bw_excerpt( $post ) );
  bw_format_read_more( $post, $atts );  
  if ( $in_block )
    e( bw_eblock() ); 
  else {  
    sediv( "cleared" );
    ediv();  
  }    
}

/** 
 * Produce a read_more link as required
 *
 * If the read_more parameter is blank then we don't include the read more link
 * otherwise, we do using the value given, 
 * allowing each post's excerpt to have overridden the value through the <!--more custom_read_more --> tag
 * 
 * @param post $post - the actual post from which we extract the ID 
 * @param array $atts - shortcode parameters - which may contain read_more=
 */
function bw_format_read_more( $post, $atts ) {
  $read_more = bw_array_get( $atts, "read_more", true );
  if ( trim( $read_more ) !== '') {
    sp();
    $read_more = bw_more_text( null, $atts );
    art_button( get_permalink( $post->ID ), $read_more, $read_more ); 
    ep(); 
  }  
}

/**
 * Format the thumbnail when displayed in a block
 *
 * This code was originally written to display the thumbnail image left aligned in the block
 * so that the text would go to the right
 * There are problems with IE and certain themes where the alignleft styling 
 * seems to override attempts to make the image scalable.
 * So, from 2013/05/13, I'm removing the "alignleft" class, but leaving "avatar"
 * This fixed it for TwentyTwelve but not for my Artisteer theme! 
 * So I removed avatar as well.
 * This has resolved some problems with oik-plugins but it may have adverse effects on other sites.
 * 
 */
function bw_format_thumbnail( $thumbnail, $post, $atts ) {
  sdiv( );
  bw_link_thumbnail( $thumbnail, $post->ID, $atts );
  ediv();
}

/**
 * Format the "post" - in a simple list item list
 * If there is a thumbnail parameter we include the image as well! 
 * We expect the thumbnail parameter to be a small number e.g. thumbnail=80
 */
function bw_format_list( $post, $atts ) {
  // bw_trace( $post, __FUNCTION__, __LINE__, __FILE__, "post" );
  $atts['title'] = get_the_title( $post->ID );
  $attachment = ( $post->post_type == "attachment" ) ;
  $thumbnail = bw_thumbnail( $post->ID, $atts, $attachment );
  if ( $thumbnail ) {
    $title = $thumbnail . $atts['title'];
  } else {
    $title = $atts['title'];
  }
  stag( 'li' );
  alink( NULL, get_permalink( $post->ID ), $title );
  etag( 'li' );
}

/**
 * Get the list of categories for this "post" as a string of slugs separated by commas
 * 
 * @return string comma separated list of categories for the global post 
 */
function bw_get_categories() {
  global $post;
  if ( $post ) {
    $categories = get_the_category( $post->ID );
    $cats = '';
    foreach ( $categories as $category ) {
      $cats .= $category->slug;
      $cats .= ' ';
    }  
    $cats = trim( $cats );
    $cats = str_replace( ' ',',', $cats );
  } else {
    //bw_trace2( $post, "missing post" );
    //bw_backtrace();
    $cats = null;
  }    
  return bw_trace2( $cats );
}

/**
 * Wrapper to get_posts() 
 * 
 * When no parameters are passed processing should depend upon the context
 * e.g for a 'page' it should list the child pages
 *     for a 'post' it should show related posts in the same category as the current post
 * 
 * # $atts       $post->     Default
 *   post_type   post_type   processing
 * - ---------   ---------   -----------------------------------
 * 1 -           page        list child pages - first level only
 * 2 -           post        list related posts - same categories
 * 3 -           custom      none
 * 4 page        page        as 1.
 * 5 page        post        ?
 * 6 page        custom      ?
 * 7 post        page        ?
 * 8 post        post        as 2.
 * 9 post        custom      ?
 * 10-12 custom  any         ?
 *
 * As you can see from the table above the default behaviour for listing posts on pages and vice-versa is not (yet) defined
 */
function bw_get_posts( $atts = NULL ) {
  
  // Copy the atts from the shortcode to create the array for the query
  // removing the class and title parameter that gets passed to bw_block()
 
  $attr = $atts;
  bw_trace( $atts, __FUNCTION__, __LINE__, __FILE__, "atts" );
  //bw_trace( $attr, __FUNCTION__, __LINE__, __FILE__, "attr" );    
  
  /* Set default values if not already set */
  $attr['post_type'] = bw_array_get_dcb( $attr, 'post_type', NULL, "bw_global_post_type"  );
  $id = bw_array_get( $attr, "id",  null );
  
  if ( !$id ) {
    
    // Only default post_parent for post_type of 'page' 
    // This allows [bw_pages] to be used without parameters on a page
    // and to be used to list 'page's from other post types.
    // 
    if ( $attr['post_type'] == 'page' || $attr['post_type'] == 'attachment' ) {
      $attr['post_parent'] = bw_array_get_dcb( $attr, "post_parent", NULL, "bw_global_post_id" );
    }
    
    if ( $attr['post_type'] == 'post' ) {
      $attr['category_name'] = bw_array_get( $attr, "category_name", NULL );
      $attr['category'] = bw_array_get( $attr, "category", null );
      if ( NULL == $attr['category_name'] && null == $attr['category'] ) {
        $categories = bw_get_categories();
        if ( $categories ) {
          $attr['category_name'] = $categories;
        } else {
          // What do we do now? 
        }    
      }  
    }
          
    $attr['numberposts'] = bw_array_get( $attr, "numberposts", -1 );
    $attr['orderby'] = bw_array_get( $attr, "orderby", "title" );
    $attr['order'] = bw_array_get( $attr, "order", "ASC" );
  } else {
    $attr['p'] = $id; 
    $attr['page_id'] = $id; 
    $attr['post__in'] = array( $id ); 
  }
  // Regardless of the post type, exclude the current post, 
  // Note: This could also be improved **?**
  $attr['exclude'] = bw_array_get_dcb( $attr, "exclude", NULL, "bw_global_post_id" );
  
  // set suppress_filters to false when global bw_filters is set
  global $bw_filter;
  if ( isset( $bw_filter ) ) {
    $attr['suppress_filters'] = false;
  }
  
  bw_trace( $attr, __FUNCTION__, __LINE__, __FILE__, "attr" );
  
  // if ( $attr['post_type'] == 'post' ) {
    $posts = get_posts( $attr );
  // } else {
  //  $posts = get_pages( $attr );
  // }  
  bw_trace( $posts, __FUNCTION__, __LINE__, __FILE__, "posts" );
  return( $posts );
  
}

/**
 * Get the post / custom post type identified by the ID and post type
 * 
 * Note: If we know the ID then why not just call get_post()? **?**  2012/11/03
 * Changed from is_int() to is_numeric() but not yet done above 2012/12/03
 */
function bw_get_post( $post_id, $post_type, $atts=null ) {
  if ( null == $atts ) {
    $atts = array();
  }  
  if ( is_numeric( $post_id ) ) { 
    $atts['include'] = $post_id;
  } else {
    $atts['name'] = $post_id;  
  }    
  $atts['numberposts'] = 1;
  $atts['post_type'] = $post_type;
  $posts = get_posts( $atts ); 
  if ( $posts ) {
    $post = bw_array_get( $posts, 0, null );
  } else {
    // gobang() or doingitwrong or something
    $post = null;
  }  
  bw_trace2( $post );
  return( $post );
}

/**
 * Load posts by meta_key array 
 *
 * @param string $post_type - the post type required
 * @param string $meta_key - the name of the meta value to match
 * @param array $value_array - the set of values to load 
 * @return array $posts - the array of posts returned
 * 
 */
function bw_get_by_metakey_array( $post_type, $meta_key, $value_array ) { 
  $atts = array();
  $atts['post_type'] = $post_type;
  $atts['numberposts'] = -1; 
  $meta_query = array();
  $meta_query[] = array( "key" => $meta_key
                       , "value" => $value_array 
                       , "compare" => "IN"  
                       );
  $atts['meta_query'] = $meta_query;
  $atts['exclude'] = -1;
  $posts = bw_get_posts( $atts ); 
  return( $posts );
} 

