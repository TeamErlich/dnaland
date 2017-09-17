require(['main'], function () {
    'use strict';

    require(['d3', 'underscore'], function (d3, _) {
        var $container = d3.select('.donut-container');

        function normalizeToPolar(percentage, min, max) {
            return Math.max(percentage * max, min)
        }

        function interpolateHsl(a, b) {
            var i = d3.interpolateString(a, b);
            return function (t) {
                return d3.hsl(i(t));
            };
        }

        var circle_data = [],
            point_data = 0,
            percentile_data = 0 ;

        // filter data
        _.each(window.USER.metrics, function (metric) {
            var key = 'metric_name', value = 'metric_value';

	    if (key in metric && value in metric) {
		if (metric[key] == 'points') {
		    point_data = metric['metric_value'];
		} else if (metric[key] == 'percentile') {
		    percentile_data = parseInt(metric['metric_value']);
		} else {
                    circle_data.push(metric);
		}
            }
        });

        // determine angle
        var data = _.each(circle_data, function (metric) {
            var key = 'metric_name', value = 'metric_value';

            if (key in metric && value in metric) {
                metric['endAngle'] = normalizeToPolar(metric[value] / 100, Math.PI / 10, 2 * Math.PI - Math.PI / 15);
            }
        });

        var back_data = _.each(_.clone(data, true), function (metric) {
            metric['endAngle'] = normalizeToPolar(1, Math.PI / 10, 2 * Math.PI);
        });

        var width = 240,
            height = 240;

        var normalized_point_data = Math.min((point_data / (100 * (data.length - 1))) * 100, 100);

        var color = d3.scale.linear()
            .range(["hsl(" + (normalized_point_data + 120) + ",50%,50%)", "hsl(" + (normalized_point_data + 140) + ",50%,65%)"])
            .interpolate(interpolateHsl);

        var arc = d3.svg.arc()
            .innerRadius(function (d, i) {
                return 50 + (25 * i);
            })
            .outerRadius(function (d, i) {
                return 70 + (25 * i)
            })
            .cornerRadius(15)
            .startAngle(0)
            .endAngle(function (d) {
                return d.endAngle
            });

        var arcOffset = d3.svg.arc()
            .innerRadius(function (d, i) {
                return 50 + (25 * i);
            })
            .outerRadius(function (d, i) {
                return 70 + (25 * i)
            })
            .cornerRadius(15)
            .startAngle(Math.PI / 15)
            .endAngle(function (d) {
                return d.endAngle
            });

        var svg = $container
            .append('svg')
            .attr('width', width)
            .attr('height', height);

        var tooltip = $container
            .append('div')
            .attr('class', 'tooltip animate-opacity-fast top')
            .attr('data-index', -1);

        tooltip.append('label');
        tooltip.append('div').attr('class', 'tooltip-arrow shadow');
        tooltip.append('div').attr('class', 'tooltip-arrow');

        var tooltipController = function () {
            var mouse = d3.mouse(this),
                x = mouse[0],
                y = mouse[1],
                xc = width / 2,
                yc = height / 2,
                di = Math.sqrt(Math.pow(x - xc, 2) + Math.pow(y - yc, 2)),
                data_reference = [
                    {'metric_name': 'percentile / points'}
                ];

            var j = -1;
            for (var i = 0; i < circle_data.length; i++) {
                var min = 50 + (25 * i),
                    max = 70 + (25 * i);

                if (di < 50) {
                    j = 0;
                    break;
                }

                if (di >= min && di < max) {
                    j = i + 1;
                    data_reference = circle_data;
                    break;
                }
            }

            var re = (tooltip[0][0].getBoundingClientRect()),
                xp = x - re.width / 2,
                yp = y - re.height - 5;

            if (j != -1) {
                if (j != tooltip.attr('data-index'))
                {
                    tooltip.attr('class', 'tooltip animate-opacity-fast top over');
                    tooltip.attr('data-index', j);

                    tooltip.select('label').html(data_reference[Math.max(j - 1, 0)].metric_name);
                }
                tooltip.attr('style', 'top:' + yp + 'px; left:' + xp + 'px');

                // Update tooltip to show "percentage" or "points"
                if(j == 0)
                {
                    if(yp >= 85)
                        tooltip.select('label').html("points you accumulated");
                    else
                        tooltip.select('label').html("contribution compared to other users");
                }

            } else {
                tooltip.attr('class', 'tooltip animate-opacity-fast top');
                tooltip.attr('data-index', j);
            }
        };

        $container.on('mousemove', tooltipController);
        $container.on('touchmover', tooltipController);

        var gradient = svg.append("svg:defs")
            .append("svg:linearGradient")
            .attr("id", "gradient")
            .attr("x1", "0%")
            .attr("y1", "0%")
            .attr("x2", "100%")
            .attr("y2", "100%")
            .attr("spreadMethod", "pad");

        var bg = svg.append('g')
            .attr('transform', 'translate(' + width / 2 + ',' + height / 2 + ')');

        var group = svg.append('g')
            .attr('transform', 'translate(' + width / 2 + ',' + height / 2 + ')');

        var back = bg.selectAll('g')
            .data(back_data)
            .enter()
            .append('path')
            .attr('d', arc)
            .style('fill', function (d, i) {
                return 'rgba(0, 0, 0, 0.02)'
            });

        var field = group.selectAll('g')
            .data(data)
            .enter()
            .append('path')
            .style('cursor', 'pointer')
            .on('click', function (d) {
                //console.log(d);
            })
            .style('fill', function (d, i) {
                return color(i)
            })
            .transition()
            .delay(750)
            .duration(750)
            .ease('linear')
            .attrTween('d', function (d, i) {
                var interpolate = d3.interpolate(Math.PI / 15, d.endAngle);
                return function (t) {
                    d.endAngle = interpolate(t);
                    return arcOffset(d, i);
                }
            });

        var text = group.selectAll('text')
            .data([
                {
                    value: percentile_data,
                    type: 'percent',
                    size: 30,
                    fill: '#000000'
                },
                {
                    value: point_data,
                    type: 'integer',
                    size: 14,
                    fill: '#000000'
                }
            ]).enter()
            .append('text')
            .attr('x', 0)
            .attr('y', function (d, i) {
                return 0 + 20 * i;
            })
            .attr('font-family', 'Quicksand, Helvetica Neue, Helvetica')
            .attr('font-weight', 'bold')
            .attr('fill', function (d) {
                return d.fill;
            })
            .attr('font-size', function (d) {
                return d.size;
            })
            .text(function (d) {
                if (d.type == 'percent') {
                    return d.value + "%";
                } else {
                    return d.value + " points";
                }
            })
            .style('text-anchor', 'middle')
            .style('pointer-events', 'none');

        var containerClass = $container.attr('class');
        $container.attr('class', containerClass + ' ' + 'loaded');
    });

    require(['eventEmitter', 'jquery', 'modernizr/feature-detects/file-api', 'bootstrap/popover'], function (eventEmitter, $) {
        $('[data-toggle="popover"]').popover({html: true});

        $('body').on('click', function (e) {
            $('[data-toggle="popover"]').each(function () {
                if (!$(this).is(e.target) && $(this).has(e.target).length === 0 && $('.popover').has(e.target).length === 0) {
                    $(this).popover('hide');
                }
            });
        });

        // check for File API
        if (Modernizr.filereader === true) {
            var $uploadForm = $('form.upload-form'),
                $progressBar = $('div.progress div.progress-bar'),
                $fileInput = $uploadForm.find('input[type="file"]'),
                file,
                filereader = new FileReader();

            filereader.onprogress = function (event) {
                console.log('file reading progress');
            };

            filereader.onloadend = function (event) {
                console.log('file reading complete');

                var xhr, provider, data = new FormData($uploadForm[0]);

                xhr = $.ajaxSettings.xhr();
                if (xhr.upload) {
                    xhr.upload.addEventListener('progress', function (event) {
                        console.log('file network progress', event);

                        window.requestAnimationFrame(function () {
                            var percentLoaded = event.loaded / event.total * 100;
                            $progressBar.attr('aria-valuenow', percentLoaded);
                            $progressBar.css('width', percentLoaded + '%');
                        });

                    }, false)
                }

                provider = function () {
                    return xhr;
                };

                $.ajax({
                    type: 'POST',
                    url: '/api/upload/',
                    xhr: provider,
                    dataType: 'json',
                    data: data,
                    cache: false,
                    processData: false,
                    contentType: false,
                    success: function () {
                        // upload complete
                        console.log('upload complete');
                    },
                    error: function () {
                        // upload error
                        console.warn('upload error');
                    }
                });
            };

            $fileInput.on('change', function (event) {
                event.stopPropagation();
                event.preventDefault();

                //console.log('file meta complete', event);

                file = this.files[0];

                // file reader
                $progressBar.removeClass('animate').css('width', 0).parent().css('opacity', 1);

                //filereader.readAsBinaryString(file);
                filereader.readAsText(file);
            });

            $('form.upload-form input[type="button"]').click(function () {
                $(this).blur();
                $fileInput.click();
            });

        } else {
            // error: File API is not supported
        }

    });

});
