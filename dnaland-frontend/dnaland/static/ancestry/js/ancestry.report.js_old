// Ancestry Report

// -----------------------------------------------------------------------------
// -- Config
// -----------------------------------------------------------------------------

// Chart Settings
var _AC_HEIGHT           = 300
var _AC_WIDTH            = 300 + 50 // margin of 50
var _AC_RADIUS           = Math.min(_AC_WIDTH, _AC_HEIGHT) / 2
var _AC_OPACITY_ON_HOVER = 0.3
//
var _AC_LEGEND_HEIGHT    = 50
var _AC_LEGEND_WIDTH     = 250
var _AC_LEGEND_MARGIN    = 10
var _AC_LEGEND_RADIUS    = 5
// Map Settings
var _AM_WIDTH            = 900
var _AM_HEIGHT           = 380
var _AM_V_TRANSLATION    = 225 // Deliberately cut off Antarctica

var display_names = {
    "AFRICA": "African",
    "AMERICAS" : "Native American", // Appears unused, in favor of CSAMERICA
    "ARABIAN": "Arab",
    "ASHKENAZI": "Ashkenazi",
    "ASHKENAZI-EMED" : "Ashkenazi/Levantine",
    "ASIA": "East Asian",
    "BALOCHI-MAKRANI-BRAHUI": "Indo-Iranian",
    "BANTUKENYA": "Central African",
    "BANTUNIGERIA": "Lower Niger Valley", // Improve this
    "BIAKA": "Aka",
    "CAFRICA" : "Mbuti", // Be suspicious
    "CAMBODIA-THAI": "Cambodian/Thai",
    "CASIA" : "Central Asian",
    "CSAMERICA": "Native American",
    "CYPRUS-MALTA-SICILY": "Mediterranean Islander",
    "EAFRICA" : "East African",
    "EAFRICA_CL" : "East African",
    "EASIA" : "Central Chinese",
    "EASIA_CL" : "East Asian",
    "EASTSIBERIA": "Siberian",
    "FINNISH": "Finnish",
    "GAMBIA": "Senegal River Valley", // Improve this
    "GUJARAT": "Gujarati",
    "HADZA": "Hadza", // Be suspicious
    "HAZARA-UYGUR-UZBEK": "Mid-Turkic",
    "ITALY" : "South/Central European",
    "ITALY-BALKANS" : "South European",
    "JAPAN-KOREA": "Japanese/Korean",
    "KALASH": "Kalash",
    "MENDE": "Mende/Akan", // Improve this
    "NAFRICA" : "North African",
    "NCASIA" : "East-Turkic",
    "NEAREAST" : "Arab/Egyptian",
    "NEASIA" : "Mongolic/Tungusic",
    "NEASIA_CL" : "Northeast Asian",
    "NEEUROPE" : "North Slavic",
    "NEEUROPE_CL" : "Northeast European",
    "NEUROPE" : "Northwest European",
    "NGANASAN": "Nganasan",
    "OCEANIA" : "Native Oceanian",
    "PATHAN-SINDHI-BURUSHO": "Indus Valley",
    "PYGMY": "Pygmy",
    "SAFRICA" : "Southern African",
    "SAMERICA": "Amazonian",
    "SARDINIA": "Sardinian",
    "SASIA" : "South Asian",
    "SBALKANS" : "Balkan",
    "SEASIA" : "Southeast Asian",
    "SEASIA_CL" : "Southeast Asian",
    "SSASIA" : "Dravidian",
    "SWEUROPE" : "Southwestern European",
    "SWEUROPE_CL" : "Southwestern European",
    "TAIWAN": "Taiwanese",
    "TUBALAR": "Tubalar",
    "TURK-IRAN-CAUCASUS" : "Central Indoeuropean",
    "WAFRICA" : "West African",
    "WEURASIA": "West Eurasian",
}

// I am deeply suspicious of the division of East Africa into EAFRICA and BANTUKENYA
// It matches neither geography nor anthropology
// For now, let's just put them together
for (var i in pop_sources['BANTUKENYA']) {
    pop_sources['EAFRICA'].push(pop_sources['BANTUKENYA'][i]);
}
delete pop_sources['BANTUKENYA'];

// These populations live amidst unrelated peoples, and need to be displayed specially
// The radii are roughly how far (in unzoomed pixels) from their sample-points each population roams
// The numbers are completely made up, but they make the map look about right
var nongeo_radii = {'ASHKENAZI': 15,
                    'BIAKA': 15,
                    'CAFRICA': 10,
                    'HADZA': 10};

// There are a few cases where geographical coloring really doesn't look right.  To avoid them,
// pretend we had a few extra sources.  These will effect coloring, but will not be displayed or
// listed.
pop_sources['NAFRICA'].push('FAKE_libya');
allsources.push(['FAKE_libya', 'FAKE', 'Libya', '32', '16']);
pop_sources['NEAREAST'].push('FAKE_eq');
allsources.push(['FAKE_eq', 'FAKE', 'Saudi Arabia (the empty quarter)', '18', '54']);
pop_sources['NEUROPE'].push('FAKE_germany');
allsources.push(['FAKE_germany', 'FAKE', 'Germany', '52', '9']);
pop_sources['CSAMERICA'].push('FAKE_greenland');
allsources.push(['FAKE_greenland', 'FAKE', 'Greenland', '70', '-27']);
pop_sources['OCEANIA'].push('FAKE_philip');
allsources.push(['FAKE_philip', 'FAKE', 'Phillipines', '13', '121']);
pop_sources['OCEANIA'].push('FAKE_indo');
allsources.push(['FAKE_indo', 'FAKE', 'Indonesia', '7', '106']);

// -----------------------------------------------------------------------------
// -- Global variables
// -----------------------------------------------------------------------------

// Note: Throughout this codebase, the following terminology is used consistently:
//   a "source" is a set of reference people at a specific location
//   a "population" or "pop" is a collection of genetically indistinguishable sources
//   a "group" is either a pop or set of related pops

// The allsources array (in popsources.js) contains both sources we use and those we throw out.
// The sources array contains only those we use, with which population uses them appended to each row.
var sources=[];

var show_on_map = [{}, {}, {}];
var ac_colors = {'Ambiguous':'white', 'All': 'grey'};
var inctext={}, dnitext={}, hltext={};
var map_data={group_td:{}};
var pop_wedge={};

var COLOR=0;
var HYBRID=1;
var GREY=2;

// -----------------------------------------------------------------------------
// -- General Code
// -----------------------------------------------------------------------------

function display_all() {
    squash_arab_eafrica();
    if (version == 0) {
        fix_italy();
    }
    blur_minors();
    make_sources();

    build_tree();

    display_legend(ancestry_tree, d3.select('#htmllegend'), null, 0, 1);
    display_chart();

    calculate_map_data();
    display_details();
    configure_map_dom();
    draw_map();
}

function squash_arab_eafrica() {
    // Some old data has separate "ARABIAN" and "NEAREAST".  Squash that.
    // Also, the division of Kenya is rather suspicious.  Squash that too.
    var squashed_data = [];
    var arab_percentage = 0;
    var eafrica_percentage = 0;
    for (var i in raw_data) {
        if (raw_data[i].type=='ARABIAN' ||
            raw_data[i].type=='NEAREAST' ||
            raw_data[i].type=='AMBIG_NEAREAST') {
            arab_percentage += raw_data[i].percentage;
        } else if (raw_data[i].type=='BANTUKENYA' ||
                   raw_data[i].type=='EAFRICA') {
            eafrica_percentage += raw_data[i].percentage;
        } else {
            squashed_data.push(raw_data[i]);
        }
    }
    if (arab_percentage > 0) {
        squashed_data.push({'percentage':arab_percentage, 'type':'NEAREAST'});
    }
    if (eafrica_percentage > 0) {
        squashed_data.push({'percentage':eafrica_percentage, 'type':'EAFRICA'});
    }
    raw_data = squashed_data;
}

function blur_minors() {
    // Make anything <1% just disappear (no-op for version >0)
    // Then normalize to summ to 100% (might be needed anyway, with rounding errors)
    var squashed_data = [];
    var total_percentage = 0;
    for (var i in raw_data) {
        if (raw_data[i].percentage>1) {
            squashed_data.push(raw_data[i]);
            total_percentage += raw_data[i].percentage;
        }
    }
    for (var i in squashed_data) {
         squashed_data[i].percentage *= 100/total_percentage;
    }
    raw_data = squashed_data;
}

// Version 0 had a single ITALY-BALKANS category.  If we're using v0 data, consolidate those source lists.
function fix_italy() {
    pop_sources["ITALY-BALKANS"] = [];
    pop_sources["ITALY"].forEach(function(source) { pop_sources["ITALY-BALKANS"].push(source); });
    pop_sources["SBALKANS"].forEach(function(source) { pop_sources["ITALY-BALKANS"].push(source); });
    delete pop_sources["ITALY"];
    delete pop_sources["SBALKANS"];
    Clusters.push(['ITALY-BALKANS', 'ITALY-BALKANS', 'WEURASIA']);
}


function make_sources() {
    var sources_wanted={};
    for (var group in pop_sources) {
        for (var i in pop_sources[group]) {
            sources_wanted[pop_sources[group][i]] = group;
        }
    }
    for (var i in allsources) {
        if (allsources[i][1] in sources_wanted) {
            allsources[i][0] = allsources[i][1];
        }
        if (allsources[i][0] in sources_wanted) {
            allsources[i].push(sources_wanted[allsources[i][0]]);
            sources.push(allsources[i]);
        }
    }
}

function andify(list, punctuation) {
    if (!punctuation) punctuation = ',';
    if (list.length == 0) {
        return '';
    } else if (list.length == 1) {
        return list[0];
    } else if (list.length == 2) {
        return list[0] + ' and ' + list[1];
    } else {
        var last = list.pop();
        var out = list.join(punctuation+' ') + ' and ' + last;
        list.push(last); // leave our input the way we found it, since js doesn't copy
        return out;
    }
}

// Convert a list of places into a reasonably compact text string, fixing ugly names.
// The data that goes into this lives in popsources.js.  Hand-cleaning that without breaking
// anything would be very difficult, and likely cause problems for the next reference panel,
// so instead we clean it up here.
function list_to_text(l) {
    var places = {};
    var geneticists = ["Ayodo", "Comas", "Coriell", "ECCAC", "Henn", "Metspalu"];
    var subplaces = ["Adana", "Aydin", "Balikesir", "Cochabamba", "East", "Istanbul", "Kayseri", "Kinyawa_MKK", "LaPaz", "NChina", "Pando", "Pomiri", "South", "Trabzon", "Kenya_LWK"];
    var subtypes = ["Argyll", "Bergamo", "Chaplin", "EastSicilian", "French", "Megrels", "Naukan", "Reindeer", "Sir", "Sireniki", "Spanish", "Tuscan", "WestSicilian"];
    for (var i in l) {
        var place='', subplace='', pop='';
        if (l[i][1].indexOf('(')!=-1 && l[i][1].indexOf('expat')==-1) {
            var tmp = l[i][1].split('(');
            place=tmp[0];
            subplace=tmp[1];
            subplace=subplace.split(')')[0];
        } else if (l[i][1].indexOf(', ')!=-1) {
            var tmp = l[i][1].split(', ');
            place=tmp[0];
            subplace=tmp[1];
        } else {
            place = l[i][1];
        }
        var usidx = l[i][0].indexOf('_');
        if (usidx==-1) {
            pop = l[i][0];
        } else if (l[i][0].indexOf('Eskimo_')!=-1) {
            // Even if the Chaplin, Naukan and Sireniki don't consider "Eskimo" an insult, the Inuit
            // do and with no actual Inuit samples Inuit users might get classified here.
            pop = l[i][0].replace('Eskimo_','');
        } else {
            var modifier = l[i][0].substring(usidx+1);
            if (geneticists.indexOf(modifier)!=-1) {
                pop = l[i][0].substring(0,usidx);
            } else if (subplaces.indexOf(modifier)!=-1) {
                pop = l[i][0].substring(0,usidx);
                subplace = modifier;
            } else if (subtypes.indexOf(modifier)!=-1) {
                pop = l[i][0].replace('_','/','g');
            } else {
                pop = l[i][0].replace('_',' ','g');
            }
        }
        if (place=='Italia') place='Italy'; // The *only* translated placename in our data
        if (subplace=='Kenya_LWK') subplace='Webuye';
        subplace = subplace.replace(/_[A-Z][A-Z][A-Z]$/, '');
        place = place.replace(/([a-z])([A-Z])/g,'$1 $2').replace('_',' ');
        if (pop=='BantuKenya') pop='Bantu';
        pop=pop.replace('Pygmy',' (Pygmy)');
        if (!(place in places)) {
            places[place] = {'pops':[], 'subplaces':[], 'cnt':0};
        }
        if (places[place].pops.indexOf(pop)==-1) places[place].pops.push(pop);
        if (subplace && places[place].subplaces.indexOf(subplace)==-1) places[place].subplaces.push(subplace);
        places[place].cnt++;
    }
    var outlist = [];
    for (var place in places) {
        var out = '';
        out += andify(places[place].pops);
        if (place.indexOf('expat')!=-1) {
            out += ' from ';
        } else {
            out += ' in ';
        }
        if (places[place].subplaces.length > 0) {
            if (places[place].cnt > places[place].subplaces.length) {
                var extra = places[place].cnt - places[place].subplaces.length;
                places[place].subplaces.push(extra + ' other site' + (extra>1 ? 's' : ''));
            }
            out += '(' + andify(places[place].subplaces) + ') ';
        } else if (places[place].cnt > places[place].pops.length) {
            out += '(' + places[place].cnt + ' sites) ';
        }
        out += place;
        outlist.push(out);
    }
    return andify(outlist,';');
}


function order(tree, name) {
    var out = {'percentage': tree.percentage,
               'name': name,
               'rows': 0,
               'children': []};
    for (var child in tree.children) {
        var flatchild = order(tree.children[child], child);
        out.children.push(flatchild);
        out.rows += flatchild.rows;
    }
    if (out.rows == 0) {
        out.rows = 1;
    }
    out.children.sort( function(a,b) { return b.percentage - a.percentage; } );
    return out;
}


function sigfig(v, digits_wanted) {
    var digits_had = Math.ceil(Math.log(v)/Math.log(10));
    var adj = Math.pow(10, digits_had - digits_wanted);
    var out = Math.round(v/adj)*adj;
    // out would be the value we want if not for floating point issues
    if (digits_had >= digits_wanted) {
        return out;
    } else {
        return (out+'').substring(0, digits_wanted-digits_had+2);
    }
}

function get_pops(group, level) {
    var out = [];
    for (var i in Clusters) {
        if (Clusters[i][2-level]==group) {
            out.push(Clusters[i][0]);
        }
    }
    return out;
}

function dbg(x) {
    d3.select('body').append('p').text(JSON.stringify(x));
}


// -----------------------------------------------------------------------------
// -- Ancestry Pie Chart and Tree
// -----------------------------------------------------------------------------

// Define ancestry chart
var ac = d3.select("#ancestry-chart")
    .append("svg:svg")
    .attr("height", _AC_HEIGHT)
    .attr("width", _AC_WIDTH)
    .append("svg:g")
    .attr("transform", "translate(" + _AC_WIDTH / 2 + "," + _AC_HEIGHT / 2 + ")")
    .attr("id", "ac-c");

var ancestry_tree = {};


function assign_hues(tree, from, to, depth) {
    if (tree.name != 'Ambiguous'){
        var hue = (from+to)/2;
        var lum_adj = -.2 * Math.exp(-Math.pow((hue-80)/40, 2)); // darken the yellows for readability
        var color = d3.hsl(hue, .4*(depth-.5), .6+lum_adj);
        ac_colors[tree.name] = color;
    }
    var childcount = tree.children.length;
    tree.children.forEach(function(child) { if (child.name=='Ambiguous') childcount--; } );
    var dh = (to-from) / (childcount*2-1);
    for (var i in tree.children) {
        assign_hues(tree.children[i], from+dh*i*2, from+dh*(i*2+1), depth+1);
    }
}

function display_legend(tree, table, tr, depth, colspan, parent) {
    if (!tr) {
        tr = table.append('tr');
    }
    if (tree.name == 'All') {
    } else if (tree.children.length==1 &&
               (tree.children[0].name!='Ambiguous' ||
                tree.name=='Ambiguous')) {
        colspan+=1;
    } else {
        var popname = (display_names[tree.name] || tree.name);
        var td = tr.append('td')
            .attr('rowspan', tree.rows)
            .attr('colspan', colspan)
            .attr('class', 'legend');
        var textbox;
        if (depth==3 && (colspan!=3 || tree.name!='Ambiguous')) {
            textbox = td.append('a')
                .style('display','inline')
                .style('color', 'inherit');
            if (tree.name!='Ambiguous') {
                //textbox.attr('href','#'+popname.replace(/[ \/]/g,'_'));
                textbox.on('click', function(){ click_on_map({type:'pop', pop:tree.name});
                                                d3.event.stopPropagation(); });
                map_data.group_td[tree.name]=td;
            } else {
                var ppopname = (display_names[parent.name] || parent.name);
                //textbox.attr('href','#'+ppopname.replace(/[ \/]/g,'_'));
                textbox.on('click', function(){ click_on_map({type:'pop', pop:parent.name});
                                                d3.event.stopPropagation(); });
                map_data.group_td[parent.name]=td;
            }
        } else {
            textbox = td;
        }
        textbox.text(popname + ' ' + sigfig(tree.percentage, 2) + '%');
        if (tree.name != 'Ambiguous') {
            td.style('background', ac_colors[tree.name]);
        } else {
            td.style('background', ac_colors[parent.name]);
        }
        if (depth==3) {
            td.classed('last', true);
        }
        colspan=1;
    }
    for (var i in tree.children) {
        if (i!=0) {
            tr = table.append('tr');
        }
        display_legend(tree.children[i], table, tr, depth+1, colspan, ((colspan>1)? parent : tree));
    }
    if (depth==0) {
        d3.select(table.node().parentNode).on('click', function(){
            click_on_map({type:'nothing'});
        });
    }
}

function build_tree() {
    for (var i in raw_data) {
        var t = raw_data[i].type;
        var levels = null;
        if (t=='AMBIGUOUS') {
            levels = ['Ambiguous', 'Ambiguous', 'Ambiguous'];
        } else if (t.substr(0,5)=='AMBIG') {
            var cat = t.substr(6);
            for (var j in Clusters) {
                if (cat==Clusters[j][1]) {
                    levels = ['Ambiguous', cat, Clusters[j][2]];
                    show_on_map[1][cat] = raw_data[i].percentage;
                    break;
                } else if (cat==Clusters[j][2]) {
                    levels = ['Ambiguous', 'Ambiguous', cat];
                    show_on_map[0][cat] = raw_data[i].percentage;
                    break;
                }
            }
        } else {
            for (var j in Clusters) {
                if (t==Clusters[j][0]) {
                    levels = Clusters[j];
                    show_on_map[2][t] = raw_data[i].percentage;
                    break;
                }
            }
        }
        if (!levels) {
            levels = [t, 'UNKNOWN', 'UNKNOWN'];
        }
        var cur_tree = ancestry_tree;
        for (var l = 2; l>=0; l--) {
            if (! (levels[l] in cur_tree)) {
                cur_tree[levels[l]] = {'percentage': 0, 'children':{}};
            }
            cur_tree[levels[l]].percentage += raw_data[i].percentage;
            cur_tree = cur_tree[levels[l]].children;
        }
    }
    ancestry_tree = order({'children':ancestry_tree}, 'All');
    assign_hues(ancestry_tree, 0, 255, 0);
}


// Ancestry Chart - Display
function display_chart() {
    ac.data([ancestry_tree]).selectAll("path")
        .data(
            d3.layout.partition()
                .size([Math.PI*2, _AC_RADIUS*_AC_RADIUS])
                .value(function(v) { return v.percentage; })
            .nodes)
        .enter().append("svg:path")
        .attr("d", d3.svg.arc()
            .startAngle(function(v)  { return v.x; })
            .endAngle(function(v)    { return v.x + v.dx; })
            .innerRadius(function(v) { return Math.sqrt(v.y)-30; })
            .outerRadius(function(v) { return Math.sqrt(v.y + v.dy); })
        )
        // Don't show center
        .attr("display", function(v) { return v.depth ? null : "none"; })
        // Set color of each part of chart
        .style("fill", function(v) { return ac_colors[v.name]; })
        .style("cursor", function(v) { return v.depth==3 ? "pointer" : "default"; })
        // Define mouse over
        .on("mouseover", ac_mouseover)
        .on("click",
            function(d) {
                if (d.depth==3) {
                    if (d.name in show_on_map[2]) {
                        click_on_map({type: "pop", pop: d.name});
                    }
                    if (d.name=='Ambiguous' && d.parent.name in show_on_map[1]) {
                        click_on_map({type: "pop", pop: d.parent.name});
                    }
                    if (d.name=='Ambiguous' && d.parent.name=='Ambiguous' &&
                        d.parent.parent.name in show_on_map[0]) {
                        click_on_map({type: "pop", pop: d.parent.parent.name});
                    }
                    d3.event.stopPropagation();
                }
            })
        .each(
            function(d) {
                if (d.depth==3) {
                    var n;
                    for (n=d; n.name=='Ambiguous'; n=n.parent);
                    pop_wedge[n.name] = this;
                }
            });

    // Define on mouse out event
    d3.select("#ac-c").on("mouseleave", ac_mouseleave);
}

// Ancestry Chart - On mouse over, highlight
function ac_mouseover(path)
{
    // Set opacity to 0.5 only to paths not being hovered
    ac.selectAll("path").each(function(ancestry)
    {
        if(ancestry.type == path.type)
            d3.select(this).style("opacity", 1)
        else
            d3.select(this).style("opacity", _AC_OPACITY_ON_HOVER)
    })

    // Show % ancestry + ancestry type
    tmpStr = sigfig(path.value, 2) + "%"
    d3.select("#ac-hover-txt").style("visibility", "")
    d3.select("#ac-hover-txt").html((display_names[path.name] || path.name) + ": " + tmpStr)
}

// Ancestry Chart - On mouse out, set everything back to opacity 1
function ac_mouseleave(path)
{
    // Hide text that shows ancestry name + %
    d3.select("#ac-hover-txt").style("visibility", "hidden");

    // Set all paths to opacity 1
    d3.selectAll("path").style("opacity", 1)
}

// -----------------------------------------------------------------------------/
// -- Ancestry Map
// -- Note: here height is set to width, and then svg is cropped
// -----------------------------------------------------------------------------

function lightness(frac, hue) {
    var minl=.2, maxl=.9;
    frac /= map_data.max_frac;
//    frac = Math.pow(frac, .8);
    var l = minl*frac + maxl*(1-frac);
    // darken the yellows and cyans for evenness
    l -= .05 * Math.exp(-Math.pow((hue-60)/20, 2));
    l -= .05 * Math.exp(-Math.pow((hue-180)/20, 2));
    if (l<minl) l=minl;
    // likewise, lighten the reds and blues
    l += .2 * Math.exp(-Math.pow((hue-0)/30, 2)) * (1-l);
    l += .4 * Math.exp(-Math.pow((hue-240)/30, 2)) * Math.pow(1-l, .9);
    if (l>maxl) l=maxl;
    return l;
}

function check(c, col, linew, size, x, y) {
    c.fillStyle=col.toString();
    c.strokeStyle='white';
    c.lineWidth = linew;
    c.beginPath();
    c.moveTo(x-size/2, y-size/2);
    c.lineTo(x, y-size/4);
    c.lineTo(x+size, y-size);
    c.lineTo(x, y+size/4);
    c.lineTo(x-size/2, y-size/2);
    c.fill();
    c.stroke();
}

function qmark(c, col, linew, size, x, y) {
    c.textAlign='center';
    c.textBaseline='middle';
    c.fillStyle=col.toString();
    c.strokeStyle='white';
    c.lineWidth = linew;
    c.beginPath();
    c.font=Math.round(1.5*size)+'px bold';
    c.strokeText('?', x, y);
    c.fillText('?', x, y);
}

function nilmark(c, linew, size, x, y) {
    // circle
    c.strokeStyle='white';
    c.lineWidth = linew;
    c.fillStyle = d3.hsl(0, 0, 0.3).toString();
    c.beginPath();
    c.arc(x, y, size/2, 0, 2*Math.PI);
    c.moveTo(x+size/4, y);
    c.arc(x, y, size/4, 0, 2*Math.PI, true);
    c.fill();
    c.stroke();
    // slash
    c.beginPath();
    c.strokeStyle=c.fillStyle;
    c.lineWidth = 3*size/16;
    c.moveTo(x-size/4, y+3*size/4);
    c.lineTo(x+size/4, y-3*size/4);
    c.stroke();
}

function collapsable_draw_map() {
    if (map_data.draw_pending) {
        clearTimeout(map_data.draw_pending);
    }
    map_data.draw_pending=setTimeout(draw_map, 50);
}

function draw_map() {
    var starttime = [(new Date()).getTime()];
    var profiling = ['start',0];
    var c = map_data.canvas.node().getContext("2d");
    var path = d3.geo.path()
        .projection(map_data.projection)
        .context(c);
    var pts2d = map_data.pts2d;
    var cells = map_data.cells;
    var links = map_data.links;
    var source_colors = map_data.source_colors[map_data.currentMode];


    c.setTransform(1, 0, 0, 1, 0, 0);
    c.lineWidth=1;
    c.clearRect(0, 0, _AM_WIDTH, _AM_HEIGHT);

    var trans = map_data.zoom.translate();
    if (trans[0] > 0) trans[0] = 0;
    if (trans[1] > 0) trans[1] = 0;
    var scale = map_data.zoom.scale();
    var s = scale - 1;
    if (trans[0] < -_AM_WIDTH*s) trans[0] = -_AM_WIDTH*s;
    if (trans[1] < -_AM_HEIGHT*s) trans[1] = -_AM_HEIGHT*s;
    map_data.zoom.translate(trans);

    c.translate(trans[0], trans[1]);
    c.scale(scale, scale)

    profiling.push('setup=')
    profiling.push((new Date()).getTime() - starttime);

    // Bottom layer: the voronoi cells
    for (var i in cells) {
        var source = sources[i][0];
        if (source in source_colors) {
            var source_color;
            if (map_data.source_groups[source] == map_data.highlighted_group) {
                c.fillStyle='yellow';
                c.strokeStyle='yellow';
            } else {
                c.fillStyle=source_colors[source].toString();
                c.strokeStyle=source_colors[source].toString();
            }
            c.beginPath();
            c.moveTo(cells[i][0][0], cells[i][0][1]);
            for (var j=1; j<cells[i].length; j++) {
                c.lineTo(cells[i][j][0], cells[i][j][1]);
            }
            c.fill();
            c.stroke();
        }
    }

    profiling.push('voronoi=');
    profiling.push((new Date()).getTime() - starttime);

    // Slightly higher: nongeo hashings
    for (var pop in map_data.show_nongeo) {
        for (var i in sources) {
            if (sources[i][5]!=pop) continue;
            var source_color;
            if (map_data.source_groups[sources[i][0]] == map_data.highlighted_group) {
                source_color = 'yellow';
            } else {
                source_color = source_colors[sources[i][0]];
            }
            var rgb = d3.rgb(source_color);
            c.fillStyle='rgba('+rgb.r+','+rgb.g+','+rgb.b+',0.1)';

            var radius = nongeo_radii[pop];
            for (var rat=1; rat<2; rat+=.1) {
                c.beginPath();
                c.arc(pts2d[i][0], pts2d[i][1], radius/rat, 0, 1.99*Math.PI, false);
                c.fill();
            }
        }
    }

    profiling.push('nongeo=');
    profiling.push((new Date()).getTime() - starttime);

    // Middle layer: the ocean
    c.fillStyle = '#56f';
    c.strokeStyle = 'black';
    c.beginPath();
    path(map_data.water);
    c.fill();
    c.stroke();

    profiling.push('ocean=');
    profiling.push((new Date()).getTime() - starttime);

    // Top layer: the source dots
    var size = 8;
    var linew = 1;
    size /= scale;
    linew /=scale;
    for (var i in sources) {
        if (sources[i][0] in source_colors) {
            var col = d3.hsl(source_colors[sources[i][0]]);
            col.l=.3;
            if (sources[i][5] in show_on_map[2]) {
                check(c, col, linew, size, pts2d[i][0], pts2d[i][1]);
            } else {
                qmark(c, col, linew, size, pts2d[i][0], pts2d[i][1]);
            }
        } else {
            nilmark(c, linew, size, pts2d[i][0], pts2d[i][1]);
        }
    }

    profiling.push('sources=');
    profiling.push((new Date()).getTime() - starttime);

    // The population "scale"
    c.beginPath();
    c.setTransform(1, 0, 0, 1, 0, 0);
    c.strokeStyle='black';
    c.fillStyle='white';
    c.lineWidth=2;
    var w = 200;
    var h = (map_data.currentMode == COLOR) ? 20 : 60;
    c.rect(_AM_WIDTH - w, _AM_HEIGHT - h, w, h);
    c.fill();
    c.stroke();
    var has_ambig = (Object.keys(show_on_map[0]).length || Object.keys(show_on_map[1]).length);

    check(c, 'black', 1, 8, _AM_WIDTH-w+(has_ambig?10:30), _AM_HEIGHT-h+12);
    c.font='12px normal';
    c.textAlign='left';
    c.textBaseline='middle';
    c.fillStyle='black';
    c.fillText('= match', _AM_WIDTH-w+(has_ambig?18:38), _AM_HEIGHT-h+10);

    if (has_ambig) {
        qmark(c, 'black', 1, 8, _AM_WIDTH-2*w/3+5, _AM_HEIGHT-h+11);
        c.font='12px normal';
        c.textAlign='left';
        c.textBaseline='middle';
        c.fillText('= ambig', _AM_WIDTH-2*w/3+11, _AM_HEIGHT-h+10);
    }

    nilmark(c, 1, 8, _AM_WIDTH-w/(has_ambig?3:2), _AM_HEIGHT-h+10);
    c.fillText('= nonmatch', _AM_WIDTH-w/(has_ambig?3:2)+8, _AM_HEIGHT-h+10);

    if (map_data.currentMode!=COLOR) {
        var barleft = _AM_WIDTH - w + 15;
        var barright = _AM_WIDTH - 15;
        var bartop = _AM_HEIGHT - h + 20;
        var barbottom = _AM_HEIGHT - 20;
        if (!map_data.cached_bar[map_data.currentMode]) {
            var barwidth = barright - barleft;
            var barheight = barbottom - bartop;
            map_data.cached_bar[map_data.currentMode] = document.createElement('canvas');
            map_data.cached_bar[map_data.currentMode].width = barwidth;
            map_data.cached_bar[map_data.currentMode].height = barheight;
            var c2 = map_data.cached_bar[map_data.currentMode].getContext('2d');
            for (var x=0; x<barwidth; x++) {
                var frac = map_data.max_frac * x / barwidth;
                for (var y=0; y<barheight; y++) {
                    var hue = 255 * y / barheight;
                    if (map_data.currentMode==HYBRID) {
                        var l = lightness(frac, hue);
                        c2.fillStyle = d3.hsl(hue, 0.3/*+frac/2*/, l).toString();
                    } else {
                        var l = lightness(frac, 0);
                        c2.fillStyle = d3.hsl(0, 0, l).toString();
                    }
                    c2.fillRect(x, y, 1, 1);
                }
            }
        }
        c.drawImage(map_data.cached_bar[map_data.currentMode], barleft, bartop);
        c.textAlign='center';
        c.textBaseline='top';
        for (var i=0; i<5; i++) {
            var x = i*(barright-barleft)/4 + barleft;
            var frac = Math.round(100*i*map_data.max_frac/4);
            var txt = (frac==0) ? '1%' : (frac+'%');
            c.fillStyle='black';
            c.fillText(txt, x, _AM_HEIGHT - 15);
        }
    }

    profiling.push('legend=');
    profiling.push((new Date()).getTime() - starttime);

    for (var i=profiling.length-1; i>1; i-=2) {
        profiling[i] -= profiling[i-2];
    }
    //dbg(profiling);
}

function configure_map_dom() {
    map_data.canvas
        .attr("width", _AM_WIDTH)
        .attr("height", _AM_HEIGHT)
        .on('click', click_on_map)
        .on('mousemove', move_over_map);

    d3.select("#ancestry-map")
        .style('display', 'inline-block')
        .style('width', _AM_WIDTH+'px');

    var map_det = d3.select('#map-details')
        .style('display', 'inline-block')
        .style('border', 'thin solid grey')
        .style('border-radius', '3px')
        .style('padding', '5px')
        .style('overflow-y', 'scroll');

    window.onresize=function() {
        var width = map_det.node().parentElement.clientWidth - 30; // 30 is the padding
        if (width > _AM_WIDTH + 200) {
            map_det
                .style('height', _AM_HEIGHT+'px')
                .style('width', width - _AM_WIDTH - 3 + 'px');
        } else {
            map_det
                .style('height', (_AM_HEIGHT/2)+'px')
                .style('width', _AM_WIDTH+'px');
        }
    };
    window.onresize();

    d3.select('#map-zoom')
        .style('position', 'absolute')
        .style('top', _AM_HEIGHT+20+'px')
        .style('left', '25px');

    d3.selectAll('#zoom-in, #zoom-out')
        .style('width', '25px')
        .style('height', '25px')
        .style('padding', '0px');

    if (d3.select('#zoom-in').style('padding-left') != '0px') {
        // We just set it, so this must be webkit's nonoveridable appearances
        d3.selectAll('#zoom-in, #zoom-out')
            .style('-webkit-appearance', 'none')
            .style('background', 'linear-gradient(white,#99d,#99d,#99d)')
            .style('padding', '0px')
            .style('border-radius', '10px');
    }

    d3.select('#zoom-in')
        .on('click', function() {
            map_data.zoom.scaleBy(2);
            draw_map();
        });
    d3.select('#zoom-out')
        .on('click', function() {
            map_data.zoom.scaleBy(0.5);
            draw_map();
        });

    map_data.tabs = [];
    map_data.tabs[COLOR] = d3.select('#colortab').on('click', function(){ changeTab(COLOR); });
    map_data.tabs[HYBRID] = d3.select('#hybridtab').on('click', function(){ changeTab(HYBRID); });
    map_data.tabs[GREY] = d3.select('#greytab').on('click', function(){ changeTab(GREY); });

    var totw = 0;
    for (var tabi in map_data.tabs) {
        totw += map_data.tabs[tabi].node().offsetWidth;
    }
    if (totw > _AM_WIDTH) {
        d3.select('#tabholder').style('font-size', Math.round(_AM_WIDTH/100)+'pt');
    }
}

function changeTab(newtab) {
    map_data.tabs[map_data.currentMode].classed('current', false);
    map_data.currentMode = newtab;
    map_data.tabs[map_data.currentMode].classed('current', true);
    draw_map();
}

function pointing_at() {
    var xy = d3.mouse(map_data.canvas.node());
    var xyl = map_data.zoom.log_from_phys(xy);
    for (var i in map_data.pts2d) {
        if ((Math.pow(xyl[0]-map_data.pts2d[i][0],2) +
             Math.pow(xyl[1]-map_data.pts2d[i][1],2)) < Math.pow(10/map_data.zoom.scale(),2)) {
            return({'type':'source', 'source':sources[i]});
        }
    }
    for (var i in map_data.pts2d) {
        if (sources[i][5] in nongeo_radii) {
            rad = nongeo_radii[sources[i][5]];
            if ((Math.pow(xyl[0]-map_data.pts2d[i][0],2) +
                 Math.pow(xyl[1]-map_data.pts2d[i][1],2)) < Math.pow(rad, 2)) {
                if (sources[i][0] in map_data.source_groups) {
                    return ({'type':'pop', 'pop':map_data.source_groups[sources[i][0]]});
                }
            }
        }
    }
    var data = (map_data.c.getImageData(xy[0], xy[1], 1, 1).data);
    var color = d3.hsl(d3.rgb(data[0], data[1], data[2]));
    if (color in map_data.color_groups[map_data.currentMode]) {
        return({'type':'pop', 'pop':map_data.color_groups[map_data.currentMode][color]});
    }
    if (data[0]==255 && data[1]==255 && data[2]==0) {
        return({'type':'pop', 'pop':map_data.highlighted_group});
    }
    return({'type':'nothing'});
}

function click_on_map(at) {
    if (d3.event.defaultPrevented) return;
    var det = d3.select('#map-details');
    var group_to_hl = null;
    det.selectAll('*').remove();
    if (!at) {
        at = pointing_at();
    }
    if (at.type=='source') {
        sourcename = list_to_text([[at.source[1], at.source[2]]])
        det.append('h5').text(sourcename);
        det.append('br');
        pop = at.source[5];
        for (var i in Clusters) {
            if (Clusters[i][0] == pop) {
                var txt='';
                var found=false;
                var opn='';
                var pn0='';
                for (var j in Clusters[i]) {
                    var p = Clusters[i][j];
                    var pn = display_names[p] || p;
                    if (j==0) pn0 = pn;
                    if (pn!=opn) {
                        det.append('div').text(pn);
                    }
                    if (!found) {
                        if (p in show_on_map[2-j]) {
                            txt += (j==0) ? 'We found ' : 'but we did find ';
                            txt += sigfig(show_on_map[2-j][p],2) + '%';
                            if (j>0) txt += ' ambiguous';
                            txt += ' ' + pn;
                            if (pn==opn) txt+= ' (the general category)'
                            txt += ' ancestry';
                            if (j>0) txt += ', which could be ' + pn0;
                            if (pn==opn) txt+= ' (the specific population)'
                            txt += '.';
                            found=true;
                            group_to_hl = p;
                        } else if (pn!=opn) {
                            txt += (j==0) ? 'We found no specific ' : ' nor ';
                            txt += pn + ' ancestry, ';
                        }
                    }
                    opn=pn;
                }
                det.append('br');
                det.append('div').text(found ? txt : 'No match');
            }
        }
    }
    if (at.type=='pop') {
        if (at.pop in inctext) {
            var dn = (display_names[at.pop] || at.pop);
            det.append('h5').text(dn + ' [' + sigfig(show_on_map[2][at.pop],2) + '%]');
            det.append('br');
            det.append('b').text('Includes: ');
            det.append('span').text(inctext[at.pop]);
            det.append('br');
            det.append('b').text('Does not include: ');
            det.append('span').text(dnitext[at.pop]);
            if (at.pop in nongeo_radii) {
                det.append('br');
                det.append('br');
                det.append('p').text('The '+dn+' are displayed specially because they ' +
                                     'do not have a territory of their own, but live amidst ' +
                                     'unrelated peoples.');
            }
        } else {
            var perc = show_on_map[1][at.pop] || show_on_map[0][at.pop];
            det.append('h5').text('Ambiguous ' + (display_names[at.pop] || at.pop) + ' [' + sigfig(perc,2) + '%]');
            det.append('br');
            det.append('p').text(hltext[at.pop]);
        }
        group_to_hl = at.pop;
    }
    if (at.type=='nothing') {
        group_to_hl = null;
    }
    if (map_data.highlighted_group != group_to_hl) {
        var td = map_data.group_td[map_data.highlighted_group];
        if (td) td.classed('hlx', false);
        var w = pop_wedge[map_data.highlighted_group];
        if (w) d3.select(w).classed('hlx', false);
        map_data.highlighted_group=group_to_hl;
        if (group_to_hl) {
            map_data.group_td[group_to_hl].classed('hlx', true);
            d3.select(pop_wedge[group_to_hl]).classed('hlx', true);
        }
        draw_map();
    }
}

function move_over_map() {
    var at = pointing_at();
    if (at.type=='nothing') {
        map_data.canvas.style('cursor', 'default');
    } else {
        map_data.canvas.style('cursor', 'pointer');
    }
}

function calculate_map_data()
{
    var available_width = d3.select("#ancestry-map").node().parentElement.parentElement.clientWidth - 36;

    if (_AM_WIDTH > available_width) {
        var newheight = Math.round(_AM_HEIGHT * (available_width / _AM_WIDTH));
        _AM_WIDTH = available_width;
        _AM_V_TRANSLATION *= (newheight / _AM_HEIGHT);
        _AM_HEIGHT = newheight;
    }

    map_data.currentMode=HYBRID;

    // Define d3 objects
    map_data.projection = d3.geo.equirectangular()
        .scale((_AM_WIDTH + 1) / 2 / Math.PI)
        .translate([_AM_WIDTH / 2, _AM_V_TRANSLATION])
        .precision(.1);
    map_data.canvas = d3.select("#ancestry-map").append("canvas")
    map_data.c = map_data.canvas.node().getContext("2d");

    // First convert all the source locations to pixel co-ordinates
    var pts2d=[];
    map_data.pts2d = pts2d;
    var nongeo_fix=[]
    var show_nongeo = {};
    map_data.show_nongeo = show_nongeo;
    for (var i in sources) {
        if (!nongeo_radii[sources[i][5]]) {
            // We appear to be swapping these, but it works
            // maybe something else is also backwards?
            var lon = sources[i][3];
            var lat = sources[i][4];
            var xy = map_data.projection([lat,lon]);
            pts2d.push(xy);
        } else {
            nongeo_fix.push(i);
            pts2d.push([-1000, -1000]); // Don't let them effect the voronoi cells
        }
    }

    map_data.cells = d3.geom.voronoi(pts2d);
    map_data.links=d3.geom.voronoi().links(pts2d);

    // Voronoi is done, handle the non-geographic populations
    for (var j in nongeo_fix) {
        var i = nongeo_fix[j];
        var lon = sources[i][3];
        var lat = sources[i][4];
        var xy = map_data.projection([lat,lon]);
        pts2d[i]=xy;
        // And handle links
        for (var k in pts2d) {
            if (sources[i][5] == sources[k][5]) continue;
            var radius = nongeo_radii[sources[i][5]];
            if ((Math.pow(pts2d[i][0]-pts2d[k][0],2) +
                 Math.pow(pts2d[i][1]-pts2d[k][1],2)) <
                Math.pow(2*radius, 2)) {
                map_data.links.push({source: pts2d[i], target: pts2d[k]});
            }
        }
    }

    for (var i in pts2d) {
        if (sources[i][1] == 'FAKE') {
            pts2d[i]=[-10000, -10000];
        }
    }

    // Find max component
    map_data.max_frac = 0;
    for (var i in show_on_map) {
        for (var group in show_on_map[i]) {
            if (show_on_map[i][group]/100 > map_data.max_frac) {
                map_data.max_frac = show_on_map[i][group]/100;
            }
        }
    }

    // Then pick colors by sources
    map_data.source_colors = [{}, {}, {}];
    map_data.color_groups = [{}, {}, {}];
    map_data.source_groups = {};
    for (var level in show_on_map) {
        for (var group in show_on_map[level]) {
            var p = show_on_map[level][group] / 100;
            p /= Math.pow(2, 2-level);
            var color = ac_colors[group];
            if (true) {
                var l = lightness(p, color.h);
                var newcolor = d3.hsl(color.h, .3/*+p/2*/, l);
                while (newcolor in map_data.color_groups) {
                    newcolor.h = (newcolor.h + 5) % 256;
                }
            } else {
                var l = lightness(p, 0);
                var newcolor = d3.hsl(color.h, 0, l);
            }
            var grey = d3.hsl(0, 0, lightness(p, 0));
            map_data.color_groups[COLOR][color] = group;
            map_data.color_groups[HYBRID][newcolor] = group;
            map_data.color_groups[GREY][grey] = group;
            var pops = get_pops(group,level);
            for (var popi in pops) {
                var pop = pops[popi];
                if (nongeo_radii[pop]) {
                    show_nongeo[pop]=true;
                }
                if (!(pop in pop_sources)) continue;
                for (var j in pop_sources[pop]) {
                    var source = pop_sources[pop][j];
                    map_data.source_groups[source] = group;
                    map_data.source_colors[COLOR][source] = color;
                    map_data.source_colors[HYBRID][source] = newcolor;
                    map_data.source_colors[GREY][source] = grey;
                }
            }
        }
    }

    map_data.zoom = d3.behavior.zoom()
        .scaleExtent([1,8])
        .on("zoom", collapsable_draw_map);
    map_data.zoom(map_data.canvas);

    // Why doesn't d3 provide these?
    map_data.zoom.log_from_phys = function(l) {
        var d,s;
        d = this.translate();
        s = this.scale();
        return [ (l[0]-d[0])/s, (l[1]-d[1])/s ];
    };
    map_data.zoom.scaleBy = function(factor) {
        var s = this.scale();
        var ext = this.scaleExtent();
        s *= factor;
        if (s < ext[0]) s=ext[0];
        if (s > ext[1]) s=ext[1];
        this.scale(s);
    };


    var invpts = {}; // needed because voronoi().links returns points, not indices
    for (var i in pts2d) {
        invpts[pts2d[i]] = i;
    }
    for (var pop in show_on_map[2]) {
        var pops_to_list = [];
        for (var i in sources) {
            for (var j in pop_sources[pop]) {
                if (sources[i][0] == pop_sources[pop][j] && sources[i][1]!='FAKE') {
                    pops_to_list.push([sources[i][1], sources[i][2]]);
                }
            }
        }
        inctext[pop] = list_to_text(pops_to_list)
        var neighbors = {};
        for (var i in map_data.links) {
            var s = invpts[map_data.links[i].source];
            var t = invpts[map_data.links[i].target];
            if (!(s in sources) || !(t in sources)) {
                //                      dbg('Bad link: '+s+'->'+t);
                continue;
            }
            if (sources[s][5]==pop && sources[t][5]!=pop) {
                neighbors[t] = true;
            }
            if (sources[t][5]==pop && sources[s][5]!=pop) {
                neighbors[s] = true;
            }
        }
        pops_to_list = [];
        for (var i in neighbors) {
            pops_to_list.push([sources[i][1], sources[i][2]]);
        }
        dnitext[pop] = list_to_text(pops_to_list)
    }

    for (var layer=1; layer>=0; layer--) {
        for (var pop in show_on_map[layer]) {
            var popname = (display_names[pop] || pop);
            var txt='';
            txt += popname;
            txt += ' is a ';
            if (layer==0) txt += 'very ';
            txt += 'general category containing ';
            var subpops={};
            for (var i in Clusters) {
                if (Clusters[i][2-layer]==pop) {
                    var subpop = Clusters[i][(2-layer)-1];
                    subpops[ (display_names[subpop] || subpop) ] = true;
                }
            }
            txt += andify(Object.keys(subpops));
            hltext[pop] = txt;
        }
    }

    map_data.cached_bar = [null, null, null];

    map_data.water={};

    // Load world map and define water
    queue()
        .defer(d3.json, "/static/ancestry/data/world-110m.json")
        .await(function(error, world)
        {
            if (error)
                throw error;
            var land = topojson.feature(world, world.objects.land)
            map_data.water = { 'type': 'Polygon', 'coordinates':[] };
            for (var polygoni in land.geometry.coordinates) {
                for (var linringi in land.geometry.coordinates[polygoni]) {
                    var ring = land.geometry.coordinates[polygoni][linringi].reverse();
                    map_data.water.coordinates.push(ring);
                }
            }
            draw_map();
        });
}

function display_details() {
    var dets = d3.select('#details');
    var keylist = Object.keys(show_on_map[2]);
    keylist.sort(function(popa,popb){return ac_colors[popa].h - ac_colors[popb].h;});
    for (var i in keylist ) {
        var pop = keylist[i];
        var popname = (display_names[pop] || pop);
        var h5 = dets.append('h5').attr('id',popname.replace(/[ \/]/g,'_'));
        h5.append('span')
            .style('background', ac_colors[pop])
            .attr('class', 'colorbox')
            .text(' ');
        h5.append('span').text(popname);
        dets.append('b').text('Includes: ');
        dets.append('span').text(inctext[pop]);
        dets.append('br');
        dets.append('b').text('Does not include: ');
        dets.append('span').text(dnitext[pop]);
    }
    for (var layer=1; layer>=0; layer--) {
        for (var pop in show_on_map[layer]) {
            var popname = (display_names[pop] || pop);
            var h5 = dets.append('h5').attr('id',popname.replace(/[ \/]/g,'_'));
            h5.append('span')
                .attr('class', 'colorbox')
                .style('background', ac_colors[pop])
                .text(' ');
            h5.append('span').text('Ambiguous ' +popname);
            dets.append('p').text(hltext[pop]);
        }
    }

    window.addEventListener('hashchange',
                            function(){
                                d3.selectAll('h5').attr('class','none');
                                d3.select(location.hash).attr('class','highlighted');
                            },
                            false);
}
