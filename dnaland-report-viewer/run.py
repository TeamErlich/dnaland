import os, shutil, sys, json
from flask import Flask, redirect, url_for
from flask import render_template
from dnaland.WebForms import GeniConnectForm
from dnaland import app,csrf

def ancestry_demo_data():
    d = [
            {"type":"ASHKENAZI","percentage":45.69},
            {"type":"TURK-IRAN-CAUCASUS","percentage":19.23},
            {"type":"BALOCHI-MAKRANI-BRAHUI","percentage":12.69},
            {"type":"SBALKANS","percentage":8.1},
            {"type":"NEAREAST","percentage":5.23},
            {"type":"AMBIG_ASHKENAZI-EMED","percentage":4.49},
            {"type":"SARDINIA","percentage":2.15},
            {"type":"AMBIG_ITALY-BALKANS","percentage":1.78}
        ]
    v = 2
    return d, v

def relatives_demo_data():
    max_d = 10
    data = []
    user_pairs = {}
    data = [
        {"ind" : "User1", "d" : 2, "pot_rel" : "",
         "d_hist" : [0.1, 0.5, 0.7, 0.3, 0.2, 0.15, 0.05, 0.03, 0.03, 0.00],
         "chrom_pic" : [
             {"chrom" : 2, "bpstart" : 169984131, "bpend" : 171430090, "pop_anc" : 1},
             {"chrom" : 2, "bpstart" : 192411420, "bpend" : 199145830, "pop_anc" : 0},
             {"chrom" : 4, "bpstart" : 108753807, "bpend" : 112297062, "pop_anc" : 0},
             {"chrom" : 5, "bpstart" : 152023408, "bpend" : 155509005, "pop_anc" : 1},
             {"chrom" : 7, "bpstart" : 36773935, "bpend" : 39336042, "pop_anc" : 1},
             {"chrom" : 9, "bpstart" : 23331644, "bpend" : 25402587, "pop_anc" : 0},
             {"chrom" : 11, "bpstart" : 91087925, "bpend" : 94330237, "pop_anc" : 0},
             {"chrom" : 12, "bpstart" : 60502520, "bpend" : 63416690, "pop_anc" : 1},
             {"chrom" : 18, "bpstart" : 35513797, "bpend" : 39503140, "pop_anc" : 0},
             {"chrom" : 22, "bpstart" : 35513797, "bpend" : 39503140, "pop_anc" : 1}
         ],"sig" : True, "name" : "John Example", "email" : "john@email.com", 
         "total_numseg" :  10, "total_cm" : 100, "total_recent_cm" : 50, "longest_recent_seg" : 20,
         "geni_profile_url": "https://www.geni.com/people/William-McKinley-25th-President-of-the-USA/6000000008127235311"
        },
        {"ind" : "User2", "d" : 7, "pot_rel" : "",
         "d_hist" : [0, 0, 0.04, 0.1, 0.2, 0.3, 0.6, 0.8, 0.7, 0.6],
         "chrom_pic" : [
             {"chrom" : 1, "bpstart" : 102078434, "bpend" : 106639229, "pop_anc" : 0},
             {"chrom" : 2, "bpstart" : 86327251, "bpend" : 100652455, "pop_anc" : 0},
             {"chrom" : 2, "bpstart" : 192411420, "bpend" : 199145830, "pop_anc" : 0},
             {"chrom" : 4, "bpstart" : 108753807, "bpend" : 112297062, "pop_anc" : 0},
             {"chrom" : 5, "bpstart" : 152023408, "bpend" : 155509005, "pop_anc" : 1},
             {"chrom" : 9, "bpstart" : 23331644, "bpend" : 25402587, "pop_anc" : 0},
             {"chrom" : 11, "bpstart" : 91087925, "bpend" : 94330237, "pop_anc" : 0},
             {"chrom" : 18, "bpstart" : 35513797, "bpend" : 39503140, "pop_anc" : 0},
         ],"sig" : False, "name" : "Susan Example", "email" : "susan@email.com",
         "total_numseg" :  8, "total_cm" : 70, "total_recent_cm" : 10, "longest_recent_seg" : 10
        }
    ]
    return data


def relatives_of_relatives_demo_data():
    max_d = 10
    data = []
    user_pairs = {}
    data = [
        {"via": "user1",
         "via_deg": 2,
         "endpoint": "user2",
         "endpoint_deg": 7,
         "via_chrom": [ 
                 {"chrom" : 1, "bpstart" : 102078434, "bpend" : 106639229, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 2, "bpstart" : 86327251, "bpend" : 100652455, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 2, "bpstart" : 192411420, "bpend" : 199145830, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 4, "bpstart" : 108753807, "bpend" : 112297062, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 5, "bpstart" : 152023408, "bpend" : 155509005, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 9, "bpstart" : 23331644, "bpend" : 25402587, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 11, "bpstart" : 91087925, "bpend" : 94330237, "adj_length" : 0.5, "rel":1},
                 {"chrom" : 18, "bpstart" : 35513797, "bpend" : 39503140, "adj_length" : 0.5, "rel":1},
                      ],
        "total_len": 10,
        "via_rel": "Uncle",
        "endpoint_rel": "Second Cousin",
        "certainty": "Low"}
    ]
    contact_info = {
            "user1":    {
                "fname": "John",
                "lname": "Example",
                "email": "john@email.com",
                "birthyear": 1940,
                "geni_profile_url": None
            },
            "user2":    {
                "fname": "Susan",
                "lname": "Example",
                "email": "susan@email.com",
                "birthyear": 1960,
                "geni_profile_url": None
            }
        }
    return data, contact_info

@app.route('/profile/ancestry')
def ancestry():
    [d,v] = ancestry_demo_data()
    return render_template('profile/ancestry.html',
                            anc_data_json=json.dumps(d),
                            current_user={"first_name":"Susan","last_name":"Example"},
                            version=v,
                            meta={})


@app.route('/profile/geni-connect',methods=['POST'])
#@login_required
def profile_geni_connect():
    return redirect(url_for('profile'))

@app.route('/profile/geni-disconnect',methods=['GET'])
#@login_required
def profile_geni_disconnect():
    return redirect(url_for('profile'))

@app.route('/profile/linkage')
def relative_matching():
    meta={}
    data = relatives_demo_data()
    data_json = json.dumps(data)
    return render_template('profile/linkage-segments-report.html',
                            data_json=data_json, data=data, meta=meta)

@app.route('/relative-finder-info')
def relative_matching_info():
    meta={}
    return render_template('profile/relative-finder-info.html',meta=meta)

@app.route('/profile/network-segments-report')
def relatives_of_relatives():
    meta={}
    data_graph,contact_info = relatives_of_relatives_demo_data()
    data_json = json.dumps(data_graph)
    return render_template('profile/network-segments-report.html',
                            data_json=data_json, data_graph=data_graph,
                            contact_info=contact_info, meta=meta)

@app.route('/relatives-of-relatives-info')
def relatives_of_relatives_info():
    return render_template('profile/relatives-of-relatives-info.html',meta={})

@app.route('/faq')
def faq_info():
    return render_template("profile/faq.html",meta={})

@app.route('/geni-info')
def geni_info():
    return render_template("profile/geni-info.html",meta={})

@app.route('/')
@app.route('/main')
def profile():
    meta={}
    metrics= [
	    {"metric_name": "percentile","metric_value": 2},
	    {"metric_name": "points",    "metric_value": 10},
	    {"metric_name": "genealogy", "metric_value" : 90 },
	    {"metric_name": "genotype",  "metric_value" : 80 },
	    {"metric_name": "phenotype", "metric_value" : 70 }
	  ]
    geniform = GeniConnectForm()
    return render_template('profile/profile.html', meta=meta,
                                current_user={"first_name":"Susan (Example User)","last_name":"Example"},
                                metrics=metrics,
                                geniform=geniform)


app.run(host='0.0.0.0')
