require(['main'], function () {
    'use strict';

    require(['jquery', 'bootstrap/carousel', 'bootstrap/transition', 'bootstrap/tooltip'], function ($) {

        // genotype sources : [23andMe,
        $('a.show-well').click(function () {
            var $anchor = $(this);
            $anchor.parent().toggleClass('selected');

            $('a.show-well').each(function () {
                var $currentAnchor = $(this);

                if (($currentAnchor[0] !== $anchor[0] && $currentAnchor.parent().hasClass('selected')) ||
                    ($currentAnchor[0] === $anchor[0] && !$currentAnchor.parent().hasClass('selected'))) {

                    $(this).parent().removeClass('selected');
                    $('#' + $currentAnchor.attr('data-well-id')).parent().removeClass('show');
                } else if ($currentAnchor[0] === $anchor[0] && $anchor.parent().hasClass('selected')) {
                    $('#' + $anchor.attr('data-well-id')).parent().addClass('show');
                }
            });

        });

        // carousels
        $('#23andMe-walkthrough').carousel({
            interval: false
        });

        // tooltips
        $('[data-toggle="tooltip"]').tooltip();

        // update progress
        var $progressContainer = $('.progress-container');
        var current_index = 0;
        var $progressBar = $progressContainer.find('div.stepped-progress-bar');
        var transEndEventNames = {
            'WebkitTransition': 'webkitTransitionEnd',  // Saf 6, Android Browser
            'MozTransition': 'transitionend',           // only for FF < 15
            'transition': 'transitionend'               // IE10, Opera, Chrome, FF 15+, Saf 7+
        };

        $progressBar.on(transEndEventNames[Modernizr.prefixed('transition')], function () {
            var $steps = $progressContainer.find('.step');

            $steps.each(function (index) {
                if (index <= current_index) {
                    $(this).addClass('active');
                } else {
                    $(this).removeClass('active');
                }
            })
        });

        window.ee.addListener('update-form-progress', function (param_current_index) {
            var $steps = $progressContainer.find('.step'),
                unitPercent = 100 / ($steps.length);

            // update current index
            current_index = param_current_index;

            // step update on transition
            $progressBar.css('width', unitPercent * (current_index + 1) + '%');

        });

	// Set the progress bar to this initial step (0=first)
	// This variable is set in the main HTML page.
        window.ee.emitEvent('update-form-progress', [window.progress_bar_pos]);




	// This function cycles throw all the form sections, defined as:
        //      <section class="form-flow">
        // When called, it will switch to the next one.
	// If the last one is selected, will switch back to the first.
	//
        // Will also emit an event to upload the top progress bar.
        var $forms = $('section.form-flow'); //get matching <section> DOM objects
	var switch_to_next_section = function() {
            var $current_form, $next_form,
                current_form_index = 0, next_form_index;

            $forms.each(function (index) {
                $current_form = $(this);

                if ($current_form.hasClass('active')) {
                    $current_form.removeClass('active');
                    current_form_index = index;
                }
            });

            next_form_index = (current_form_index + 1) % $forms.length;
            $next_form = $($forms[next_form_index]);

            $next_form.addClass('active');
            window.ee.emitEvent('update-form-progress', [next_form_index]);
	};



	// The 'select file' button (FileUpload object)
	// When the user selects a file,
	// Advanced on the next step
	$("#select-file-btn").change(function(){
	    //New file selected by the user, move on to the next 'step'
	    switch_to_next_section();
	});


	// The questionnaire button:
	// When clicked, simply move on to the next 'step'
	$("#questions-cont-btn").click(function(){
	    var o = $("#user_first_name")
	    var v = $.trim(o.val());
            if (v == "") {
		o.focus()
		alert("Please enter your first name to continue");
		return ;
            }
	    var o = $("#user_last_name")
	    var v = $.trim(o.val());
            if (v == "") {
		o.focus()
		alert("Please enter your last name to continue");
		return ;
            }

	    // This will submit the entire form with
	    // the selected file and all the other questionnaire fields.
	    var $form = $("#upload-form");
	    $form.submit();
	});


        // "I AGREE" button in the consent step.
        // If the user marked the checkbox - submit the form.
	// otherwise, alert.
        $('#consent-agree-btn').click(function() {

            var $checkbox_marked = $('#agree_checkbox').is(':checked');
            if (! $checkbox_marked ) {
               alert("Please Mark the checkbox to continue");
               return ;
            }

	    switch_to_next_section();
	});


    });
});
