var dists=dists;                      //input bar heights for histogram
var dists_ml=dists_ml;
var dists_fm=dists_fm;
var show_male_female_histograms=show_male_female_histograms;
var requested_div=requested_div;    //div for displaying histogram
var user_val=user_val;              //bar showing user val in relation to histogram
var lower_hist_x_label=lower_hist_x_label;
var upper_hist_x_label=upper_hist_x_label;
var trait_x_label=trait_x_label;

var HIST_WIDTH=800;
var HIST_HEIGHT=400;
var d_hist_width = HIST_WIDTH / 1.6;
var d_hist_height = HIST_HEIGHT / 1.6;

console.log(dists)
var data_minx = d3.min(dists, function(d){ return d.x; });
var data_maxx = d3.max(dists, function(d){ return d.x; });
var data_miny = d3.min(dists.concat(dists_ml, dists_fm), function(d){ return d.y; });
var data_maxy = d3.max(dists.concat(dists_ml, dists_fm), function(d){ return d.y; });

// histogram
var width_margin = 60;
var width_margin_right = 5; //this side doesn't need to accommodate the axes
var height_margin = 20;
var x = d3.scale.linear().range([width_margin, d_hist_width - width_margin_right]).domain([
            data_minx - 0.1 * (data_maxx - data_minx), 
            data_maxx + 0.1 * (data_maxx - data_minx)]);
var y = d3.scale.linear().range([d_hist_height - height_margin, height_margin]).domain([
            0,
            data_maxy + 0.1 * (data_maxy - data_miny)]);
var xAxis = d3.svg.axis().scale(x).orient("bottom").ticks(5);
var yAxis = d3.svg.axis().scale(y).orient("left").ticks(5);

var lineGen = d3.svg.line()
.x(function(d) { return x(d.x); })
.y(function(d) { return y(d.y); })
.interpolate("basis");


var super_dhist_svg = d3.select(requested_div);

//var dhist_svg = row.append("td")
var dhist_svg = super_dhist_svg
.append("svg")
.attr("height", d_hist_height + height_margin)
.attr("width", d_hist_width + width_margin)
.style("font-size", HIST_HEIGHT/35);

dhist_svg.append("g")
.attr("class", "x axis")
.attr("transform", "translate(0," + (d_hist_height - height_margin) + ")")
.call(xAxis);

dhist_svg.append("g")
.attr("class", "y axis")
.attr("transform", "translate(" + (width_margin) + ", 0)")
.call(yAxis);

dhist_svg.selectAll(".axis line, .axis path").style({"stroke": "Black", "fill": "none", "stroke-width": "1px"});

//axes labels
dhist_svg.append("text")
.attr("x", d_hist_width/2)
.attr("y", d_hist_height + height_margin/2 + 8)
.style("text-anchor", "middle")
.text(trait_x_label)
.attr("font-size", 10);

dhist_svg.append("text")
.attr("x", d_hist_width/6)
.attr("y", d_hist_height + height_margin/2 + 8)
.style("text-anchor", "middle")
.html(lower_hist_x_label)
.attr("font-size", 10);
//.attr("fill", "#7535B9");

dhist_svg.append("text")
.attr("x", d_hist_width*6/7)
.attr("y", d_hist_height + height_margin/2 + 8)
.style("text-anchor", "middle")
.html(upper_hist_x_label)
.attr("font-size", 10);
//.attr("fill", "#F49903");

dhist_svg.append("text")
.attr("transform", "rotate(-90)")
.attr("y", 20)
.attr("x", 0 - d_hist_width/3.5 + width_margin/2 - 10)
.style("text-anchor", "middle")
.text("Percentile")
.attr("font-size", 10);

dhist_lists = [dists, dists_ml, dists_fm];
dhist_classes = ["all", "male", "female"];
dhist_stroke = ["mediumpurple", "lightskyblue", "lightcoral"];
dhist_fill = ["mediumpurple", "lightskyblue", "lightcoral"];
if (show_male_female_histograms) {
    dhist_opacity = [1.0, 1.0, 1.0];
} else {
    dhist_opacity = [1.0, 0.0, 0.0];
}
for (i = 0; i < dhist_lists.length; i++) {
    dhist_temp = dhist_lists[i];
    //draw the line (all users)
    dhist_svg.append("svg:path")
    .attr("class", dhist_classes[i] + " " + dhist_classes[i] + "_line")
    .attr("d", lineGen(dhist_temp))
    .attr("stroke", dhist_stroke[i]) //"rgb(51, 51, 255)"
    .attr("stroke-width", 2)
    .attr("fill", "none")
    .attr("opacity", dhist_opacity[i]);

    var areaGen = d3.svg.area()
    .x(function(d) { return x(d.x); })
    .y0(d_hist_height - height_margin)
    .y1(function(d) { return y(d.y); })
    .interpolate("basis");
    dhist_svg.append("svg:path")
    .datum(dhist_temp)
    .attr("class", dhist_classes[i] + " " + dhist_classes[i] + "_area")
    .attr("d", areaGen)
    .attr("fill", dhist_fill[i]) //"rgb(102, 178, 255)"
    .attr("opacity", 0.6 * dhist_opacity[i]); 
}

//draw current user's position in distribution
dhist_svg.append("line")
.attr("x1", x(user_val))
.attr("y1", 25)
.attr("x2", x(user_val))
.attr("y2", d_hist_height - height_margin)
.attr("stroke", "darkblue")
.attr("stroke-width", 3)
.attr("stroke-dasharray", "4,4");

//text bubble above user's position
dhist_svg.append("polygon")
.attr("points", x(user_val) + ",25 " + (x(user_val)+5) + ",15 " + (x(user_val)-5) + ",15") 
.style("fill", "darkblue")
.style("opacity", 1.0);

dhist_svg.append("rect")
.attr("rx", 6)
.attr("ry", 6)
.attr("x", x(user_val)-15)
.attr("y", 0)
.attr("width", 30)
.attr("height", 15)
.attr("fill", "darkblue")
.style("opacity", 1.0);

dhist_svg.append("text")
.attr("x", x(user_val))
.attr("y", 15 - 3)
.style("text-anchor", "middle")
.style("fill", "white")
.html("You");

//add text for showing male/female distributions
/*
dhist_svg.append("text")
.attr("id", "text_button")
.attr("show", true)
.attr("y", 10) 
.attr("x", d_hist_width - 80)
.style("text-anchor", "middle")
.style("cursor", "pointer")
.style("fill", "#0000EE")
.html("<a>Show Male/Female Scores</a>")
.attr("font-size", 12)
.on("click", function() {
    if (d3.select("#text_button").html() == "<a>Show Male/Female Scores</a>") {
        d3.select("#text_button").html("<a>Show All User Scores</a>");
        d3.selectAll(".hist_sep_line").transition().duration(500).style("opacity", 1.0);
        d3.selectAll(".hist_sep_area").transition().duration(500).style("opacity", 0.6);
        d3.selectAll(".hist_all").transition().duration(500).style("opacity", 0.0);
    } else {
        d3.select("#text_button").html("<a>Show Male/Female Scores</a>");
        d3.selectAll(".hist_all_line").transition().duration(500).style("opacity", 1.0);
        d3.selectAll(".hist_all_area").transition().duration(500).style("opacity", 0.6);
        d3.selectAll(".hist_sep").transition().duration(500).style("opacity", 0.0);
    }
});

//male/female labels
mf_labels = ["Female", "Male"];
mf_colors = ["lightcoral", "lightskyblue"];
for (i=0; i<mf_labels.length; i++)  {
    dhist_svg.append("text")
    .attr("class", "hist_sep hist_sep_line")
    .attr("show", true)
    .attr("y", 15+10*(i+1))
    .attr("x", d_hist_width - 90)
    .style("text-anchor", "left")
    .text(mf_labels[i])
    .attr("font-size", 12)
    .style("fill", mf_colors[i])
    .style("opacity", 0.0);
}
*/
