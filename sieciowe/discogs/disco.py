import sys
import json
import requests
import time


if(len(sys.argv) != 2):
    print("Jako argument podaj identyfikator zespolu")
    sys.exit()

id_zespolu = sys.argv[1]

url = "https://api.discogs.com/artists/"

strona_zespolu = requests.get(url + id_zespolu)
if(strona_zespolu.status_code != 200):
    sys.exit()

try:
    members = json.loads(strona_zespolu.text)['members']
except KeyError:
    print("To nie jest id nalezace do zespolu")
    sys.exit()

zespoly = {}

for artysta in members:
    strona_artysty = requests.get(url + str(artysta['id']))
    while(strona_artysty.status_code != 200):
        print("czekam...\n")
        time.sleep(10)
        strona_artysty = requests.get(url + str(artysta['id']))

    groups = json.loads(strona_artysty.text)['groups']

    for group in groups:
        if group['name'] not in zespoly:
            zespoly[group['name']] = [artysta['name']]
        else: 
            zespoly[group['name']].append(artysta['name'])

for zespol in sorted(zespoly.keys()):
    if(len(zespoly[zespol]) > 1):
        print(zespol + ": " + ', '.join(map(str, zespoly[zespol])) + "\n")