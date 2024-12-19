import mysql.connector
import random
from faker import Faker
import time

fake = Faker()

l = list(range(1400000))
random.shuffle(l)

connection = mysql.connector.connect(
    host='localhost',
    user='root',
    password='',
    database='jee_mains_result'
)

cursor = connection.cursor()

insert_query = """
INSERT IGNORE INTO jee_mains_result (roll_no, name, DOB, marks, selected)
VALUES (%s, %s, %s, %s, %s);
"""

batch_size = 10000
data_batch = []
start_time = time.time()

for idx, roll_no in enumerate(l):
    year = str(random.randint(1995, 2008))
    month = str(random.randint(1, 12)).zfill(2)
    day = str(random.randint(1, 28)).zfill(2)
    date = f"{year}-{month}-{day}"

    name = fake.name()
    marks = random.randint(-10, 295)
    selected = 'YES' if 220 <= marks <= 295 else 'NO'

    data_batch.append((roll_no, name, date, marks, selected))

    if (idx + 1) % batch_size == 0:
        cursor.executemany(insert_query, data_batch)
        connection.commit()
        print(f"Inserted {idx + 1} records...")
        data_batch = []

if data_batch:
    cursor.executemany(insert_query, data_batch)
    connection.commit()

end_time = time.time()
print(f"Total execution time: {end_time - start_time:.2f} seconds")

cursor.close()
connection.close()
