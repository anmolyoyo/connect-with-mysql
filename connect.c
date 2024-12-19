#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>

#define BATCH_SIZE 10000
#define TOTAL_RECORDS 1400000

void generate_random_date(char *date) {
    int year = rand() % 14 + 1995;
    int month = rand() % 12 + 1;
    int day = rand() % 28 + 1;

    sprintf(date, "%04d-%02d-%02d", year, month, day);
}

void generate_random_name(char *name) {
    const char *first_names[] = {
        "John", "Jane", "Alex", "Emily", "David", "Anna", "Michael", "Sarah", 
        "James", "Linda", "Robert", "Jessica", "William", "Elizabeth", "Daniel", 
        "Mary", "Christopher", "Patricia", "Joshua", "Amanda", "Matthew", "Dorothy", 
        "Joshua", "Samantha", "Henry", "Charlotte", "Joseph", "Megan", "Brian", 
        "Laura", "Charles", "Nancy", "Andrew", "Grace", "George", "Sophie", "Jack", 
        "Olivia", "Benjamin", "Victoria", "Daniel", "Sophia", "Samuel", "Emma", 
        "Catherine", "Luke", "Natalie", "Aaron", "Madison", "Ethan", "Hannah"
    };
    
    const char *last_names[] = {
        "Smith", "Johnson", "Brown", "Williams", "Jones", "Davis", "Miller", "Wilson",
        "Moore", "Taylor", "Anderson", "Thomas", "Jackson", "White", "Harris", "Martin", 
        "Thompson", "Garcia", "Martinez", "Roberts", "Lee", "Walker", "Hall", "Young",
        "Allen", "Sanchez", "Wright", "King", "Scott", "Green", "Adams", "Baker", "Gonzalez", 
        "Nelson", "Carter", "Mitchell", "Perez", "Robinson", "Wood", "Ross", "Gray", "Murphy",
        "Bell", "Hughes", "Ward", "Evans", "Coleman", "Phillips", "Campbell", "Parker"
    };

    strcpy(name, first_names[rand() % (sizeof(first_names) / sizeof(first_names[0]))]);
    strcat(name, " ");
    strcat(name, last_names[rand() % (sizeof(last_names) / sizeof(last_names[0]))]);
}

int main() {
    MYSQL *conn;
    MYSQL_STMT *stmt;
    MYSQL_BIND bind[5];
    MYSQL_RES *res;
    int i;
    time_t start_time, end_time;
    char date[11];
    char name[100];
    int marks;
    const char *selected;
    int roll_no = 1;
    
    srand(time(NULL));

    conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if (mysql_real_connect(conn, "localhost", "root", "", "jee_mains_result", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    const char *insert_query = "INSERT IGNORE INTO jee_mains_result (roll_no, name, DOB, marks, selected) VALUES (?, ?, ?, ?, ?)";
    stmt = mysql_stmt_init(conn);
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    memset(bind, 0, sizeof(bind));
    
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = (char *)&roll_no;
    
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char *)name;
    bind[1].buffer_length = 100;
    
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char *)date;
    bind[2].buffer_length = 11;
    
    bind[3].buffer_type = MYSQL_TYPE_LONG;
    bind[3].buffer = (char *)&marks;
    
    bind[4].buffer_type = MYSQL_TYPE_STRING;
    bind[4].buffer = (char *)selected;
    bind[4].buffer_length = 3;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    start_time = time(NULL);

    for (i = 0; i < TOTAL_RECORDS; i++) {
        generate_random_date(date);
        generate_random_name(name);
        marks = rand() % 306 - 10;
        selected = (marks >= 220 && marks <= 295) ? "YES" : "NO";
        
        roll_no = i + 1;
        if (mysql_stmt_execute(stmt)) {
            fprintf(stderr, "mysql_stmt_execute() failed\n");
            mysql_close(conn);
            return EXIT_FAILURE;
        }

        if ((i + 1) % BATCH_SIZE == 0) {
            printf("Inserted %d records...\n", i + 1);
        }
    }
    
    mysql_commit(conn);
    
    end_time = time(NULL);
    printf("Total execution time: %.2f seconds\n", difftime(end_time, start_time));

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return EXIT_SUCCESS;
}
