<?php // (C) Copyright Bobbing Wide 2011-2013

/**
 * Implement [bw_address] shortcode to display an address using Microformats
 * 
 * 
 */
function bw_address( $atts=null, $content=null, $tag=null ) {
  $type = bw_array_get( $atts, "type", "Work" );
  sdiv("adr bw_address" );
    sdiv("type");
    e( $type );
    ediv();
    sdiv("extended-address");
    e( bw_get_option_arr( "extended-address", "bw_options", $atts ) );
    ediv();
    sdiv("street-address");
    e( bw_get_option_arr( "street-address", "bw_options", $atts ) );
    ediv();  
    sdiv("locality");
    e( bw_get_option_arr( "locality", "bw_options", $atts ) );
    ediv();      
    sdiv("region");
    e( bw_get_option_arr( "region", "bw_options", $atts ) );
    ediv();      
    sdiv("postal-code");
    e( bw_get_option_arr( "postal-code", "bw_options", $atts ) );
    ediv();
    span("country-name");
    e( bw_get_option_arr( "country-name", "bw_options", $atts ) );
    epan();
  ediv();
  return( bw_ret() );
}
