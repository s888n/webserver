const fs = require('fs');
const querystring = require('querystring');
const databasefilename = 'www/cgi-bin/data.csv';
const httpCookie = process.env.HTTP_COOKIE;
const sessionidDatabase = 'www/cgi-bin/sessionid.csv';
if (fs.existsSync(sessionidDatabase) == false) {
    fs.writeFileSync(sessionidDatabase, '');
}
if (fs.existsSync(databasefilename) == false) {
    process.stdout.write ("location: /project/register.html\r\n\r\n");
}

const cookies = querystring.parse(httpCookie, '; ');
let sessionIdExists = false;
if (cookies.sessionId) {

    fs.readFile(sessionidDatabase, 'utf8', function (err, data) {
        if (err) {
            console.error(err);
            return;
        }
        const lines = data.split('\n');
        for (let i = 0; i < lines.length; i++) {
            const entry = lines[i].split(',');
            const storedSessionId = entry[0];
            if (cookies.sessionId === storedSessionId) {
                
                sessionIdExists = true;
            }
        }
    }

    );
    }


async function processData() {
    const formData = querystring.parse(process.env.QUERY_STRING);

    const username = formData.username;
    const password = formData.password;

    if (username == null || password == null) {
        process.stdout.write ("<h2>Invalid input</h2>");
        return;
    }
    // Read the CSV file
    fs.readFile(databasefilename, 'utf8', function (err, data) {
        if (err) {
            console.error(err);
            return;
        }
        // Split the file content into lines
        const lines = data.split('\n');
        
        // Search for a matching entry;
        for (let i = 0; i < lines.length; i++) {
            const entry = lines[i].split(',');
            const storedUsername = entry[0];
            const storedPassword = entry[1];
            if (username === storedUsername && password === storedPassword ) {

                if(sessionIdExists == false)
                {
                    var key = Math.random().toString(36).substring(2, 15) + Math.random().toString(36).substring(2, 15);
                    fs.appendFile(sessionidDatabase, key + "\n", function (err) {
                        if (err) throw err;
                    });
                    process.stdout.write ("set-cookie: sessionId="+ key + "; path=/\r\n");
                }
                // process.stdout.write ("set-cookie: sessionId=deleted; path=/ ; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n");
                process.stdout.write ("location: /project/index.html\r\n\r\n");
                return;
            }
            
        }
        process.stdout.write ("location: /project/login.html\r\n\r\n");
    });
}
processData();