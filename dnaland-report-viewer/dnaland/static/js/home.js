// http://simonsmith.io/modular-html-components-with-requirejs/

require(['main'], function () {
    require(['jquery', 'backbone', 'bootstrap/carousel', 'bootstrap/transition'], function ($, Backbone, carousel, transition) {
        Backbone.history.start();

        $('#information-carousel').carousel({
            interval: 5000
        });
    });
});