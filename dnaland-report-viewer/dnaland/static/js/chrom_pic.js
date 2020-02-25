var chromlens = [249250621, 243199373, 198022430, 191154276,
180915260, 171115067, 159138663, 146364022, 141213431, 135534747,
135006516, 133851895, 115169878, 107349540, 102531392, 90354753,
81195210, 78077248, 59128983, 63025520, 48129895, 51304566];

var chromcents = [124.2, 93.4, 91.7, 50.9, 47.7, 60.5, 58.9, 45.2,
50.6, 40.3, 52.9, 35.4, 16, 15.6, 17, 38.2, 22.2, 16.1, 28.5, 27.1,
12.3, 11.8];

var WIDTH=400;
var HEIGHT=200;

var num_autosomes = 22;
var chrom_sep = HEIGHT / 50;
var chrom_width = (1.8 * HEIGHT - 2 * chrom_sep) / 22 - chrom_sep;
var chrom_longest_len = WIDTH * 0.8;
var horiz_border = WIDTH/30;
var top_adj = HEIGHT / 40;

function draw_chart(data_json)
{
    //d3.select("#chrom_pic").append("svg").attr("height",100).attr("width",WIDTH);
    //chromosome picture
    
    //colors
    chromdark = "rgb(78,109,127)"
    chromlight = "rgb(146,197,222)"
    ibddark = "rgb(105,0,17)"
    ibdlight = "rgb(202,0,32)"
    var chrom_svg = d3.select("#chrom_pic");
    var c = chrom_svg
    .append("svg:svg")
    .attr("height", HEIGHT)
    .attr("width",  WIDTH)
    .append("svg:g");

    // border around chromosome picture
    c.append("rect")
    .attr("x",0)
    .attr("y",0)
    .attr("width",WIDTH)
    .attr("height",HEIGHT)
    .attr("fill","white")
    .attr("stroke","none");

    // ---------------
    // color gradients
    // ---------------
    var gradient_chrom = c.append("svg:defs")
    .append("svg:linearGradient")
    .attr("id", "gradient_chrom")
    .attr("x1", "0%")
    .attr("y1", "0%")
    .attr("x2", "0%")
    .attr("y2", "100%")
    .attr("spreadMethod", "pad");

    gradient_chrom.append("svg:stop")
    .attr("offset", "0%")
    .attr("stop-color", chromlight)
    .attr("stop-opacity", 1);

    gradient_chrom.append("svg:stop")
    .attr("offset", "100%")
    .attr("stop-color", chromdark)
    .attr("stop-opacity", 1);
    //-----------------
    var gradient_ibdseg = c.append("svg:defs")
    .append("svg:linearGradient")
    .attr("id", "gradient_ibdseg")
    .attr("x1", "0%")
    .attr("y1", "0%")
    .attr("x2", "0%")
    .attr("y2", "100%")
    .attr("spreadMethod", "pad");

    gradient_ibdseg.append("svg:stop")
    .attr("offset", "0%")
    .attr("stop-color", ibdlight)
    .attr("stop-opacity", 1);

    gradient_ibdseg.append("svg:stop")
    .attr("offset", "100%")
    .attr("stop-color", ibddark)
    .attr("stop-opacity", 1);

    //calculate longest chrom to scale to frame width
    var longest_chrom = 0;
    for (i = 0; i < chromlens.length; i++) {
      if (chromlens[i] > longest_chrom)   {
        longest_chrom = chromlens[i];
      }
    }

    //generate positions and dimensions for chromosomes
    var chrom_positions = [];
    for (i = 0; i < num_autosomes; i++)   {
      var xlen = chromlens[i] / longest_chrom * chrom_longest_len;
      var ylen = chrom_width;

      var x1 = horiz_border;
      var y1 = top_adj + chrom_sep + (chrom_sep + chrom_width) * i;
      if (i > 11)    {
        var chrom_partner = num_autosomes + 1 - i;
        x1 = WIDTH - horiz_border - xlen;
        y1 = top_adj + chrom_sep + (chrom_sep + chrom_width) * chrom_partner;
      }
      chrom_positions.push({ chrom : i + 1, x1 : x1, y1 : y1, xlen : xlen, ylen : ylen });
    }

    //add legend for segment types (ibd/pop)
    c.append("g").append("circle").attr("cx", WIDTH - 55).attr("cy", 13).attr("r", 3).attr("fill", "url(#gradient_ibdseg)");
    c.append("g").append("text").attr("x", WIDTH - 55 + 5).attr("y", 13 + 3).text("SNP").attr("fill", "url(#gradient_ibdseg)").attr("font-size", 12);

    //place the chromosomes
    var chrom_group = c.append("g");
    chrom_group.selectAll("rect")
    .data(chrom_positions)
    .enter()
    .append("rect")
    .attr("x", function(d)  { return d.x1; })
    .attr("y", function(d)  { return d.y1; })
    .attr("width", function(d) { return d.xlen; })
    .attr("height", function(d) { return d.ylen; })
    .attr("fill", "url(#gradient_chrom)");

    //place chromosome number label
    var text_group = c.append("g");
    text_group.selectAll("text")
    .data(chrom_positions)
    .enter()
    .append("text")
    .attr("x", function(d)  { return d.x1 - horiz_border / 1.5; })
    .attr("y", function(d)  { return d.y1 + chrom_width / 2; })
    .text(function(d) { return d.chrom; })
    .attr("fill", "black")
    .attr("font-size", (chrom_width / 2) + "px");

    //place chromosome-specific segments and curved edges
    var seg_group = c.append("g");
    for (i = 0; i < num_autosomes; i++)   {
      var xlen = chrom_positions[i].xlen;
      var ylen = chrom_positions[i].ylen;
      var x1 = chrom_positions[i].x1;
      var y1 = chrom_positions[i].y1;

      //position of centromere
      var cent_frac = chromcents[i] * Math.pow(10, 6) / chromlens[i];

      // Draw shared segments over chromosomes
      for (j = 0; j < data_json.length; j++)    {
        var segment = data_json[j];
        rectx = x1 + segment.bpstart / chromlens[i] * xlen;
        //will set the width to 0 if the bpstart point is past the length of the chromosome
        rectwidth = Math.max((Math.min(segment.bpend, chromlens[i]) - segment.bpstart) / chromlens[i] * xlen, 0);

        var seg_color = "url(#gradient_ibdseg)";
        if (segment.chrom == i + 1)   {
          seg_group.append("rect")
          .attr("x", rectx)
          .attr("width", rectwidth)
          .attr("y", y1)
          .attr("height", ylen)
          .attr("fill", seg_color);
        }
      }

      // Draw rounded ends and centromeres of chromosomes
      radius = ylen / 2;
      stroke_width = radius / 15;
      var stroke_color = "white";
      for (j = 0 - stroke_width; j < ylen + stroke_width; j += stroke_width)  {
        var line_len = radius - Math.sqrt(Math.max(2 * radius * j - j * j, 0));
        for( k = 0; k < 2; k++ )  { //k layers
          c.append("line")  //left
          .attr("x1", x1 - 1)
          .attr("y1", j + y1)
          .attr("x2", x1 + line_len)
          .attr("y2", j + y1)
          .attr("stroke-width", stroke_width)
          .attr("stroke", stroke_color)
          .attr("opacity", 1);
          if (line_len >= 0.5)  {
            c.append("line")  //middle
            .attr("x1", x1 + cent_frac * xlen - line_len)
            .attr("y1", j + y1)
            .attr("x2", x1 + cent_frac * xlen + line_len)
            .attr("y2", j + y1)
            .attr("stroke-width", stroke_width)
            .attr("stroke", stroke_color)
            .attr("opacity", 1);
          }
          c.append("line")  //right
          .attr("x1", x1 + xlen - line_len)
          .attr("y1", j + y1)
          .attr("x2", x1 + xlen + 1)
          .attr("y2", j + y1)
          .attr("stroke-width", stroke_width)
          .attr("stroke", stroke_color)
          .attr("opacity", 1);
        }
      }

      // Draw shared segments over chromosomes
      for (j = 0; j < data_json.length; j++)    {
        var segment = data_json[j];
        rectx = x1 + segment.bpstart / chromlens[i] * xlen;
        //will set the width to 0 if the bpstart point is past the length of the chromosome
        rectwidth = Math.max((Math.min(segment.bpend, chromlens[i]) - segment.bpstart) / chromlens[i] * xlen, 0);

        var seg_color = "url(#gradient_ibdseg)";
        if (segment.chrom == i + 1)   {
          c.append("rect")
          .attr("class", "chrom_pic_all")
          .attr("id", "chrom_pic--" + segment.indexsnp)
          .attr("indexsnp", segment.indexsnp)
          .attr("x", rectx)
          .attr("width", rectwidth)
          .attr("y", y1)
          .attr("height", ylen)
          .attr("fill", "none")
          .style({"stroke":"yellow", "stroke-width": 2, "visibility":"hidden"});
        }
      }
    }
    var ttdiv = c.append("g");
    //ttdiv.append("rect").attr("class", "tooltip").attr("fill", "yellow").style("height", "100px").style("width", "100px");
    //ttdiv.append("text").text("This is some information").attr("fill", "black").attr("stroke-width", 1).attr("class", "tooltip").attr("y", "100px").attr("x", "100px");
    ttdiv.append("rect").attr("id", "tooltip_box").attr("rx", 3).attr("ry", 3);
    ttdiv.append("text").attr("id", "tooltip_text").style("font-size", "10px");
}
