connStr = getEnv("DATABASE_URL")

if (connStr == "") {
    print("Error: DATABASE_URL not set")
    exit(1)
}

db = Database.connect(connStr)

results = db.query("SELECT * FROM users;")

for row in results {
    print("User:", row[1])
}
