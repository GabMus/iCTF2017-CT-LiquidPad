import requests, sys

IP_ARBITRO = "127.0.0.1"
PORT_ARBITRO = "5000"
ENEMY_NAME = "CuredPin"
FRIEND_NAME = "LiquidPad"

def sendFlag(flag, service):
	payload = {'flag': flag, 'team': FRIEND_NAME, 'service': service}
	r = requests.post("http://" + IP_ARBITRO + ":" + PORT_ARBITRO + "/submit", data=payload)
	return r.text

def getFlagID(service):
	payload = {'enemy_name': ENEMY_NAME, 'service': service}
	r = requests.get("http://" + IP_ARBITRO + ":" + PORT_ARBITRO + "/flagid", params=payload)
	return r.text