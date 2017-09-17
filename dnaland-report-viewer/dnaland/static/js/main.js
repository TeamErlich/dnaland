require.config({
    shim: {
        'bootstrap': { deps: ['jquery'], exports: 'jquery'},
        'bootstrap/carousel': { deps: ['jquery'], exports: '$.fn.carousel' },
        'bootstrap/transition': { deps: ['jquery'], exports: '$.fn.transition'},
        'bootstrap/tooltip': { deps: ['jquery'], exports: '$.fn.tooltip' },
        'bootstrap/popover': { deps: ['jquery', 'bootstrap/tooltip'], exports: '$.fn.popover' },
        'bootstrap/modal': {deps: ['jquery'], exports: '$.fn.modal'},
        'facebook': {exports: 'FB'},
        'radar': {deps: ['d3'], exports: 'RadarChart'}

    },
    paths: {
        'eventEmitter': '../bower_components/eventEmitter/EventEmitter',
        'd3': '../bower_components/d3/d3.min',
        'jquery': '../bower_components/jquery/dist/jquery',
        'backbone': '../bower_components/backbone/backbone',
        'underscore': '../bower_components/lodash/lodash',
        'bootstrap': '../bower_components/bootstrap/js',
        'modernizr': '../bower_components/modernizr',
        'facebook': '//connect.facebook.net/en_US/sdk',
/*
        'signature_pad': '../bower_components/signature_pad/signature_pad',
        'persona': 'https://login.persona.org/include',
*/
        'radar': '../bower_components/radar-chart-d3/src/radar-chart'
    }
});

/* window.location.origin polyfill for slightly older browsers */
if (!window.location.origin) {
    window.location.origin = window.location.protocol + "//" + window.location.hostname + (window.location.port ? ':' + window.location.port : '');
}

/* requestAnimationFrame polyfill for slightly older browsers */
(function () {
    'use strict';

    var lastTime = 0;
    var vendors = ['webkit', 'moz'];
    for (var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x] + 'RequestAnimationFrame'];
        window.cancelAnimationFrame =
            window[vendors[x] + 'CancelAnimationFrame'] || window[vendors[x] + 'CancelRequestAnimationFrame'];
    }

    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function (callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function () {
                    callback(currTime + timeToCall);
                },
                timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };

    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function (id) {
            clearTimeout(id);
        };
}());

// Loaded Events
require(['eventEmitter', 'jquery'], function (eventEmitter, $) {
    'use strict';

    window.ee = new eventEmitter();

    $(function () {
        $('body').addClass('loaded');
    });

    $('.text-muted').click(function (e) {
        e.originalEvent.preventDefault();
    });
});


// Share Modules in Page
require(['jquery', 'bootstrap/tooltip'], function () {
    $('[data-toggle="tooltip"]').tooltip({container: 'body'});

    var $shareWidgets = $('.share-widget');

    $('html').click(function () {
        $shareWidgets.removeClass('show-widgets');
    });

    $shareWidgets.each(function () {

        var $widget = $(this),
            $share = $widget.find('a.btn-share');

        $share.click(function (event) {
            $share.tooltip('hide');
            $widget.toggleClass('show-widgets');
            event.stopPropagation();
        });

        $widget.on('click', 'div.share-pane a', function (event) {
            var $anchor = $(this),
                network = $anchor.attr('data-network');

            // --- Fetch metrics
            var point_data = 0,
                percentile_data = 0 ;
            //
            _.each(window.USER.metrics, function (metric) {
                var key = 'metric_name', value = 'metric_value';
                if (key in metric && value in metric)
                {
                    if (metric[key] == 'points')
                        point_data = metric['metric_value'];
                    if (metric[key] == 'percentile')
                        percentile_data = parseInt(metric['metric_value']);
                }
            });

            // 
            var location = encodeURIComponent(window.location.href),
                title = 'I helped science with my genome and scored ' +  point_data + ' points (' + percentile_data + 'th percentile)! Can you do better? Go to DNA.Land'

            switch (network) {
                case 'facebook':
                    window.FB.ui({
                        method: 'feed',
                        name: 'DNA.LAND',
                        caption: title,
                        picture: 'https://dna.land/static/splash/img/dna-land/logo-large.png',
                        link: 'https://dna.land'}, function (response) {

                    });

                    break;
                case 'twitter':
                    window.open('https://twitter.com/share?&text=' + title + '&url=http://&', 'twitterwindow', 'height=250, width=550, top=' + ($(window).height() / 2 - 225) + ', left=' + $(window).width() / 2 + ', toolbar=0, location=0, menubar=0, directories=0, scrollbars=0');
                    break;
                case 'google':
                    window.open('https://plus.google.com/share?url=' + location, '', 'menubar=no, toolbar=no, resizable=yes, scrollbars=yes, height=400, width=600, top=' + ($(window).height() / 2 - 225) + ', left=' + $(window).width() / 2);

                    break;
                default:
                    //console.log('error');
                    break;
            }
        });
    });
});
