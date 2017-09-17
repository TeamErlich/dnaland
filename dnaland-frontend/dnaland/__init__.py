# -*- coding: utf-8 -*-
import sys,os
from flask import Flask
from flask_wtf.csrf import CSRFProtect

def create_dnaland_dummy_flask_app():
    path = os.path.dirname(os.path.realpath(__file__))
    static_folder = path + "/static"
    template_folder = path + "/templates"
    if not os.path.exists(static_folder):
        sys.exit("can't find static folder '%s'" % (static_folder))
    if not os.path.exists(template_folder):
        sys.exit("can't find template folder '%s'" % (template_folder))

    app = Flask(__name__,
                static_url_path='/static',
                static_folder=static_folder,
                template_folder=template_folder)
    return app

app = create_dnaland_dummy_flask_app()
app.secret_key = "supersecretkey"
csrf = CSRFProtect(app)

