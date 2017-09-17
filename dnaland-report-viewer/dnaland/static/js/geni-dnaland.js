
var gver = " (0009)";

/* Display a warning to the user that Geni.com connection
   requires pop-up blockers to be disabled */
function geni_connection_errors()
{
   alert("Pop-up blockers can interfere with Geni.com\n" +
         "connections. Please ensure pop-up blockers\n" +
         "are disabled, or add dna.land and geni.com\n" +
         "to the pop-up blocker's list of approved websites");
}

/* called at document.ready(), to initialize the geni object
   and get connection's status. */
function geni_initialize()
{
    /* See: https://www.geni.com/platform/developer/help/sdk_js?version=1 */

    /* Initialize Geni's object */
    Geni.init({
        app_id: 'iTOGlfoizEIV6Vwc8rTnC4l0OTj1Fhfz1Olhm4rg',
	host: 'https://geni.com',
        cookies:true,
        logging:true
    });

    /* Setup Change Event Notification */
    /* Geni.Event.bind('auth:statusChange', geni_status_changed); */

    console.log("Calling Geni.getStatus()" + gver)

    /* Get the Geni-Login status */
    Geni.getStatus(function(response) {
        if(response.status == 'authorized') {
            // User is logged in and has authorized your application.
            // You can now make authorized calls to the API.
            console.log("app is authorized" + gver);
        } else {
            // User is either logged out, has not authorized the app or both.
            console.log("app unauthorized/user logged-out" + gver);
        }
    });
}


/* Called when the user clicks on the 'Connect' button */
function geni_connect()
{
    console.log("geni_connect() " + gver);

    // Get the Geni-Login status
    console.log("geni_connect() - getStatus() " + gver);
    Geni.getStatus(function(response) {
        if(response.status == 'authorized') {
            // User is logged in and has authorized your application.
            // You can now make authorized calls to the API.
            console.log("geni_connect() - app is authorized, getting profile" + gver);

            // if we're authorized, get the profile data
            geni_get_profile();
        } else {
            // User is either logged out, has not authorized the app or both.
            console.log("geni_connect() - status not authorized - calling connect() " + gver);

	    Geni.connect(function(response) {
	        console.log("Connected to geni.com, new status = " + response + gver);
	        console.log("geni_connect(): getting profile " + gver);
	        geni_get_profile();
	    });
        }
    });
}


/* called when geni's connection status changes,
   will show/hide the connection button.

   'status' will be either 'authorized', 'unauthorized' or 'unknown'
*/
function geni_status_changed(status)
{
    console.log("Geni status changed, new status = " + status + gver);

    if(status == 'authorized') {
        // User is logged in and has authorized your application.
        // You can now make authorized calls to the API.
    }  else {
        // User canceled the popup
    }
}


/* Called after the user connected to geni (and authorized the app),
   to get the profile's information */
function geni_get_profile()
{
    console.log("Getting geni user-profile..." + gver);

    Geni.api('/profile', function(response) {
        console.log("Got geni user-profile:" + gver);
        // returns current user's profile data
	// 'response' should be a JSON with the user's geni profile data
        console.log(response);

	// Send the response back to DNA.Land's servers
	console.log("setting genidata value" + gver);
	$("#genidata").val(JSON.stringify(response))
	console.log("genidata - submitting form" + gver);
	$("#geniconnectform").submit();
	console.log("genidata - submitting form - done" + gver);
    });
}
