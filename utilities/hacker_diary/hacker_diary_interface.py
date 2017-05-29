#!/usr/bin/env python3
# hacker_diary_interface by Gabriele Musco (GabMus)
"""
Copyright (C) 2017 Gabriele Musco

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""


from urllib.parse import urlencode
import requests
import json

HOST='http://192.168.56.102'
PORT='20130'
SERVER=':'.join([HOST, PORT])

def makeRequest(uri, method, data=None, textOnly=False, token=None):
    if data and method=='POST':
        if token:
            r = requests.post(SERVER+uri, data=data, headers={'Authorization': 'JWT '+token})
        else:
            r = requests.post(SERVER+uri, data=data)
    else:
        if token:
            r = requests.get(SERVER+uri, headers={'Authorization': 'JWT '+token})
        else:
            r = requests.get(SERVER+uri)
    if textOnly:
        return r.text
    else:
        return r.json()


def register(username, password, first_name=None, last_name=None, email=None):
    data={'username':username, 'password':password}
    if first_name:
        data['first_name']=first_name
    if last_name:
        data['last_name']=last_name
    if email:
        data['email']=email
    return makeRequest('/register/', 'POST', data=data)

def login(username, password):
    data={'username': username, 'password': password}
    return makeRequest('/login/', 'POST', data=data)

def users():
    return makeRequest('/users/', 'GET')

def getUserDict(username):
    userslist=users()
    for user in userslist:
        if user['username'] == username:
            return user
    print('User not found')
    return None

def user(uid):
    return makeRequest('/users/'+uid+'/', 'GET')

def publicKey():
    return makeRequest('/public_key/', 'GET')

def entries(token):
    return makeRequest('/entries/', 'GET', token=token)

def entriesPost(token, entry):
    data={'entry': entry}
    return makeRequest('/entries/', 'POST', data=data, token=token)

def getEntry(entryid, token):
    return makeRequest('/entries/'+entryid, 'GET', token=token)
