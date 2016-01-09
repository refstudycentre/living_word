(function ($) {
    Drupal.behaviors.livingWordQuoteBehavior = {
        attach: function (context, settings) {

            // find the form elements
            var form_id = Drupal.settings.living_word_quote.form_id;
            var form = $('#'+form_id+" div.fieldset-wrapper", context);
            var form_start = form.find("input.living-word-quote-start");
            var form_text = form.find("input.living-word-quote-text");

            // get the source text
            var source_text = Drupal.settings.living_word_quote.source_text;

            // hide the text fields
            // TODO: uncomment the following line once the eye candy is done
            //form.find('div.form-item').hide();

            // build the user interface
            var textarea = $("<textarea>"+source_text+"</textarea>");
            form.append(textarea);

            // act on text being selected
            textarea.select(function (select_event) {

                // inspect the selection
                var position = textarea.selection('getPos');
                var text = textarea.selection();

                // fill in the form
                form_start.val(position.start);
                form_text.val(text);

                // TODO: eye candy selection thingy (like on android phones)

            })

        }
    };
})(jQuery);