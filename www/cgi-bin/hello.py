#!/usr/bin/python3
import os
import cgi

print("Set-Cookie:UserID = XYZ;\r\n",end="")
print("Set-Cookie:Password = XYZ123;\r\n",end="")
print("Set-Cookie:Expires = Tuesday, 31-Dec-2021 23:12:40 GMT;\r\n",end="")
print("\r\n",end="")
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
    background-color: #fff;
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
            position: fixed;
            top: 0;
            width: 100%;
            display: flex;
            justify-content: center; 
            align-items: center; 
            background-color: black;
            z-index: 100; 
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
         .video-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
            gap: 1rem;
        }
        .video-item {
            width: 100%;
            position: relative;
        }
        .video-item video {
            width: 100%;
            height: auto;
        }
        .video-item .title {
            position: absolute;
            bottom: 0;
            background: rgba(0,0,0,0.7);
            color: #fff;
            width: 100%;
            padding: 0.5rem;
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
            <a  class="change" href="hello.py">Videos</a>
        </div>
        <div>
            <a  id="nev_a"  href="../project/upload.html">Upload</a>
        </div>
        <div>
            <a  id="nev_a" href="younsi.php">about us</a>
        </div>
    </div>
""")

# Specify the directory you want to use
directory = 'www/'

# Get list of all files in directory
files_in_directory = os.listdir(directory)

# Filter list for files ending in .mp4, .avi, .mov
video_files = [file for file in files_in_directory if file.endswith((".mp4", ".avi", ".mov"))]

# Write each video file to the HTML file as a video item
for video_file in video_files:
    print("""
    <div class="video-item">
        <video width="320" height="240" controls>
            <source src="{}" type="video/mp4">
            Your browser does not support the video tag.
        </video>
        <br>
        <div class="title">{}</div>
        <br>
        <br>
        <br>
    </div>
    """.format("../" + video_file, video_file))
print("</body>\n</html>")