const readline = require('readline');

const commandstxt = "Select mapstore command:\n" +
"ping\t- check if connection with the map store can be made\n" +
"list\t- show a list of all avaiable episodes\n" +
"install\t- install an episode\n" +
"back\t- return to MultiPekka\n"

const rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout
})

function getinput() {
	rl.question(commandstxt, function(answer) {
		process.stdout.write('\033c')
		switch (answer) {
			case "back": process.exit(0)
		}
		getinput()
	})
}

getinput()
