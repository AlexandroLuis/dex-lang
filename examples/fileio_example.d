let text = readFile("data.json")
let obj = parseJSON(text)
print(obj)

let csvText = toCSV([["Name", "Age"], ["Alice", "30"], ["Bob", "25"]])
writeFile("out.csv", csvText)

let csvParsed = parseCSV(csvText)
print(csvParsed)
