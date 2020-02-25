from flask_wtf import Form
from wtforms import HiddenField

class GeniConnectForm(Form):
    """
    Geni-Connect <Form>

    A Hidden Form on the main profile page.
    Will be submitted by Javascript code after the user connects to
    geni, and the client-side Javascript fetches the geni.com profile
    data.

    The JSON geni profile will then be transmitted back to the DNA.Land server.
    """
    genidata = HiddenField('genidata')
