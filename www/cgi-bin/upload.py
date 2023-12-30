import os
import cgi

import http.cookies

form = cgi.FieldStorage()          

cookie_string = os.environ.get('HTTP_COOKIE',"no cookie")
cookie = http.cookies.SimpleCookie()

if(cookie_string == "no cookie"):
    print("location: /project/login.html\r\n\r\n",end="")
    exit(0)

print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload Page</title>
    <style>
        body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    /* display: flex;
    justify-content: center;
    align-items: center; */
    height: 100vh;
    background-color: black;
}

.upload-container {
    background-color: rgb(50, 47, 47);
    border-radius: 8px;
    box-shadow: 0 0 50px rgba(0, 0, 0, 0.1);
    padding: 50px;
    margin: 300px;
    text-align: center;
}

h1 {
    color: white;
}

form {
    margin-top: 20px;
}



button {
    background-color: #4caf50;
    color: #fff;
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

.custom-file-upload {
    font-family: 'Montserrat', sans-serif;
            display: inline-block;
            text-transform: uppercase;
            color: #ff005a;
            text-decoration: none;
            border: 2px solid;
            background: transparent;
            padding: 10px 45px;
            font-size: 14px;
            font-weight: 700;
            margin-right: 100px;
            -webkit-transition: 0.2s all;
                transition: 0.2s all;
    }
    .custom-file-upload:hover {
            color: #8400ff;
            } 
    #file {
        display: none;
    }

button:hover {
    background-color: #45a049;
}


        .nav {
            display: flex;
            justify-content: space-between;
            align-items: center;
            justify-content: center;
            background-color: black;
        }
        .nav div {
            /* flex: 1; */
            text-align: center;
            padding: 5px;
            margin: 5px;
        }
        .nav #nev_a{
            font-family: 'Montserrat', sans-serif;
            display: inline-block;
            text-transform: uppercase;
            color: #ff005a;
            text-decoration: none;
            border: 2px solid;
            background: transparent;
            padding: 10px 45px;
            font-size: 14px;
            font-weight: 700;
            -webkit-transition: 0.2s all;
                transition: 0.2s all;
        }
        .nav #nev_a:hover {
            color: #8400ff;
            }
        #but{
            ont-family: 'Montserrat', sans-serif;
            display: inline-block;
            text-transform: uppercase;
            color: #ff005a;
            text-decoration: none;
            border: 2px solid;
            background: transparent;
            padding: 10px 45px;
            font-size: 14px;
            font-weight: 700;
            -webkit-transition: 0.2s all;
                transition: 0.2s all;
        }
        #but:hover {
            color: #8400ff;
            }
        .change{
            font-family: 'Montserrat', sans-serif;
            display: inline-block;
            text-transform: uppercase;
            color: bisque;
            text-decoration: none;
            border: 2px solid;
            background: transparent;
            padding: 10px 45px;
            font-size: 14px;
            font-weight: 700;
            -webkit-transition: 0.2s all;
                transition: 0.2s all;
        }
        .change:hover {
            color: #8400ff;
            }

    </style>
</head>
<body>
    <div class="nav">
        <div>
            <a href="../project/index.html">
                <img src="../project/images.png" alt="Description of image" width="40px" >
            </a>
        </div>
        <div>
            <a id="nev_a" href="../project/index.html">Home</a>
        </div>
        <div>
            <a  id="nev_a" href="hello.py">Videos</a>
        </div>
        <div>
            <a   class="change" href="upload.py">Upload</a>
        </div>
        <div>
            <a  id="nev_a" href="../project/aboutUs.html">about us</a>
        </div>
        <div>
            <a  id="nev_a" onclick='return logOut()'>LOG out</a>
        </div>
         <script>
    function logOut(){
        document.cookie = "sessionId=delete; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;"
        document.location.href = "../project/login.html";

        }
    </script>
    </div>
    <div class="upload-container">
        <h1>File Upload</h1>
        <form action="/" method="post" enctype="multipart/form-data">
            <label  class="custom-file-upload" for="file">Choose a file:</label>
            <input type="file" id="file" name="file">
            <button id="but" type="submit">Upload</button>
        </form>
    </div>
</body>
</html>

""")