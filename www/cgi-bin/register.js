
const fs = require('fs');
const querystring = require('querystring');
const databasefilename = 'www/cgi-bin/data.csv';

if (fs.existsSync(databasefilename) == false) {
    fs.writeFileSync(databasefilename, '');
}



async function processData() {
    const formData = querystring.parse(process.env.QUERY_STRING);

    const username = formData.username;
    const password = formData.password;
    const password2 = formData.password2;

    if (username == null || password == null || password2 == null) {
        process.stdout.write ("<h2>Invalid input</h2>");
        return;
    }

    if (password != password2) {
        process.stdout.write ("<h2> Passwords do not match< /h2>");
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
            if (username === storedUsername) {

                process.stdout.write ("<h2> Username already exists </h2>");
                return;
            }
        }
        fs.appendFile(databasefilename, username + "," + password + "\n", function (err) {
            if (err) throw err;
            process.stdout.write ('location: /project/login.html\r\n\r\n');
        });
    });
}
processData();
// process.stdout.write ("<style> h2 { font-size: 2em;background: -webkit-linear-gradient(45deg, #f3ec78, #af4261);-webkit-background-clip: text;-webkit-text-fill-color: transparent;text-align: center;margin: 20px 0;} body {background-color: black;}  </style>");
