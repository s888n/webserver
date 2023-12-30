#!/usr/bin/env python3
def function_in_script1():
    print("""
    <html>
    <head>
        <title>Login Page</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                color: #fff;
            }
            .login-container {
                width: 300px;
                margin: 0 auto;
                padding-top: 100px;
            }
            .login-container form {
                display: flex;
                flex-direction: column;
            }
            .login-container input {
                margin-bottom: 10px;
                padding: 10px;
                font-size: 16px;
            }
            .login-container button {
                padding: 10px;
                font-size: 16px;
            }
        </style>
    </head>
    <body>
        <div class="login-container">
            <form action="userexist.py" method="post">
                <input type="text" name="username" placeholder="Username" required>
                <input type="password" name="password" placeholder="Password" required>
                <button type="submit">Login</button>
            </form>
        </div>
    </body>
    </html>
    """)



# function_in_script1()