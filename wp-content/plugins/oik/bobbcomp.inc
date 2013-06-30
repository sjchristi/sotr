<?php // (C) Copyright Bobbing Wide 2010-2013
//namespace bw_oik; 

// bobbcomp.inc - Company Information

/**
 * Determine the CMS type
 *
 * Sets the global $bw_cms_version for Drupal
 *
 * @return string global $bw_cms_type: "WordPress", "Drupal" or "unknown" 
 */
function bw_get_cms_type() {
  global $bw_cms_type, $bw_cms_version;
  if ( function_exists( 'is_blog_installed' )) {
    $bw_cms_type = 'WordPress';
    return( $bw_cms_type );       
  } elseif ( function_exists( 'drupal_bootstrap' )) {
    $bw_cms_type = "Drupal"; 
    if (defined( 'VERSION' ) ) {
      $bw_cms_version = VERSION;
    } 
  } else {
    $bw_cms_type = "unknown";
  }
  return( $bw_cms_type );       
}

/** 
 * Return true if the CMS is WordPress
 */
function bw_is_wordpress() {
  return( bw_get_cms_type() == "WordPress" );
}
 
/**
 * Return true if the CMS is Drupal
 */ 
function bw_is_drupal() {
  return( bw_get_cms_type() == "Drupal" );
} 

if ( bw_is_wordpress() ) {
  /**
   * Get the value of an option field
   *
   * @param string $field field name within set
   * @param string $set option name
   * @return mixed option value
   */
  function bw_get_option( $field, $set="bw_options" ) {
    /* WordPress code */
    /* Need to put some code here to see if it's been loaded */
    $bw_options = get_option( $set );
    //bw_trace( $bw_options, __FUNCTION__,  __LINE__, __FILE__, "bw_options" );  
    if ( isset( $bw_options[ $field ] ) )
      $option = $bw_options[ $field ] ; 
    else
      $option = NULL;  
      
    // Note: A value that appears to be blank ( == '') may actually be FALSE ( == FALSE )
    // bw_trace( '!' .$option.'!', __FUNCTION__,  __LINE__, __FILE__, "option" );  
    return( $option ); 
  } 
}
else {
  /* 
   * Notice the trailing underscore - used to prevent these alternative APIs from being automatically documented
   */
  oik_require( "bobbnotwp.inc_" ); 
}

/**
 * **?** Should we allow the $atts array to be pre-mangled. Can we add useful objects? 
 */
function bw_prepare_shortcode_atts( $atts ) {
  $atts = apply_filters( "bw_prepare_shortcode_atts", $atts );
  return( $atts ); 
} 

/**
 * Given a valid user ID return the required field, which may be from a set such as bw_options
 * 
 * Q. Which do we use: get_the_author_meta() or get_user_meta() **?** 
 * A. get_user_meta() is simpler but it won't return the fields from the wp_user table
 * If we're obtaining a field from a set then we must use get_user_meta()
 * otherwise we'll use get_the_author_meta()
 *
 */
function bw_get_user_field( $ID, $field, $set=null ) {
  if ( $set ) { 
    $option = get_user_meta( $ID, $set, true );
    bw_trace2( $option, "option" );
    $option = bw_array_get( $option, $field, null );
  } else {
    $option = get_the_author_meta( $field, $ID );
  }
  bw_trace2( $option, "option" );
  return( $option );
}

/**
 * Return user information 
 *
 * @param string $id - a value that can be used to access the user
 * @param string $hint - get_user_by( $hint, $id ) if we can't guess
 * @return WP_user object from get_user_by()
 * 
 * Values for $field are: 
 *  id: numeric
 *  email: contains @
 *  slug: contains "-" indicating it's a user_nicename
 *  login: anything else 
 *
 * Perhaps we should MAP username to login or viceversa anyway! **?**
 * 
 * @uses is_email()
 *
 */
function bw_get_user( $id, $hint="login" ) {
  if ( is_numeric( $id ) ) {
    $field = "id";
  } elseif ( is_email( $id ) ) {
    $field = "email";
  } elseif ( strpos( $id, "-" ) !== false ) {
    $field = "slug"; // user_nicename; 
  } else {
    $field = $hint;
  }   
  $user = get_user_by( $field, $id );
  bw_trace2( $user );
  return( $user );
}

/**
 * Return a user option field - which may come from a composite field
 */
function bw_get_user_option( $user, $field, $set=null ) {
  //oik_require( "shortcodes/oik-user.php" );
  $user = bw_get_user( $user );
  if ( $user ) {
    $ID = $user->ID; 
    $option = bw_get_user_field( $ID, $field, $set );
  } else {
    bw_trace2( "Invalid user" );
    $option = null;
  } 
  bw_trace2( $option, "option" );
  return( $option ); 
}

/**
 * Return the default user ID
 *
 * Try the current post and if that's no good the current user (if $parm is true )
 * @param bool/string $parm - whether or not to attempt to retrieve the current user ID. In most cases this will be false.
 * @return integer - the default user ID to use or null
 *
 */
function bw_default_user( $parm=false ) {
  $post = bw_global_post();
  // bw_trace2( $post );
  if ( $post ) {
    $id = $post->post_author;
  } else {
    if ( $parm ) {
      $id = bw_get_current_user_id();
    } else {
      $id = null;
    }
  }
  return( $id );
}

/**
 * Return the current user ID if there is one
 * 
 * @return ID|null - the current user ID or null
 */
function bw_get_current_user_id() {
  $user = wp_get_current_user();
  if ( $user ) {
    $id = $user->ID;
  }
  else {
    $id = null;
  }
  return( $id );
}  

/**
 * Retrieve the requested option value depending on the $atts array
 *
 * @param string $field - the name of the field to obtain
 * @param string $set - the name of the set from which to obtain the field 
 * @param array $atts - the set of parameters which may include user= or alt= values
 * @return string - the value of the required option or null
 * 
 * We're trying to eliminate alt=1 or alt=2 in favour of user fields but only if oik-user is active.
 * The alt= keyword overrides the user= keyword, for backward compatibility
 * Use user=0 or alt=0 to force the use of "oik options"
 * Use alt=1 or alt=2 to use "more options" or "more options 2" 
 *
 * Note: The author is null when there is no current post being processed.
 * <pre>
 * oik-user  alt=  user=  author  processing
 * --------  ----  -----  ------  --------------------------------------------
 * active    set   n/a    n/a     use the alt= value ( 0 is treated as '' )
 * active    null  set    n/a     use the value specified for the user options
 * active    null  null   null    use oik options
 * active    null  null   set     use the user options
 *                 
 * inactive  set   n/a    n/a     use the alt= value
 * inactive  null  n/a    n/a     use the oik options 
 * </pre>
 */
function bw_get_option_arr( $field, $set="bw_options", $atts=null ) {
  $alt = bw_array_get( $atts, "alt", null );
  if ( is_callable( "oiku_loaded" ) ) {
    if ( $alt === null ) {
      $user = bw_array_get_dcb( $atts, "user", false, "bw_default_user" );
      if ( $user ) {
        $option = bw_get_user_option( $user, $field, $set );
      }
    } else { 
      $user = null;
    }  
  } else {
    $user = null; 
  }
  if ( !$user ) { 
    if ( !$set ) {
      $set="bw_options";
    }
    if ( $alt == "0" )  
      $alt = "";
    $option = bw_get_option( $field, "$set$alt" );
  }
  return( $option );  
}

/**
 * Return the tooltip value for "me" 
 * 
 * The name displayed in the tooltips comes from "me=" parameter, oik (user) options contact or user display_name field
 */
function bw_get_me( $atts=null ) {
  $me = bw_array_get( $atts, "me", null );
  if ( !$me ) {
    $me = bw_get_option_arr( "contact", "bw_options", $atts );
    if ( !$me ) {
      $me = bw_get_option_arr( "display_name", null, $atts );
      if ( !$me ) {
        $me = __( "me" );
      }
    }
  }
  return( $me ); 
}

/**
 * Display the value of a 'field' in a span with a class of the field name
 *
 * @param string $field - the name of the option field to display
 * @return string - the formatted field
 */
function bw_output( $field ) {
  $fieldvalue = bw_get_option( $field );
  span( $field );
  e( $fieldvalue );
  epan();
  return( bw_ret() );
}

/**
 * Implement the [clear] shortcode to create a div to clear the floats
 * 
 * class cleared is used for Artisteer themes
 * class clear is the simpler version in oik
 */
function bw_clear( $atts=null ) {
  sediv( "cleared clear" );
  return( bw_ret());
} 
 
/** 
 * 
 * bw_oik() is needed here since it's used in the oik admin pages
 *   
 */
function bw_oik( $atts=null ) {
  $class = bw_array_get( $atts, "class", "bw_oik" );
  $bw = nullretstag( "span", $class );
  $bw .= "oik"; 
  $bw .= nullretetag( "span", $class ); 
  bw_trace2( $bw, "bw" );
  return( $bw );
}

/** 
 * bw_oik_long - spells out Often Included Key-information
 *   
 */
function bw_oik_long( $atts=null ) {
  $class = bw_array_get( $atts, "class", "bw_oik" );
  $bw = nullretstag( "span", $class ); 
  $bw .= "Often Included Key-information"; 
  $bw .= nullretetag( "span", $class ); 
  return( $bw );
}

/**
 * Start a div tag
 * Use in conjunction with ediv to add custom divs in pages, posts or blocks
 * e.g. [div class="blah"]blah goes here[ediv]
 */
function bw_sdiv( $atts ) {
  $class = bw_array_get( $atts, 'class', null );
  $id = bw_array_get( $atts, 'id', null );
  sdiv( $class, $id );
  return( bw_ret());
}

/**
 * End a div tag 
 */ 
function bw_ediv( $atts ) {
  ediv();
  return( bw_ret());
}

/**  
 * Create an empty div for a particular set of classes, id or both
 * e.g. [sediv class="bd-100"] 
 */
function bw_sediv( $atts=null ) {
  $class = bw_array_get( $atts, 'class', null );
  $id = bw_array_get( $atts, 'id', null );
  sediv( $class, $id );
  return( bw_ret());
}
 
/** 
 * Display the company abbreviation using an abbr tag
 */
function bw_abbr( $atts = NULL ) {
  $abbr = bw_get_option( "abbr" );
  $company = bw_get_option( "company" );
  abbr( $company, $abbr );
  return( bw_ret());
}
  
/** 
 * Display the company abbreviation using an acronym tag
 * there is a subtle difference between the two: abbr and acronym
 * see (for example) http://www.benmeadowcroft.com/webdev/articles/abbr-vs-acronym/   
 */
function bw_acronym( $atts = NULL ) {
  $abbr = bw_get_option( "abbr" );
  $company = bw_get_option( "company" );
  acronym( $company, $abbr );
  return( bw_ret());
}


/**
 * Set a default value for an empty attribute value from the oik options or a hardcoded value
 * @param string $bw_value - value passed... if not set then
 * @param string $bw_field - get the oik option value - this is the field name of the oik option - e.g. 'company'
 * @param string $bw_default - the (hardcoded) default value if the oik option is not set
 * @param string $set - options set from which the field should be returned 
 *
 * e.g. 
 * $width = bw_default_empty_att( $width, "width", "100%" );
 * 
*/
function bw_default_empty_att( $bw_value=NULL, $bw_field=NULL, $bw_default=NULL, $set="bw_options" ) {
  $val = $bw_value;
  if ( empty( $val )) {
    $val = bw_get_option( $bw_field, $set );
    if ( empty( $val ))
      $val = $bw_default;
  } 
  return( $val );
}

/** 
 * Return the array[index] or build the result by calling $callback, passing the $default as the arg.
 *
 * @param array $array array from which to obtain the value
 * @param string $index - index of value to obtain]
 * @param mixed $default - parameter to the $callback function 
 * @param string $callback - function name to invoke - defaults to invoking __()
 *
 * Notes: dcb = deferred callback
 * Use this function when applying the default might take some time but would be unnecessary if the $array[$index] is already set.
 *
 * You can also use this function when the default value is a string that you want to be translated.
 *
 * 2012/10/23 - When the parameter was passed as a null value e.g. "" then it was being treated as NULL
 * hence the default processing took effect. 
 * In this new verision we replace the NULLs in the code body with $default
 * So bw_array_get() can return a given NULL value which will then override the default.
 * In this case, if the parameter that is passed turns out to be the default value then this will also be translated.
 * Note: It could could still match a default null value
 * Also: We don't expect a null value for the default callback function __()
 * 2012/12/04 - we have to allow for the value being set as 0 which differs from a default value of NULL
 * so the comparison needs to be identical ( === ) rather than equal ( == ) 
 */
function bw_array_get_dcb( $array = array(), $index, $default = NULL, $callback='__' ) {
  $value = bw_array_get( $array, $index, $default );
  if ( $value === $default ) {
    if ( is_callable( $callback ) ) {
      $value = call_user_func( $callback, $default ); 
    } else {
      bw_backtrace();
    }
  }  
  return( $value );  
}

/**
 * Set a default value for an empty array[ index]
 * @param bw_array - array containing the value
 * @param bw_index - index to check... if not set then 
 * @param bw_field - get the oik option value - this is the field name of the oik option - e.g. 'company'
 * @param bw_default - the (hardcoded) default value if the oik option is not set
 *
 * e.g. 
 * $width = bw_default_empty_arr( $atts, "width", "width", "100%" );
 * 
*/
function bw_default_empty_arr( $bw_array=NULL, $bw_index=NULL, $bw_field=NULL, $bw_default=NULL ) {
  $val = bw_array_get( $bw_array, $bw_index, NULL );
  if ( empty( $val )) {
    bw_trace( $bw_field, __FUNCTION__, __LINE__, __FILE__, "bw_field" );
    $val = bw_get_option( $bw_field );
    bw_trace( $val, __FUNCTION__, __LINE__, __FILE__, "value" );
    if ( empty( $val ))
      $val = $bw_default;
  } 
  bw_trace( $val, __FUNCTION__, __LINE__, __FILE__, "value" );
  return( $val );
}


/**
 * return a nice SEO title
 * taking into account which plugins are being used
 */
if (!function_exists( 'bw_wp_title' )) {
  function bw_wp_title() {
    if ( class_exists( 'WPSEO_Frontend' )) {
      $title = wp_title('', false );
    }
    else {
      $title = wp_title( '|', false, 'right' ); 
      $title .= get_bloginfo( 'name' );
    }
    return $title;
  }
}

/** 
 * Display a blockquote unaffected by wpautop
 */
function bw_blockquote( $atts = NULL ) {
  $text = bw_array_get( $atts, "text", NULL ); 
  $class = bw_array_get( $atts, "class", NULL );
  blockquote( $text, $class );
  return( bw_ret());
}

/**
 * Display a <cite> unaffected by wpautop 
 */
 
function bw_cite( $atts = NULL ) {
  $text = bw_array_get( $atts, "text", NULL ); 
  $class = bw_array_get( $atts, "class", NULL );
  cite( $text, $class );
  return( bw_ret());
}

/**
 *  Format a range of years
 *
 *  $yearfrom $yearto result
 *  x         x       x
 *  x         y       x,y
 *  x         z       x-z
 *
 *  where: z >= (y + 1) = (x + 1)
 *  if $atts['sep'] is set we use that
 */
function bw_year_range( $yearfrom, $yearto, $atts=NULL ) {
  if ( !$yearfrom ) {
    $yearfrom = $yearto;
  }  
  $diff = $yearto - $yearfrom;
      
  switch ( $diff ) {
    case 0:
      $years = $yearfrom;
      break;
    case 1:
      $sep = bw_array_get( $atts, "sep", "," );
      $years = "$yearfrom$sep$yearto";
      break;
    default: /* more than one OR negative - which is a bit of a boo boo */
      $sep = bw_array_get( $atts, "sep", "-" );
      $years = "$yearfrom$sep$yearto";
  }
  return( $years );    
} 

/**
 * Determine the date of the blog from the date of the earliest registered user
  
  (
    [0] => stdClass Object
        (
            [ID] => 1
            [user_login] => admin
            [user_pass] => $P$BijsY7/BdZ9AzR8YdJwYVVt68FBovk0
            [user_nicename] => admin
            [user_email] => info@bobbingwide.com
            [user_url] => 
            [user_registered] => 2010-12-23 12:22:39
            [user_activation_key] => qLc3INyEWwBOsfFDnZeV
            [user_status] => 0
            [display_name] => admin
        )

)
 */
function bw_get_yearfrom() {
  $users = bw_get_users();
  bw_trace2( $users );
  $yearfrom = bw_format_date( $users[0]->user_registered, 'Y' );

  return $yearfrom;
}


/** 
 * Simple wrapper to get_users
 *
 * @param array $atts - array of key value pairs 
 * Default parameters to get_users() are:
 * <pre>
 
				'blog_id' => $GLOBALS['blog_id'],
				'role' => '',
				'meta_key' => '',
				'meta_value' => '',
				'meta_compare' => '',
				'include' => array(),
				'exclude' => array(),
				'search' => '',
				'orderby' => 'login',
				'order' => 'ASC',
				'offset' => '',
				'number' => '',
				'count_total' => true,
				'fields' => 'all',
				'who' => ''
  </pre>                                
 * @return array - array of user objects  
 *
 */
function bw_get_users( $atts = NULL ) {
  $atts['orderby'] = bw_array_get( $atts, "orderby", "registered" );
  $atts['number'] = bw_array_get( $atts, "number", 1 );
  $users = get_users( $atts );
  return( $users );
}

/** 
 * Build a simple ID, title array from an array of $user objects
 * @param array $user - array of user objects
 * @return array - associative array of user ID to user_title
 */
if ( !function_exists( "bw_user_array" ) ) { 
function bw_user_array( $users ) {
  $options = array();
  foreach ($users as $user ) {
    $options[$user->ID] = $user->display_name; 
  }
  return bw_trace2( $options );
}

/**
 * Return an associative array of all users
 * @return array - associative array of user ID to user_title
 */
function bw_user_list() {
  $users = bw_get_users( array( "number" => "" )) ;
  $userlist = bw_user_array( $users );
  return( $userlist );
}
}

/** 
 * Display the copyright statement for the company
 * showing start and end years
 * e.g. (C) Copyright [bw_company] [bw_from]&sep[year]&suffix
 * where [bw_from] is the first year of the site
 * &sep is the separator ( defaults to ',' for one year apart and '-' for a range ) 
 * [year] represents the current year
 * &suffix defaults to ". All rights reserved." 
 *
 */ 
function bw_copyright( $atts = NULL ) {
  $copyright = bw_array_get_dcb( $atts, "prefix", "&copy; Copyright" );
  $company = bw_array_get_dcb( $atts, "company", "company", "bw_get_option" );
  $suffix = bw_array_get_dcb( $atts, "suffix", ". All rights reserved." );
  $yearto = bw_format_date( null, 'Y');
  // bw_trace( $yearto, __FUNCTION__, __LINE__, __FILE__, "yearto!" );
  $yearfrom = bw_array_get_dcb( $atts, "from", "yearfrom", "bw_get_option" );
  
  $years = bw_year_range( $yearfrom, $yearto, $atts );
  span( "bw_copyright" );
  e( "$copyright " );
  e( bw_do_shortcode( "$company ") );
  e( "$years" );
  e( "$suffix" );
  epan();
  return( bw_ret());
}

function bw_stag( $atts = NULL ) {
  $tag = bw_array_get( $atts, "name", NULL );
  $class = bw_array_get( $atts, "class", NULL );
  $id = bw_array_get( $atts, "id", NULL );
  stag( $tag, $class, $id );
  return( bw_ret());
}

function bw_etag( $atts = NULL ) {
  $tag = bw_array_get( $atts, "name", NULL );
  etag( $tag );
  return( bw_ret());
}

/**
 * Return the value from a list of possible parameters
 *
 * @param array $atts - an array of key value pairs
 * @param mixed $from - a list e.g. ( "api,func" ) or array of key names  
 * @param string $default - the default value if not set 
 * @return string - the first value found or the default
 */
function bw_array_get_from( $atts, $from, $default ) {
  $from = bw_as_array( $from );
  $fc = count( $from );
  $f = 0;
  $result = null;
  while ( ( $f < $fc ) && $result === null ) {
    $result = bw_array_get( $atts, $from[$f], null );  
    $f++;
  }
  if ( !$result ) {
    $result = $default;
  }
  return( $result );      
}

/** 
 * Split a string into an array if necessary
 *
 * @param mixed $mixed - either an array already or a string of comma or blank separated values
 * @return array - an unkeyed array 
 */
function bw_as_array( $mixed ) {
  if ( $mixed ) {
    if ( is_array( $mixed ) ) {
      $mixed_array = $mixed;
    } else { 
      $mixed = str_replace( ",", " ", $mixed );
      $mixed_array = explode( " ", $mixed );
    } 
    bw_trace2( $mixed_array, "mixed_array" ); 
  } else {
    $mixed_array = array();
  }      
  return( $mixed_array );
}

/**
 * Return all the unkeyed items as an unkeyed array
 * 
 * @param mixed $array - array from which to extract the unkeyed values
 * @param bool $split - whether or not to break down the values further
 * @return mixed array of results
 *
 * e.g. 
   $atts = array( "san ta"
                , "claus"
                , "zip" => "a"
                , "diddy" => "do"
                , "dah" => "day"
                , "the"
                , "mo,vie"
                );
   $unkeyed = array( "san", "ta", "claus", "the", "mo", "vie" );
                
 */
function bw_array_get_unkeyed( $array=null, $split=true ) {
  $unkeyed = array();
  if ( is_array( $array) && count( $array ) ) {
    foreach ( $array as $key => $value ) {
      if ( is_numeric( $key ) ) {
        if ( $split ) {
          $value = str_replace( "," , " ", $value );  
          $values = explode( " ", $value );
          foreach ( $values as $valu ) {
            $unkeyed[] = $valu;
          }  
        } else {
          $unkeyed[] = $value ;
        }  
      }
    }
  }
  return( $unkeyed );
}



