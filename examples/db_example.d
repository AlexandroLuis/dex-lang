db = Database.connect("sqlite://test.db")

db.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT);")
db.execute("INSERT INTO users (name) VALUES ('Alice');")
db.execute("INSERT INTO users (name) VALUES ('Bob');")

results = db.query("SELECT * FROM users;")

for row in results {
    print("User ID: " + row[0] + ", Name: " + row[1])
}
