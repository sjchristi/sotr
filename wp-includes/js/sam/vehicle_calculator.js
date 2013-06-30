function formatDollar(num, decimalPlaces)
{
    var p = num.toFixed(2).split(".");
    var result = "$" + p[0].split("").reverse().reduce(function(acc, num, i, orig) {
        return  num + (i && !(i % 3) ? "," : "") + acc;
    }, "");
    if (decimalPlaces)
    {
    	result += "." + p[1];
    }
    return result;
}

function numberFromInput(name)
{
	var val = jQuery('#' + name).val();
	val = val.replace(',','');  // replace separators..
	val = val.replace('$','');  // remove $ signs..
	return parseFloat(val);
}

function recalculate()
{
	var avg_nightly_hotel_cost = numberFromInput('avg_nightly_hotel_cost');
	var avg_restaurant_meal_cost = numberFromInput('avg_restaurant_meal_cost');
	var avg_cooked_meal_cost = numberFromInput('avg_cooked_meal_cost');
	var avg_gas_price = numberFromInput('avg_gas_price');
	var avg_miles_per_month = numberFromInput('avg_miles_per_month');
	var total_months = numberFromInput('total_months');
	
	var options = ['a', 'b', 'c', 'd'];

	for (var i=0;i<options.length;i++)
	{
		var cost = numberFromInput('vehicle_' + options[i] + '_cost');
		var resale_value = numberFromInput('vehicle_' + options[i] + '_resale_value');
		var mpg = numberFromInput('vehicle_' + options[i] + '_mpg');
		var cooked_meals = numberFromInput('vehicle_' + options[i] + '_cooked_meals');
		var rest_meals = numberFromInput('vehicle_' + options[i] + '_rest_meals');
		var hotel_nights = numberFromInput('vehicle_' + options[i] + '_hotels');

		var meal_cost, gas_cost, hotel_cost, monthly_cost, up_front_cost, total_cost, net_cost;

		meal_cost = cooked_meals * avg_cooked_meal_cost + rest_meals * avg_restaurant_meal_cost;
		gas_cost = avg_gas_price * avg_miles_per_month / mpg;
		hotel_cost = avg_nightly_hotel_cost * hotel_nights;
		monthly_cost = meal_cost + gas_cost + hotel_cost;
		up_front_cost = cost;  // up front cost is just the cost of the vehicle for now..
		total_cost = monthly_cost * total_months + up_front_cost;
		net_cost = total_cost - (resale_value / 100.0) * cost;

		jQuery('#vehicle_' + options[i] + '_meal_cost').val(formatDollar(meal_cost, true));
		jQuery('#vehicle_' + options[i] + '_gas_cost').val(formatDollar(gas_cost, true));
		jQuery('#vehicle_' + options[i] + '_hotel_cost').val(formatDollar(hotel_cost, true));
		jQuery('#vehicle_' + options[i] + '_monthly_cost').val(formatDollar(monthly_cost, true));
		
		jQuery('#vehicle_' + options[i] + '_up_front_cost').val(formatDollar(up_front_cost, false));
		jQuery('#vehicle_' + options[i] + '_total_cost').val(formatDollar(total_cost, false));
		jQuery('#vehicle_' + options[i] + '_net_cost').val(formatDollar(net_cost, false));
	}
	
	// now highlight the fields with the best values
	var fields = ['meal_cost', 'gas_cost', 'hotel_cost', 'monthly_cost', 'up_front_cost', 'total_cost', 'net_cost'];
	for (var i=0; i<fields.length; i++)
	{
		var optionIds = [];
		for (var j=0; j<options.length; j++)
		{
			optionIds.push('vehicle_' + options[j] + '_' + fields[i]);
		}
		
		var optionVals = [];
		for (var j=0; j<options.length; j++)
		{
			optionVals.push(numberFromInput(optionIds[j]));
		}
		
		var bestOption = 1000000;
		for (var j=0; j<options.length; j++)
		{
			bestOption = Math.min(bestOption, optionVals[j]);
		}
		
		for (var j=0; j<options.length; j++)
		{
			var optionId = '#' + optionIds[j];
			if (optionVals[j] == bestOption)
			{
				jQuery(optionId).css('font-weight', 'bold');
			}
			else
			{
				jQuery(optionId).css('font-weight', 'normal');
				//var value = jQuery(optionId).val();
				//value += "  (+" + formatDollar(optionVals[j] - bestOption, false) + ")";
				//jQuery(optionId).val(value);
			}
		}
	}
}

jQuery(document).ready(function() {
	jQuery('.vehiclecalculatorinput').change(function() {
		recalculate();
	});
	recalculate();
});