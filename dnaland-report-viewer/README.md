# DNA.Land Frontend Reports

A Python Flask dummy app for displaying some of the frontend features of [DNA.Land](https://dna.land). This represents a rudimentary version of what a user sees once he or she has registered on the website. As we have not released much of the backend of the website, including the database storing information used by frontend features, some components are non-functional, and the data displayed in the reports is hard-coded example data. Our hope is that the design of the reports and the HTML/js/CSS code may be of use for future crowdsourcing-based genomic research projects.


## Install

The following Python (2.7) packages are required to run the code.
```
sudo pip install flask flask_wtf flask-sqlalchemy
```

## Usage
Run `python run.py` on your local machine or a server, and in a web browser visit:

```
http://localhost:5000
```

(Or replace `localhost` with a server IP address.) This will load the main profile page, where the available features can be browsed.

## Navigation

The main page is a view of a user's profile once he or she has registered and logged in. The top left pane shows available reports, and the top right pane shows a badge representing the participation "score" of the user. The score is determined by how much demographic information (such as birth dates) the user provides through answering survey questions. The bottom panel shows files available for download by the user, including the original file the user uploaded, and an imputed genotype file displaying vastly more SNPs.

Clicking the link to Geni (a service for discovering ancestry by building and sharing family trees online) will open a pop-up prompting the user to sign into a Geni account. On the real website, this will associate your DNA.Land profile with the account, and allow you to share your Geni account with your potential relatives of DNA.Land's relative matching feature. On the real website, the sign-in button will be replaced with a "Connected" status along with an option to Disconnect. In this dummy app, this status is not recorded.

The available features include ancestry inference, relative matching, and relatives of relatives. The ancestry report includes a pie chart showing the user's percentage composition across approximately 50 different ancestry categories. The bottom panel shows these proportions on a map, along with the location where reference populations around the world were sequenced.

The relative matching feature displays other DNA.Land users who are likely to be relatives of a user based on the number and length of DNA segments shared by the two users. Each row displays information about a potential match, including a visualization of the positions of the segments overlayed over the 22 autosomes. The leftmost column includes matching users' names and contact emails, if they have chosen to provide them If a user has linked a Geni account to his or her profile, a link to the account will also be shown underneath the contact info. The relative matching feature can detect a relationship between individuals separated by at most 9 meioses. To detect even more distant relatives, we added a relatives of relatives feature, which simply returns the relatives of every one of a user's direct relatives. Each row will now include two individuals, a newly detected potential relative, and the direct relative through which he or she is connected to the current user. Users can choose to opt out of both of these features.
