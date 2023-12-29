#!/usr/bin/env python3
import csv
import login
import os
import hello
from urllib.parse import parse_qs


def check_user_exists():
    body = os.environ.get('QUERY_STRING')
    parsed_body = parse_qs(body)

    username = parsed_body.get("username")[0]
    password = parsed_body.get("password")[0]
    if username == "hamza" and password == "younsi":
        return True
    else:
        return False

user_exists = check_user_exists()
if user_exists == True:
    hello.video_maker()
else:
    login.function_in_script1()
