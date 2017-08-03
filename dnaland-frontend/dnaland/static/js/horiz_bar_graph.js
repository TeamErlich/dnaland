// bar chart figure
var margin = {top: 20, right: 30, bottom: 40, left: 30},
    barchart_width = 400 - margin.left - margin.right,
    barchart_height = 400 - margin.top - margin.bottom;

var x = d3.scale.linear()
    .range([0, barchart_width]);

var y = d3.scale.ordinal()
    .rangeBands([0, barchart_height], 0.1, 0.1);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom");

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("right")
    .tickSize(0)
    .tickPadding(6);

var svg = d3.select("#wrapper").append("svg")
    .attr("width", barchart_width + margin.left + margin.right)
    .attr("height", barchart_height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

//x.domain(d3.extent(chrom_pic_snps, function(d) { return d.diff_effect_size })).nice();
x.domain(d3.extent(
            [].concat(chrom_pic_snps.map(function(d) { return d.diff_effect_size; }),
                      chrom_pic_snps.map(function(d) { return d.max_effect_size; }),
                      chrom_pic_snps.map(function(d) { return d.min_effect_size; })))).nice();
y.domain(chrom_pic_snps.map(function(d) { return d.indexsnp }));

var effect_bars = svg.selectAll(".bar")
    .data(chrom_pic_snps)
    .enter();
effect_bars.append("rect")
    .attr("class", "bar bar--negative")
    .attr("x", function(d) { return x(Math.min(0, d.max_effect_size)); })
    .attr("y", function(d) { return y(d.indexsnp); })
    .attr("width", function(d) { return Math.abs(x(d.max_effect_size) - x(0)); })
    .attr("height", y.rangeBand())
    .on("click", function(d){ hl_snp(d.indexsnp); })
    .style("cursor", "pointer");
effect_bars.append("rect")
    .attr("class", "bar bar--negative")
    .attr("x", function(d) { return x(Math.min(0, d.min_effect_size)); })
    .attr("y", function(d) { return y(d.indexsnp); })
    .attr("width", function(d) { return Math.abs(x(d.min_effect_size) - x(0)); })
    .attr("height", y.rangeBand())
    .on("click", function(d){ hl_snp(d.indexsnp); })
    .style("cursor", "pointer");
effect_bars.append("rect")
    .attr("class", "bar bar--positive")
    .attr("x", function(d) { return x(Math.min(0, d.diff_effect_size)); })
    .attr("y", function(d) { return y(d.indexsnp); })
    .attr("width", function(d) { return Math.abs(x(d.diff_effect_size) - x(0)); })
    .attr("height", y.rangeBand())
    .on("click", function(d){ hl_snp(d.indexsnp); })
    .style("cursor", "pointer");

svg.append("g")
  .attr("class", "x axis")
  .attr("transform", "translate(0," + barchart_height + ")")
  .style({"font": "10px sans-serif"})
  .call(xAxis);

svg.append("g")
  .attr("id", "yaxis")
  .attr("class", "y axis")
  .attr("transform", "translate(" + x(0) + ",0)")
  .style({"font": Math.min(barchart_height/chrom_pic_snps.length, 10) + "px sans-serif"})
  .call(yAxis);

d3.select("#yaxis")
  .selectAll(".tick")
  .on("click", function(d){ hl_snp(d); })
  .style("cursor", "pointer");

// legend above bar chart
var svg_leg = d3.select("#bar_legend").append("svg").attr("width", barchart_width).attr("height", 40);
var legend_colors = [["Your Effect Size", "bar--positive"], ["Min/Max Possible Effect", "bar--negative"]];
var legend = svg_leg.selectAll(".legend")
        .data(legend_colors)
        .enter().append("g")
        .attr("class", function(d){ return d[1]; })
        .attr("transform", function(d, i) { return "translate(0," + i * 20 + ")"; });
legend.append("rect")
        .attr("x", 0)
        .attr("width", 18)
        .attr("height", 18);
legend.append("text")
        .attr("x", 20)
        .attr("y", 9)
        .attr("dy", ".35em")
        .style("text-anchor", "start")
        .html(function(d) { return d[0]; });
